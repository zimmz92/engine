target_sources(mySrcFiles
    PRIVATE
        ae_renderer_system.hpp
        ae_renderer_system.cpp
        ae_particle_system.hpp
        ae_particle_system.cpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})