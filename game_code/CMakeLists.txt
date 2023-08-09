target_sources(mySrcFiles
    PRIVATE
        game_components.hpp
        game_systems.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})