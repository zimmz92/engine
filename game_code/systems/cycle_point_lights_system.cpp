/// \file player_input_system.cpp
/// \brief The script implementing the player input system class.
/// The player input system class is implemented.

#include "cycle_point_lights_system.hpp"

namespace ae {
    CyclePointLightsSystem::CyclePointLightsSystem(GameComponents* t_game_components, TimingSystem* t_timingSystem)
                                     : ae_ecs::AeSystem<CyclePointLightsSystem>() {

        m_game_components = t_game_components;
        m_timingSystem = t_timingSystem;

        // Register component dependencies
        m_game_components->worldPositionComponent.requiredBySystem(this->getSystemId());
        m_game_components->pointLightComponent.requiredBySystem(this->getSystemId());

        // Register system dependencies
        this->dependsOnSystem(m_timingSystem->getSystemId());

        this->enableSystem();
    };

    CyclePointLightsSystem::~CyclePointLightsSystem(){};

    void CyclePointLightsSystem::setupSystem(){};

    void CyclePointLightsSystem::executeSystem(){
        std::vector<ecs_id> validEntityIds = m_systemManager.getValidEntities(this->getSystemId());

        auto rotateLight = glm::rotate(
                glm::mat4(1.0f),
                m_timingSystem->getDt(),
                { 0.0f, -1.0f, 0.0f });

        m_numPointLights = 0;
        for (ecs_id entityId : validEntityIds){
            // TODO: Fix this limit!!
            assert(m_numPointLights < 10 && "Number of point lights exceed MAX_LIGHTS=10!");

            // Calculate the new point light position
            glm::vec3 newPointLightPosition = glm::vec3(rotateLight * glm::vec4(m_game_components->worldPositionComponent.getWorldPositionVec3(entityId), 1.0f));

            // Set the new position of the point light
            m_game_components->worldPositionComponent.setWorldPositionVec3(entityId,newPointLightPosition);

            m_numPointLights += 1;
        };
    };

    void CyclePointLightsSystem::cleanupSystem(){};
}
