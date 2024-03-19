target_sources(mySrcFiles
    PRIVATE
        ae_allocator_base.hpp
        ae_allocator_base.cpp
        ae_memory_manager.hpp
        ae_memory_manager.cpp
        ae_stack_allocator.hpp
        ae_stack_allocator.cpp
        ae_de_stack_allocator.hpp
        ae_de_stack_allocator.cpp
        ae_pool_allocator.hpp
        ae_pool_allocator.cpp
        ae_free_linked_list_allocator.hpp
        ae_free_linked_list_allocator.cpp
        ae_allocator_stl_adapter.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})