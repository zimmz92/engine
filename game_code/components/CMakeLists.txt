target_sources(mySrcFiles
    PRIVATE
        model_component.hpp
        world_position_component.hpp
        player_controlled_component.hpp
        camera_component.hpp
        ubo_data_flags_component.hpp
        point_light_component.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})