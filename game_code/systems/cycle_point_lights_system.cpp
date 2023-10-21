/// \file cycle_point_lights_system.cpp
/// \brief The script implementing the system that cycles the point lights.
/// The cycle point light system is implemented.

#include "cycle_point_lights_system.hpp"

namespace ae {

    // Constructor implementation
    CyclePointLightsSystem::CyclePointLightsSystem(ae_ecs::AeECS& t_ecs, GameComponents& t_game_components, TimingSystem& t_timingSystem)
    : m_worldPositionComponent{t_game_components.worldPositionComponent},
    m_PointLightComponent{t_game_components.pointLightComponent},
    m_timingSystem{t_timingSystem},
    ae_ecs::AeSystem<CyclePointLightsSystem>(t_ecs) {

        // Register component dependencies
        m_worldPositionComponent.requiredBySystem(this->getSystemId());
        m_PointLightComponent.requiredBySystem(this->getSystemId());

        // Register system dependencies
        this->dependsOnSystem(m_timingSystem.getSystemId());

        // Enable the system so it will run.
        this->enableSystem();
    };



    // Destructor implementation
    CyclePointLightsSystem::~CyclePointLightsSystem(){};



    // Set up the system prior to execution. Currently not used.
    void CyclePointLightsSystem::setupSystem(){};



    // Update the positions of the point lights to make them move in a circle.
    void CyclePointLightsSystem::executeSystem(){

        // Get the entities that use the components this system depends on. Get enabled entities since this will be a
        // system that will conditionally update their component data no matter if previous systems have acted upon
        // them.
        std::vector<ecs_id> validEntityIds = m_systemManager.getEnabledSystemsEntities(this->getSystemId());

        // Calculate the transform matrix to update the point light position to make them move in a circle around a
        // fixed normalized axis in space.
        auto rotateLight = glm::rotate(
                glm::mat4(1.0f),
                m_timingSystem.getDt(),
                { 0.0f, -1.0f, 0.0f });

        // Reset the number of point lights counter in case additional compatible point lights were added.
        m_numPointLights = 0;

        // Loop through the valid entities and update their world position to make them rotate.
        for (ecs_id entityId : validEntityIds){
            // TODO: Fix this limit!!
            assert(m_numPointLights < 10 && "Number of point lights exceed MAX_LIGHTS=10!");

            // Calculate the new point light position
            glm::vec3 newPointLightPosition = glm::vec3(rotateLight * glm::vec4(m_worldPositionComponent.getWorldPositionVec3(entityId), 1.0f));

            // Set the new position of the point light
            m_worldPositionComponent.setWorldPositionVec3(entityId,newPointLightPosition);

            // Increment the number of point lights to keep track of how many lights are being rotated and will need to
            // be rendered.
            m_numPointLights += 1;
        };
    };



    // Clean up the system after execution. Currently not used.
    void CyclePointLightsSystem::cleanupSystem(){
        m_systemManager.clearSystemEntityUpdateSignatures(m_systemId);
    };
}
