target_sources(mySrcFiles
    PRIVATE
        keyboard_movement_controller.hpp
        keyboard_movement_controller.cpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})