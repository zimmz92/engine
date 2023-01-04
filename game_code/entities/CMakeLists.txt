target_sources(mySrcFiles
    PRIVATE
        objectEntity.hpp
        objectEntity.cpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})