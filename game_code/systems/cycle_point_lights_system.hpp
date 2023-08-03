/*! \file player_input_system.hpp
    \brief The script defining the player input system.

    The player input system is defined and the instance for the game is declared.

*/
#pragma once

#include "ae_ecs.hpp"

#include "game_components.hpp"

#include "timing_system.hpp"


namespace ae {
    class CyclePointLightsSystem : public ae_ecs::AeSystem<CyclePointLightsSystem> {
    public:
        CyclePointLightsSystem(GameComponents* t_game_components, TimingSystem* t_timingSystem);
        ~CyclePointLightsSystem();

        void setupSystem() override;
        void executeSystem() override;
        void cleanupSystem() override;

        /// Returns the number of point lights being handled by this system
        int getNumPointLights() const {return m_numPointLights; };

    private:

        /// The game components this system interacts with
        GameComponents* m_game_components;

        /// Points to the timing system this system requires to operate prior to it's own operation.
        TimingSystem* m_timingSystem;

        /// Number of point lights being updated
        int m_numPointLights;
    };
}


