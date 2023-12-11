target_sources(mySrcFiles
    PRIVATE
        ae_allocator_base.hpp
        ae_memory_manager.hpp
        ae_memory_manager.cpp
        ae_allocator.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})