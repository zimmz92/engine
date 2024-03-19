target_sources(mySrcFiles
    PRIVATE
        pre_allocated_stack.hpp
        stl_wrappers.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})