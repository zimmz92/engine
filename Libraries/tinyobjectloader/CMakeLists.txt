target_sources(mySrcFiles
    PRIVATE
        tiny_obj_loader.h
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})