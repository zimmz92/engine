target_sources(mySrcFiles
    PRIVATE
        Test_entity-create-destroy_system.hpp
        Test_entity-create-destroy_system.cpp
        test_memory_allocators.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})