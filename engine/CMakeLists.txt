target_sources(mySrcFiles
    PRIVATE
        ae_engine_constants.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})