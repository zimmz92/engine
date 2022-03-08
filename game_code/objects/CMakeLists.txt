target_sources(mySrcFiles
    PRIVATE
        ae_game_object.cpp
        ae_game_object.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})