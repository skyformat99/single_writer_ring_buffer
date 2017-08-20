#ifndef SINGLE_BUFFER_RING_BUFFER_HPP
#define SINGLE_BUFFER_RING_BUFFER_HPP

// EXTERNAL DEPENDENCIES
// =============================================================================
#include <cstdlib>    // std::[malloc|free]
#include <new>        // std::bad_alloc
#include <stdexcept>  // std::runtime_error
#include <typetraits> // std::enable_if
#include <utility>    // std::[move|forward]
#include <atomic>     // std::atomic[_thread_fence]



template<typename T>
class SingleWriterRingBuffer
{
private:
    template <bool E, typename R = void>
    using enable_if_t = typename std::enable_if<E, R>::type;

    template <typename ...Args, typename R = void>
    using enable_if_trivially_destructible = enable_if_t<
        std::is_trivially_destructible<T>::value,
        R
    >;

    template <typename R = void>
    using enable_if_nothrow_move_constructible_and_destructible = enable_if_t<
        (   std::is_nothrow_move_constructible<T>::value
         && std::is_nothrow_destructible<T>::value),
        R
    >;

    template <typename R = void>
    using enable_if_not_nothrow_move_constructible_and_destructible = enable_if_t<
        !(   std::is_nothrow_move_constructible<T>::value
          && std::is_nothrow_destructible<T>::value),
        R
    >;


public:
    SingleWriterRingBuffer(const std::size_t capacity)
        : first(allocate(capacity)), // pointer to first element in buffer
          last(first + capacity),    // pointer to last element in buffer
          head(first),
          tail(first)
    {}

    ~SingleWriterRingBuffer()
    {
        destroy();
    }

    template<typename ...Args>
    void
    emplace_front(Args &&...args)
    {
        // advance head
        // std::atomic_thread_fence(std::memory_order_acquire);
        T *next_head;
        T *next_tail;

        T *const current_head = head.load(std::memory_order_relaxed);

        if (current_head == last) {
            next_head = first;
            next_tail = next_head + 1;

        } else {
            next_head = current_head + 1;
            next_tail = (next_head == last) ? first : (next_head + 1);
        }

        // copy next_head into temp variable in case CAS fails and resets
        T *overlap = next_head;

        // advance tail if next_head would clobber
        if (tail.compare_exchange_strong(overlap,
                                         next_tail,
                                         std::memory_order_relaxed,
                                         std::memory_order_relaxed))
            overlap->~T(); // make room for next head

        (void) new(current_head) T(std::forward<Args>(args)...);

        // ensure construction writes committed before advancing head
        std::atomic_thread_fence(std::memory_order_release);

        // advance head
        head.store(next_head,
                   std::memory_order_relaxed);
    }


    enable_if_nothrow_move_constructible_and_destructible<bool>
    try_pop_back(T &value)
    {
        T *current_tail;

        // acquire tail
        do {
            current_tail = tail.exchange(nullptr,
                                         std::memory_order_relaxed);
        } while (current_tail == nullptr);

        const bool not_empty = (   current_tail
                                != head.load(std::memory_order_relaxed));

        // release tail ASAP
        if (not_empty) {
            // advance tail
            tail.store((current_tail == last) ? first : (current_tail + 1),
                       std::memory_order_relaxed);

            // ensure tail is not accessed mid-construction (from an insertion)
            std::atomic_thread_fence(std::memory_order_acquire);

            value = std::move(*current_tail);

            current_tail->~T();

        } else {
            // replace
            tail.store(current_tail,
                       std::memory_order_relaxed);
        }

        return not_empty;
    }


    enable_if_not_nothrow_move_constructible_and_destructible<bool>
    try_pop_back(T &value)
    {
        T *current_tail;

        // acquire tail
        do {
            current_tail = tail.exchange(nullptr,
                                         std::memory_order_relaxed);
        } while (current_tail == nullptr);

        T *next_tail = current_tail;

        const bool not_empty = (   current_tail
                                != head.load(std::memory_order_relaxed));

        if (not_empty) {
            // ensure tail is not accessed mid-construction (from an insertion)
            std::atomic_thread_fence(std::memory_order_acquire);

            try {
                value = std::move(*current_tail);

                // advance tail
                next_tail = (current_tail == last) ? first : (current_tail + 1);

                current_tail->~T();

            } catch (...) {
                // advance tail if move succeeded
                tail.store(next_tail,
                           std::memory_order_relaxed);

                throw; // reraise
            }
        }

        // replace tail
        tail.store(current_tail,
                   std::memory_order_relaxed);

        return not_empty;
    }




private:
    inline static T *
    allocate(const std::size_t capacity)
    {
        if (capacity == 0)
            throw std::runtime(
                "SingleWriterRingBuffer::allocate() -- capacity must be nonzero"
            );

        T *const buffer = static_cast<T *>(
            std::malloc(sizeof(T) * (capacity + 1))
        );

        if (!buffer)
            throw std::bad_alloc();

        return buffer;
    }

    inline std::enable_if<std::is_trivially_destructible<T>::value>::type
    destroy() noexcept
    {
        std::free(static_cast<void *>(first));
    }

    inline std::enable_if<!std::is_trivially_destructible<T>::value>::type
    destroy() noexcept(std::is_nothrow_destructible<T>::value)
    {
        T *ptr = tail;

        if (ptr > head) {
            while (ptr < last) {
                ptr->T();
                ++ptr;
            }

            ptr = first;
            do {
                ptr->T();
            } while (++ptr < head);

        } else if (ptr != nullptr) {
            while (ptr < head) {
                ptr->T();
                ++ptr;
            }
        }

        std::free(static_cast<void *>(first));
    }

    T *const   first;
    const T *const   last;
    std::atomic<T *> head;
    std::atomic<T *> tail;
}; // class SingleWriterRingBuffer

#endif // ifndef SINGLE_BUFFER_RING_BUFFER_HPP
