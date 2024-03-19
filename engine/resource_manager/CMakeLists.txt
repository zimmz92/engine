target_sources(mySrcFiles
    PRIVATE
        ae_resource_manager.hpp
        ae_resource_manager.cpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})