target_sources(mySrcFiles
    PRIVATE
        ae_rs_simple.cpp
        ae_rs_simple.hpp
        ae_frame_info.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})