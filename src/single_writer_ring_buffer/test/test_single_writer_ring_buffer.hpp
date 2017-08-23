#include <utility>       // std::move
#include <string>        // std::string
#include "gtest/gtest.h" // TEST, ASSERT_*

// SingleWriterRingBuffer
#include "single_writer_ring_buffer/single_writer_ring_buffer.hpp"



TEST(constructor, capacity_gt_zero)
{
    ASSERT_NO_THROW(
        SingleWriterRingBuffer<int>(1)
    ) << "constructor threw with valid capacity";

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


TEST(constructor, all_elements_constructed)
{

    unsigned int count_constructed;

    struct CtorCounter
    {
    public:
        CtorCounter()
        {
            ++count_constructed;
        }
    }; // struct CtorCounter

    count_constructed = 0;

    SingleWriterRingBuffer<CtorCounter> buffer(100);

    for (unsigned int i = 0; i < 10; ++i)
            buffer.emplace_front();

    ASSERT_EQ(10,
              count_constructed) << "count_constructed != count inserted";

    count_constructed = 0;

    for (unsigned int i = 0; i < 1000; ++i)
            buffer.emplace_front();

    ASSERT_EQ(1000,
              count_constructed) << "count_constructed != count inserted";
}


TEST(destructor, no_elements_destroyed)
{
    struct DoNotDestroy
    {
        ~DoNotDestroy()
        {
            FAIL() << "~DoNotDestroy() called";
        }
    }; // struct DoNotDestroy

    SingleWriterRingBuffer<DoNotDestroy>(200);

    SUCCEED();
}


TEST(destructor, all_elements_destroyed)
{
    unsigned int count_destroyed;

    struct DtorCounter
    {
        ~DtorCounter()
        {
            ++count_destroyed;
        }
    }; // struct DtorCounter


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
