target_sources(mySrcFiles
    PRIVATE
        ecs_test.hpp
        ecs_test.cpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})