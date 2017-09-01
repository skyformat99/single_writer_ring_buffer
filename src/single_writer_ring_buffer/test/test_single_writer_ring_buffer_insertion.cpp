#include <utililty>             // std::move
#include <thread>               // std::thread
#include "gtest/gtest.h"        // TEST, ASSERT_*
#include "mail_box/mail_box.hpp" // MailBox

// SingleWriterRingBuffer
#include "single_writer_ring_buffer/single_writer_ring_buffer.hpp"




TEST(insertion, single_thread_under_fill)
{
    MailBox<int, 10> mail_box;

    SingleWriterRingBuffer buffer(100);

    for (int i = 0; i < 10; ++i)
        buffer.emplace_front(i);


    for (int i = 0; i < 10; ++i) {
        ASSERT_TRUE(try_pop_back(next)) << "buffer under-filled";

        mail_box.push_back(next);
    }

    ASSERT_FALSE(try_pop_back(next)) << "buffer over-filled";

    MailBox<int, 10>::const_iterator mail_box_iter = mail_box.cbegin();

    for (int i = 0; i < 10; ++mail_box_iter, ++i)
        ASSERT_EQ(i, *mail_box_iter) << "insertions out of order";

    ASSERT_EQ(mail_box.cend(), mail_box_iter) << "mail_box over-filled";
}
