target_sources(mySrcFiles
    PRIVATE
        ae_entity.cpp
        ae_entity.hpp
        ae_componentManager.cpp
        ae_componentManager.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})