target_sources(mySrcFiles
    PRIVATE
        ae_ecs.hpp
        ae_ecs_constants.hpp
        ae_component_manager.cpp
        ae_component_manager.hpp
        ae_entity_manager.cpp
        ae_entity_manager.hpp
        ae_system_manager.cpp
        ae_system_manager.hpp
        ae_entity.hpp
        ae_component.hpp
        ae_system_base.cpp
        ae_system_base.hpp
        ae_system.hpp
        ae_ecs_include.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})