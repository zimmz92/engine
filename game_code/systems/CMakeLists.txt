target_sources(mySrcFiles
    PRIVATE
        simple_render_system.cpp
        simple_render_system.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})