target_sources(mySrcFiles
    PRIVATE
        ae_allocator.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})