target_sources(mySrcFiles
    PRIVATE
        point_light_render_system.hpp
        point_light_render_system.cpp
        simple_render_system.hpp
        simple_render_system.cpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC .)