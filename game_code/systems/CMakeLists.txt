target_sources(mySrcFiles
    PRIVATE
        timing_system.cpp
        timing_system.hpp
        player_input_system.cpp
        player_input_system.hpp
        camera_update_system.cpp
        camera_update_system.hpp
        game_systems.hpp
        update_ubo_system.cpp
        update_ubo_system.hpp
        cycle_point_lights_system.cpp
        cycle_point_lights_system.hpp
        renderer_system.hpp
        renderer_system.cpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})