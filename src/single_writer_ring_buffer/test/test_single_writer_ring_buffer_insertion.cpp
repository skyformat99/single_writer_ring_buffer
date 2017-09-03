#include <utility>               // std::move
#include <thread>                // std::thread
#include <algorithm>             // std::min
#include <iterator>              // std::distance
#include "gtest/gtest.h"         // TEST, ASSERT_*
#include "mail_box/mail_box.hpp" // MailBox

// SingleWriterRingBuffer
#include "single_writer_ring_buffer/single_writer_ring_buffer.hpp"



TEST(insertion, single_thread)
{
    static const std::size_t mail_box_size = 100;

    for (std::size_t buffer_size = 1000; buffer_size >= 10; buffer_size /= 10) {
        MailBox<int, mail_box_size> mail_box;
        int prev;
        int next;

        SingleWriterRingBuffer<int> buffer(buffer_size);

        for (int i = 0; i < mail_box_size; ++i)
            buffer.emplace_front(i);

        const std::size_t buffer_count = std::min(mail_box_size, buffer_size);

        for (int i = 0; i < buffer_count; ++i) {
            ASSERT_TRUE(buffer.try_pop_back(next)) << "buffer under-filled";

            mail_box.push_back(next);
        }

        ASSERT_FALSE(buffer.try_pop_back(next)) << "buffer over-filled";

        auto       mail_box_iter = mail_box.cbegin();
        const auto mail_box_end  = mail_box.cend();

        ASSERT_EQ(buffer_count,
                  std::distance(mail_box_iter,
                                mail_box_end)) << "invalid mail_box";

        prev = *mail_box_iter;

        while (++mail_box_iter != mail_box_end) {
            next = *mail_box_iter;

            ASSERT_LT(prev, next) << "insertions out of order";

            prev = next;
        }
    }
}
