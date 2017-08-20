#ifndef SINGLE_BUFFER_RING_BUFFER_HPP
#define SINGLE_BUFFER_RING_BUFFER_HPP

// EXTERNAL DEPENDENCIES
// =============================================================================
#include <cstdlib>    // std::[malloc|free]
#include <new>        // std::bad_alloc
#include <stdexcept>  // std::runtime_error
#include <typetraits> // std::enable_if
#include <utility>    // std::move
#include <vector>     // std::vector
#include <atomic>     // std::atomic[_thread_fence]



template<typename T>
class SingleWriterRingBuffer
{

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
    std::enable_if<std::is_nothrow_constructible<T, ...Args>::value>::type
    emplace_front(Args &&...args)
    {
        // advance head
        // std::atomic_thread_fence(std::memory_order_acquire);
        T *next_head;
        T *next_tail;

        T *const slot = head.load(std::memory_order_relaxed);

        if (slot == last) {
            next_head = first;
            next_tail = next_head + 1;

        } else {
            next_head = slot + 1;
            next_tail = (next_head == last) ? first : (next_head + 1);
        }

        // advance tail if next_head would clobber
        if (tail.compare_exchange_weak(next_head,
                                       next_tail,
                                       std::memory_order_relaxed,
                                       std::memory_order_relaxed))
            next_head->~T(); // make room for next head

        (void) new(slot) T(std::forward<Args>(args)...);

        // advance head
        head.store(next_head,
                   std::memory_order_relaxed);
    }

    std::enable_if<std::is_nothrow_destructible<T>::value, bool>::type
    try_pop_back(T &value)
    {

        T *const current_head = head.load(std::memory_order_relaxed);

        const bool not_empty = (   tail.load(std::memory_order_relaxed)
                                != head.load(std::memory_order_relaxed));

        const bool empty = tail.compare_exchange_weak()

        if (not_empty) {
            tail->~T();

            // advance tail
            ++tail;
            if (tail == last)
                tail = first;

            // if tail overlaps head, mark empty
            if (tail == head)
                tail = nullptr;
        }

        unlock();

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
