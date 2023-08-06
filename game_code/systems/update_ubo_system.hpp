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
    class UpdateUboSystemClass : public ae_ecs::AeSystem<UpdateUboSystemClass> {
    public:
        /// Constructor of the CyclePointLightSystemClass
        /// \param t_game_components The game components available that this system may require.
        /// \param t_cameraUpdateSystem The CameraUpdateSystemClass the UpdateUboSystemClass will depend on executing first
        /// and require information from.
        /// \param t_cyclePointLightsSystem The CyclePointLightsSystemClass the UpdateUboSystemClass will depend on executing first
        /// and require information from.
        UpdateUboSystemClass(GameComponentsStruct& t_game_components,
                             CameraUpdateSystemClass& t_cameraUpdateSystem,
                             CyclePointLightsSystemClass& t_cyclePointLightsSystem);

        /// Destructor of the UpdateUboSystemClass
        ~UpdateUboSystemClass();

        /// Setup the CyclePointLightSystemClass, this is handled by the ECS.
        void setupSystem() override;

        /// Execute the CyclePointLightSystemClass, this is handled by the ECS.
        void executeSystem() override;

        /// Clean up the CyclePointLightSystemClass, this is handled by the ECS.
        void cleanupSystem() override;

        /// Get the ubo managed by this system
        /// \return A pointer to the GlobalUbo managed by this system.
        GlobalUbo* getUbo(){return & m_ubo;};

    private:

        // Components this system utilizes.
        /// The WorldPositionComponent this systems accesses to update the ubo with the position of applicable point
        /// light entities.
        WorldPositionComponentClass& m_worldPositionComponent;
        /// The UboDataFlagsComponent this system accesses to determine what type of ubo data an entity has.
        UboDataFlagsComponentClass& m_uboDataFlagsComponent;
        /// The PointLightComponent this system accesses to update the ubo with the point light characteristics.
        PointLightComponentClass& m_pointLightComponent;
        /// The CameraComponent this system access to update the ubo with camera perspective and view data.
        CameraComponentClass& m_cameraComponent;


        // Prerequisite systems for the PlayerInputSystemClass.
        /// The CameraUpdateSystem this system requires to operate prior to it's own operation.
        CameraUpdateSystemClass& m_cameraUpdateSystem;
        /// The CyclePointLightSystem this system requires to operate prior to it's own operation.
        CyclePointLightsSystemClass& m_cyclePointLightsSystem;

        /// Stores the ubo information
        GlobalUbo m_ubo{};

        /// Keeps track of the number of point lights in the ubo
        int m_numPointLights = 0;

    };
}


