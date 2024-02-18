target_sources(mySrcFiles
    PRIVATE
        easylogging++.h
        easylogging++.cc
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})