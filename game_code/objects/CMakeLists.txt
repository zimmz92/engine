target_sources(mySrcFiles
    PRIVATE
        ae_game_object.hpp
        ae_game_object.cpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})