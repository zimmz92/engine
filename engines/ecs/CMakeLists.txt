target_sources(mySrcFiles
    PRIVATE
        ae_entity.cpp
        ae_entity.hpp
        ae_component_manager.cpp
        ae_component_manager.hpp
        ae_entity_manager.cpp
        ae_entity_manager.hpp
        ae_component.cpp
        ae_component.hpp
        ae_system.cpp
        ae_system.hpp
        ae_id_counters.hpp
    PUBLIC
)

target_include_directories(mySrcFiles PUBLIC ${CMAKE_CURRENT_LIST_DIR})