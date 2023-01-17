target_sources(mySrcFiles
    PRIVATE
        modelComponent.hpp
        positionComponent.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})