#include <stdexcept>     // std::exception
#include "gtest/gtest.h" // TEST, ASSERT_*

// SingleWriterRingBuffer
#include "single_writer_ring_buffer/single_writer_ring_buffer.hpp"



TEST(destruction, no_elements_destroyed)
{
    struct DoNotDestroy
    {
        ~DoNotDestroy()
        {
            throw std::exception();
        }
    }; // struct DoNotDestroy

    ASSERT_NO_THROW(
        SingleWriterRingBuffer<DoNotDestroy>(200)
    ) << "~DoNotDestroy() called";
}


TEST(destruction, all_elements_destroyed)
{
    unsigned int count_destroyed;

    class DtorCounter
    {
    public:
        DtorCounter(unsigned int &count_destroyed)
            : count_destroyed(count_destroyed)
        {}

        ~DtorCounter()
        {
            ++count_destroyed;
        }
    private:
        unsigned int &count_destroyed;
    }; // class DtorCounter


    count_destroyed = 0;
    {
        SingleWriterRingBuffer<DtorCounter> buffer(1000);

        for (unsigned int i = 0; i < 1000; ++i)
            buffer.emplace_front(count_destroyed);
    }
    ASSERT_EQ(1000,
              count_destroyed) << "count constructions != count destroyed";


    count_destroyed = 0;
    {
        SingleWriterRingBuffer<DtorCounter> buffer(100);

        for (unsigned int i = 0; i < 10000; ++i)
            buffer.emplace_front(count_destroyed);
    }
    ASSERT_EQ(10000,
              count_destroyed) << "count constructions != count destroyed";
}
