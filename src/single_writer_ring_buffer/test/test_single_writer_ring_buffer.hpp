#include <utility>       // std::move
#include <string>        // std::string
#include "gtest/gtest.h" // TEST, ASSERT_*

// SingleWriterRingBuffer
#include "single_writer_ring_buffer/single_writer_ring_buffer.hpp"


TEST(constructor, capacity_gt_zero)
{
    try {
        SingleWriterRingBuffer<int>(0);

    } catch (const std::runtime_error &error) {
        SUCCEED();
    }

    FAIL() << "constructor succeeded with capacity of 0";
}


TEST(constructor, no_elements_constructed)
{
    unsigned int count_constructed = 0;

    struct CtorCounter
    {
        CtorCounter() { ++count_constructed; }
    }; // struct CtorCounter

    SingleWriterRingBuffer<CtorCounter>(200);

    ASSERT_EQ(0, count_constructed) << "elements unintentionally constructed";
}


TEST(constructor, all_elements_destroyed)
{
    unsigned int count_destroyed = 0;
    static const unsigned int count_constructed = 10000;

    struct DtorCounter
    {
        ~DtorCounter() { ++count_destroyed; }
    }; // struct DtorCounter


    {
        SingleWriterRingBuffer<DtorCounter> buffer(1000);

        for (unsigned int i = 0; i < count_constructed; ++i)
            buffer.emplace_front();
    }

    ASSERT_EQ(count_constructed, count_destroyed) << "count constructions "
                                                     "!= count destroyed";
}
