target_sources(mySrcFiles
    PRIVATE
        renderer_system.hpp
        renderer_system.cpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})