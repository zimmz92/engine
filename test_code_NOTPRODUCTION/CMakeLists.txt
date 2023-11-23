target_sources(mySrcFiles
    PRIVATE
        Test_entity-create-destroy_system.hpp
        Test_entity-create-destroy_system.cpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})