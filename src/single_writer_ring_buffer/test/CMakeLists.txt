add_google_test(
    NAME    test_single_writer_ring_buffer
    SOURCES test_single_writer_ring_buffer.cpp
            ${SINGLE_WRITER_RING_BUFFER_HEADERS}
)
