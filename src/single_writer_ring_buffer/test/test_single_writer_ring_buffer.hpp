#include <utility>                           // std::move
#include <string>                            // std::string
#include "gtest/gtest.h"                     // TEST, ASSERT_*
#include "blocking_queue/blocking_queue.hpp" // BlockingQueue



TEST(enqueue, const_ref)
{
    BlockingQueue<std::string> queue;

    ASSERT_TRUE(queue.empty());

    std::string element("element");

    queue.enqueue(element);

    ASSERT_EQ(1, queue.size());

    ASSERT_EQ("element", element);
}


TEST(enqueue, rvalue_ref)
{
    BlockingQueue<std::string> queue;

    ASSERT_TRUE(queue.empty());

    std::string element("element");

    queue.enqueue(std::move(element));

    ASSERT_EQ(1, queue.size());

    ASSERT_TRUE(element.empty());
}
