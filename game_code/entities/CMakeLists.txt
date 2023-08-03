target_sources(mySrcFiles
    PRIVATE
        game_object_entity.hpp
        game_object_entity.cpp
        camera_entity.hpp
        camera_entity.cpp
        point_light_entity.cpp
        point_light_entity.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})