/// \file player_input_system.cpp
/// \brief The script implementing the player input system class.
/// The player input system class is implemented.

#include "update_ubo_system.hpp"

#include <string>

namespace ae {
    UpdateUboSystem::UpdateUboSystem(GameComponents* t_game_components,
                                     CameraUpdateSystemClass* t_cameraUpdateSystem,
                                     CyclePointLightsSystem* t_cyclePointLightsSystem)
                                     : ae_ecs::AeSystem<UpdateUboSystem>() {

        m_game_components = t_game_components;
        m_cyclePointLightsSystem = t_cyclePointLightsSystem;

        // Register component dependencies
        m_game_components->worldPositionComponent.requiredBySystem(this->getSystemId());
        m_game_components->uboDataFlagsComponent.requiredBySystem(this->getSystemId());

        // Register system dependencies
        this->dependsOnSystem(t_cameraUpdateSystem->getSystemId());
        this->dependsOnSystem(t_cyclePointLightsSystem->getSystemId());
        this->enableSystem();
    };

    UpdateUboSystem::~UpdateUboSystem(){};

    void UpdateUboSystem::setupSystem(){};

    void UpdateUboSystem::executeSystem(){
        std::vector<ecs_id> validEntityIds = m_systemManager.getValidEntities(this->getSystemId());

        // Clear the point light counter, need this to check to insert point lights into the ubo.
        // Also, currently required to verify that the number of point lights going into the ubo matches the expected
        // number of point lights handled by other systems.
        m_numPointLights = 0;

        // Loop through all the valid entities with the required components with the UpdateUboSystem
        for (ecs_id entityId : validEntityIds){


            // Check if this entity has camera data for the ubo
            if(m_game_components->uboDataFlagsComponent.getDataPointer(entityId)->hasUboCameraData){
                // Check that the entity actually uses the cameraComponent. If not we have some sort of error going on
                // so throw one!
                if(m_game_components->cameraComponent.doesEntityUseThis(entityId)) {
                    // Get a pointer to entity's camera data
                    auto entityCameraData = m_game_components->cameraComponent.getDataPointer(entityId);

                    // Only the main camera should be fed to the ubo since all buffers will require that data
                    if (entityCameraData->isMainCamera) {
                        // Give the ubo the camera's perspective and view data
                        m_ubo.projection = entityCameraData->m_projectionMatrix;
                        m_ubo.view = entityCameraData->m_viewMatrix;
                        m_ubo.inverseView = entityCameraData->m_inverseViewMatrix;
                    };
                } else{
                    // Error if the entity does not contain the camera data it claims to have in the uboDataFlagsComponent.
                    std::string errorMessage = std::string("The \"") +
                                               std::string(typeid(entityId).name()) +
                                               std::string("\" entity with ID, ") +
                                               std::to_string(entityId) +
                                               std::string(", is using the UpdateUboSystem claims to have camera "
                                                           "data for the ubo, however, does not use the "
                                                           "cameraComponent!");
                    throw std::runtime_error(errorMessage);
                };
            };


            // Check if the entity has point light data for the ubo
            if(m_game_components->uboDataFlagsComponent.getDataPointer(entityId)->hasUboPointLightData){
                // Check that the entity actually uses the pointLightComponent. If not we have some sort of error going on
                // so throw one!
                if(m_game_components->pointLightComponent.doesEntityUseThis(entityId)) {
                    // Ensure we are not exceeding our point light limit for some reason.
                    assert(m_numPointLights < MAX_LIGHTS && "Number of point lights exceed MAX_LIGHTS!");

                    // Get a pointer to entity's point light data
                    auto entityPointLightData = m_game_components->pointLightComponent.getDataPointer(entityId);

                    // Put the entity's point light data into the ubo.
                    m_ubo.pointLights[m_numPointLights].position = glm::vec4(m_game_components->worldPositionComponent.getWorldPositionVec3(entityId), 1.0f);
                    m_ubo.pointLights[m_numPointLights].color = glm::vec4(entityPointLightData->m_color, entityPointLightData->lightIntensity);

                    // Increment the number of point lights counter
                    m_numPointLights += 1;
                } else{
                    // Error if the entity does not contain the point light data it claims to have in the uboDataFlagsComponent.
                    std::string errorMessage = std::string("The \"") +
                                               std::string(typeid(entityId).name()) +
                                               std::string("\" entity with ID, ") +
                                               std::to_string(entityId) +
                                               std::string(", is using the UpdateUboSystem claims to have point"
                                                           " light ubo data, however, does not use the "
                                                           "pointLightComponent!");
                    throw std::runtime_error(errorMessage);
                };
            };
        };

        // Verify that the number of point lights handled by this system match the number of point lights handled by the
        // other point light related systems.
        if(m_numPointLights != m_cyclePointLightsSystem->getNumPointLights()){
            // Error if the entity does not contain the point light data it claims to have in the uboDataFlagsComponent.
            std::string errorMessage = std::string("The number of point lights handled by the UpdateUboSystem, ") +
                                       std::to_string(m_numPointLights) +
                                       std::string(", does not match the number of lights handled in the CyclePointLightsSystem, ") +
                                       std::to_string(m_cyclePointLightsSystem->getNumPointLights()) +
                                       std::string("!");
            throw std::runtime_error(errorMessage);
        }
    };

    void UpdateUboSystem::cleanupSystem(){};
}
