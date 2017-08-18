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

    SingleWriterRingBuffer(std::size_t capacity)
        : begin(allocate(++capacity)), // pointer to first element in buffer
          end(begin + capacity),     // pointer to last element in buffer
          head(begin),
          tail(begin)
    {}

    ~SingleWriterRingBuffer()
    {
        destroy();
    }

    template<typename ...Args>
    std::enable_if<std::is_nothrow_constructible<T, ...Args>::value>::type
    emplace_front(Args &&..args)
    {
        // advance head
        std::atomic_thread_fence(std::memory_order_acquire);


        T *prev_head = last;

        if (head.compare_exchange_weak(prev_head,
                                       begin,
                                       std::memory_order_acquire,
                                       std::memory_order_acquire)) {
            slot = begin;

        } else {

            slot = kk
            while ()


        }

        T *const slot = head.fetch_add(1,
                                       std::memory_order_relaxed);

        std::atomic_thread_fence(std::memory_order_release);

        T *slot;

        while (1) {
                next_slot = begin;
                break;
            }

            next_slot = head.fetch_add(1,
                                       std::memory_order_relaxed);

            assert(next_slot < end)


        }


            next_slot = begin;

            } else {

            }
        }

            
        // advance tail unconditionally

        if 
        T *prev_tail = tail.exchange
        T *const prev_tail = tail.fetch_add(1,
                                            std::memory_order_relaxed);

        // if previous tail overlaps with head, destroy
        if (prev_tail == head.load(std::memory_order_relaxed)) {
            prev_tail->~T();

            // if tail has advanced off end, restore to begining
            (void) tail.compare_exchange_weak(end,
                                              begin,
                                              std::memory_order_relaxed,
                                              std::memory_order_relaxed);

        } else {
            tail.store(prev_tail,
                       std::memory_order_relaxed); // restore previous value
        }

        // construct into empty memory
        (void) new(head) T(std::forward<Args>(args)...);
    }

    std::enable_if<std::is_nothrow_destructible<T>::value, bool>::type
    pop_back(T &value)
    {

        T *const current_head = head.load(std::memory_order_relaxed);

        const bool not_empty = (   tail.load(std::memory_order_relaxed)
                                != head.load(std::memory_order_relaxed));

        const bool empty = tail.compare_exchange_weak()

        if (not_empty) {
            tail->~T();

            // advance tail
            ++tail;
            if (tail == end)
                tail = begin;

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
        // if (capacity == 0)
        //     throw std::runtime(
        //         "SingleWriterRingBuffer::allocate() -- capacity must be nonzero"
        //     );

        T *const buffer = static_cast<T *>(
            std::malloc(sizeof(T) * capacity)
        );

        if (!buffer)
            throw std::bad_alloc();

        return buffer;
    }

    inline std::enable_if<std::is_trivially_destructible<T>::value>::type
    destroy() noexcept
    {
        std::free(static_cast<void *>(begin));
    }

    inline std::enable_if<!std::is_trivially_destructible<T>::value>::type
    destroy() noexcept(std::is_nothrow_destructible<T>::value)
    {
        T *ptr = tail;

        if (ptr > head) {
            while (ptr < end) {
                ptr->T();
                ++ptr;
            }

            ptr = begin;
            do {
                ptr->T();
            } while (++ptr < head);

        } else if (ptr != nullptr) {
            while (ptr < head) {
                ptr->T();
                ++ptr;
            }
        }

        std::free(static_cast<void *>(begin));
    }

    T *const   begin;
    const T *const   end;
    std::atomic<T *> head;
    std::atomic<T *> tail;
}; // class SingleWriterRingBuffer

#endif // ifndef SINGLE_BUFFER_RING_BUFFER_HPP
