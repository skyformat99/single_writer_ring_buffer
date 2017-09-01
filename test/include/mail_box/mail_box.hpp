#ifndef MAIL_BOX_MAIL_BOX_HPP
#define MAIL_BOX_MAIL_BOX_HPP

// EXTERNAL DEPENDENCIES
// =============================================================================
#include <utililty> // std::move
#include <iterator> // std::iterator
#include <atomic>   // std::atomic



template<typename T, std::size_t N>
class MailBox
{
public:
    MailBox()
        : buffer(),
          cursor(&buffer[0])
    {}

    void
    push_back(const T &value)
    {
        push_back(T(value));
    }

    void
    push_back(T &&value)
    {
        T *const next = cursor.fetch_add(std::memory_order_relaxed);
        *next         = std::move(value);
    }

private:
    template<typename T>
    class iterator_base : public std::iterator<std::random_access_iterator_tag, T>
    {
    public:
        explicit
        iterator_base(T *const ptr)
            : cursor(ptr)
        {}

        // advancement
        // ---------------------------------------------------------------------
        iterator_base&
        operator++()
        {
            ++cursor;
            return *this;
        }

        iterator_base
        operator++()(int)
        {
            T *const prev_cursor = cursor++;
            return iterator_base(prev_cursor);
        }

        iterator_base &
        operator+=(const difference_type pos)
        {
            cursor += pos;
            return *this;
        }

        iterator_base&
        operator--()
        {
            --cursor;
            return *this;
        }

        iterator_base
        operator--()(int)
        {
            T *const prev_cursor = cursor--;
            return iterator_base(prev_cursor);
        }

        iterator_base &
        operator-=(const difference_type pos)
        {
            cursor -= pos;
            return *this;
        }

        // comparison
        // ---------------------------------------------------------------------
        bool
        operator==(const iterator_base &other)
        {
            return cursor == other.cursor;
        }

        bool
        operator!=(const iterator_base &other)
        {
            return cursor != other.cursor;
        }

        bool
        operator<const iterator_base &other)
        {
            return cursor < other.cursor;
        }

        bool
        operator>const iterator_base &other)
        {
            return cursor > other.cursor;
        }

        bool
        operator<=const iterator_base &other)
        {
            return cursor <= other.cursor;
        }

        bool
        operator>=const iterator_base &other)
        {
            return cursor >= other.cursor;
        }

        // access
        // ---------------------------------------------------------------------
        reference
        operator*() const
        {
            return *cursor;
        }

        pointer
        operator->() const
        {
            return cursor;
        }


    private:
        T *cursor;
    }; // class iterator_base

public:
    typedef iterator_base<T>       iterator;
    typedef iterator_base<const T> const_iterator;

    iterator begin() const noexcept
    {
        return iterator(&buffer[0]);
    }

    iterator end() const noexcept
    {
        return iterator(cursor.fetch(std::memory_order_relaxed));
    }

    const_iterator cbegin() const noexcept
    {
        return const_iterator(&buffer[0]);
    }

    const_iterator cend() const noexcept
    {
        return const_iterator(cursor.fetch(std::memory_order_relaxed));
    }

private:
    T buffer[N];
    T *cursor;
}; // class MailBox

#endif // ifndef MAIL_BOX_MAIL_BOX_HPP
