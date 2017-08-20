set(
    SINGLE_WRTIER_RING_BUFFER_HEADERS
    ${PROJECT_INCLUDE_DIR}/single_writer_ring_buffer/single_writer_ring_buffer.hpp
)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
