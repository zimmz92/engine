target_sources(mySrcFiles
    PRIVATE
        ecs_test.hpp
        ecs_test.cpp
        test_systemA.hpp
        test_systemB.hpp
        test_systemC.hpp
        test_systemD.hpp
        test_systemE.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})