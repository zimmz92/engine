/*! \file update_ubo_system.hpp
    \brief The script defining the update ubo system.
    Updates the ubo information that would be required to render each frame.
*/
#pragma once

#include "ae_ecs_include.hpp"

#include "game_components.hpp"

#include "camera_update_system.hpp"
#include "cycle_point_lights_system.hpp"

namespace ae {

    #define MAX_LIGHTS 10

    /// Structure to hold the point light ubo/push constant data.
    struct PointLight {
        glm::vec4 position{}; // ignore w
        glm::vec4 color{}; // w is intensity
    };

    /// Structure to hold the Ubo data.
    struct GlobalUbo {
        // Remember alignment! Always ensure to align data properly or use only 4d!!!

        // Main Camera information
        glm::mat4 projection{ 1.0f };
        glm::mat4 view{ 1.0f };
        glm::mat4 inverseView{ 1.0f };

        // Ambient light, last field, w,  is the light intensity.
        glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f };

        // Point light data.
        PointLight pointLights[MAX_LIGHTS];
        int numLights = 0;
    };


    /// Updates the global universal buffer object (UBO) with relevant entity information.
    class UpdateUboSystem : public ae_ecs::AeSystem<UpdateUboSystem> {
    public:
        /// Constructor of the UpdateUboSystem
        /// \param t_game_components The game components available that this system may require.
        /// \param t_cameraUpdateSystem The CameraUpdateSystem the UpdateUboSystem will depend on executing first
        /// and require information from.
        /// \param t_cyclePointLightsSystem The CyclePointLightsSystem the UpdateUboSystem will depend on executing first
        /// and require information from.
        UpdateUboSystem(ae_ecs::AeECS& t_ecs,
                        GameComponents& t_game_components,
                        CameraUpdateSystem& t_cameraUpdateSystem,
                        CyclePointLightsSystem& t_cyclePointLightsSystem);

        /// Destructor of the UpdateUboSystem
        ~UpdateUboSystem();

        /// Setup the UpdateUboSystem, this is handled by the ECS.
        void setupSystem() override;

        /// Execute the UpdateUboSystem, this is handled by the ECS.
        void executeSystem() override;

        /// Clean up the UpdateUboSystem, this is handled by the ECS.
        void cleanupSystem() override;

        /// Get the ubo managed by this system
        /// \return A pointer to the GlobalUbo managed by this system.
        GlobalUbo* getUbo(){return & m_ubo;};

    private:

        // Components this system utilizes.
        /// The WorldPositionComponent this systems accesses to update the ubo with the position of applicable point
        /// light entities.
        WorldPositionComponent& m_worldPositionComponent;
        /// The UboDataFlagsComponent this system accesses to determine what type of ubo data an entity has.
        UboDataFlagsComponent& m_uboDataFlagsComponent;
        /// The PointLightComponentOld this system accesses to update the ubo with the point light characteristics.
        PointLightComponent& m_pointLightComponent;
        /// The CameraComponent this system access to update the ubo with camera perspective and view data.
        CameraComponent& m_cameraComponent;


        // Prerequisite systems for the PlayerInputSystem.
        /// The CameraUpdateSystem this system requires to operate prior to it's own operation.
        CameraUpdateSystem& m_cameraUpdateSystem;
        /// The CyclePointLightSystem this system requires to operate prior to it's own operation.
        CyclePointLightsSystem& m_cyclePointLightsSystem;

        /// Stores the ubo information
        GlobalUbo m_ubo{};

        /// Keeps track of the number of point lights in the ubo
        int m_numPointLights = 0;

    };
}


