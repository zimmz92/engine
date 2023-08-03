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
    class UpdateUboSystem : public ae_ecs::AeSystem<UpdateUboSystem> {
    public:
        UpdateUboSystem(GameComponents* t_game_components,
                        CameraUpdateSystemClass* t_cameraUpdateSystem,
                        CyclePointLightsSystem* t_cyclePointLightsSystem);
        ~UpdateUboSystem();

        void setupSystem() override;
        void executeSystem() override;
        void cleanupSystem() override;

        /// Get the ubo managed by this system
        /// \return A pointer to the GlobalUbo managed by this system.
        GlobalUbo* getUbo(){return & m_ubo;};

    private:

        /// The game components this system interacts with
        GameComponents* m_game_components;

        /// Pointer to the cycle point light system
        CyclePointLightsSystem* m_cyclePointLightsSystem;

        /// Stores the ubo information
        GlobalUbo m_ubo;

        /// Keeps track of the number of point lights in the ubo
        int m_numPointLights;

    };
}


