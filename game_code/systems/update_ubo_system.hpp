/*! \file player_input_system.hpp
    \brief The script defining the player input system.

    The player input system is defined and the instance for the game is declared.

*/
#pragma once

#include "ae_ecs.hpp"

#include "game_components.hpp"

#include "camera_update_system.hpp"
#include "cycle_point_lights_system.hpp"

#include "ae_frame_info.hpp"


namespace ae {

    /// Updates the global universal buffer object (UBO) with relevant entity information.
    class UpdateUboSystem : public ae_ecs::AeSystem<UpdateUboSystem> {
    public:
        /// Constructor of the UpdateUboSystem
        /// \param t_game_components The game components available that this system may require.
        /// \param t_cameraUpdateSystem The CameraUpdateSystem the UpdateUboSystem will depend on executing first
        /// and require information from.
        /// \param t_cyclePointLightsSystem The CyclePointLightsSystem the UpdateUboSystem will depend on executing first
        /// and require information from.
        UpdateUboSystem(GameComponentsStruct& t_game_components,
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


