target_sources(mySrcFiles
    PRIVATE
        Test_entity-create-destroy_system.hpp
        Test_entity-create-destroy_system.cpp
        test_rotate_object_system.hpp
        test_rotate_object_system.cpp
        test_memory_allocators.hpp
        test_rotate_object_component.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})