target_sources(mySrcFiles
    PRIVATE
        stb_image.h
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})