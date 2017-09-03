#include <utility>               // std::move
#include <thread>                // std::thread
#include "gtest/gtest.h"         // TEST, ASSERT_*
#include "mail_box/mail_box.hpp" // MailBox

// SingleWriterRingBuffer
#include "single_writer_ring_buffer/single_writer_ring_buffer.hpp"



TEST(insertion, single_thread)
{
    for (std::size_t buffer_size = 1000; buffer_size >= 10; buffer_size /= 10) {
        MailBox<int, 100> mail_box;
        int next;

        SingleWriterRingBuffer<int> buffer(buffer_size);

        for (int i = 0; i < 100; ++i)
            buffer.emplace_front(i);

        for (int i = 0; i < 100; ++i) {
            ASSERT_TRUE(buffer.try_pop_back(next)) << "buffer under-filled";

            mail_box.push_back(next);
        }

        ASSERT_FALSE(buffer.try_pop_back(next)) << "buffer over-filled";

        MailBox<int, 100>::const_iterator mail_box_iter = mail_box.cbegin();

        for (int i = 0; i < 100; ++mail_box_iter, ++i)
            ASSERT_EQ(i, *mail_box_iter) << "insertions out of order";

        ASSERT_EQ(mail_box.cend(), mail_box_iter) << "mail_box over-filled";
    }
}
