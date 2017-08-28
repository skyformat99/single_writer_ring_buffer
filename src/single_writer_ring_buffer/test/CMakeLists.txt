foreach(component constructor destructor)
    add_google_test(
        NAME    test_single_writer_ring_buffer_${component}
        SOURCES test_single_writer_ring_buffer_${component}.cpp
                ${SINGLE_WRITER_RING_BUFFER_HEADERS}
    )
endforeach()
