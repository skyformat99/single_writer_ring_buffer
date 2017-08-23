#include <utility>       // std::move
#include <string>        // std::string
#include "gtest/gtest.h" // TEST, ASSERT_*

// SingleWriterRingBuffer
#include "single_writer_ring_buffer/single_writer_ring_buffer.hpp"



class CtorCounter
{
public:
    CtorCounter(int &count_constructed)
    {
        ++count_constructed;
    }
}; // class CtorCounter



class DtorCounter
{
public:
    DtorCounter(int &count_destroyed)
        : count_destroyed(count_destroyed)
    {}

    ~DtorCounter()
    {
        ++count_destroyed;
    }

private:
    int &count_destroyed;
}; // class DtorCounter



TEST(constructor, capacity_gt_zero)
{
    ASSERT_THROW(
        SingleWriterRingBuffer<int>(0),
        std::invalid_argument
    ) << "constructor succeeded with capacity of 0";
}


TEST(constructor, no_elements_constructed)
{
    struct DoNotConstruct
    {
        DoNotConstruct()
        {
            FAIL() << "DoNotConstruct() called";
        }
    }; // struct DoNotConstruct

    SingleWriterRingBuffer<DoNotConstruct>(200);

    SUCCEED();
}


TEST(destructor, all_elements_destroyed)
{
    unsigned int count_destroyed = 0;

    {
        SingleWriterRingBuffer<DtorCounter> buffer(1000);
    }

    ASSERT_EQ(0,
              count_destroyed) << "destructor called on empty buffer";

    {
        SingleWriterRingBuffer<DtorCounter> buffer(1000);

        for (unsigned int i = 0; i < 1000; ++i)
            buffer.emplace_front(count_destroyed);
    }

    ASSERT_EQ(1000,
              count_destroyed) << "count constructions != count destroyed";


    {
        SingleWriterRingBuffer<DtorCounter> buffer(1000);

        for (unsigned int i = 0; i < 10000; ++i)
            buffer.emplace_front(count_destroyed);
    }

    ASSERT_EQ(10000,
              count_destroyed) << "count constructions != count destroyed";
}


TEST(insertion, all_elements_destroyed)
{
    unsigned int count_destroyed = 0;

    {
        SingleWriterRingBuffer<DtorCounter> buffer(1000);

        for (unsigned int i = 0; i < 10000; ++i)
            buffer.emplace_front();
    }

    ASSERT_EQ(10000,
              count_destroyed) << "count constructions != count destroyed";
}
