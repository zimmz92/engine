target_sources(mySrcFiles
    PRIVATE
        ae_point_light_render_system.hpp
        ae_point_light_render_system.cpp
        ae_simple_render_system.hpp
        ae_simple_render_system.cpp
        ae_ui_render_system.hpp
        ae_ui_render_system.cpp
        ae_model_3d_buffer_system.hpp
        ae_model_3d_buffer_system.cpp
        ae_aabb_render_system.hpp
        ae_aabb_render_system.cpp
        ae_obb_render_system.hpp
        ae_obb_render_system.cpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC .)