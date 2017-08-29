#include <thread>        // std::thread
#include <atomic>        // std::atomic
#include "gtest/gtest.h" // TEST, ASSERT_*

// SingleWriterRingBuffer
#include "single_writer_ring_buffer/single_writer_ring_buffer.hpp"


template<typename T, std::size_t N>
class MailBox
{
public:
    MailBox()
        : buffer(),
          cursor(&buffer[0])
    {}

private:
    T buffer[N];
    T *cursor;
}; // class MailBox


TEST(insertion, single_thread)
{
}
