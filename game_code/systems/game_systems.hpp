/*! \file GameSystems.hpp
    \brief The script that declares the systems of the game.
    The game systems are declared.
*/
#pragma once

#include "game_components.hpp"

#include "camera_update_system.hpp"
#include "player_input_system.hpp"
#include "timing_system.hpp"
#include "cycle_point_lights_system.hpp"
#include "update_ubo_system.hpp"

namespace ae {

    /// A structure to declare the game systems of this game.
    struct GameSystems{

        /// Constructor for this struct.
        GameSystems(GameComponentsStruct& t_game_components, GLFWwindow* t_window, AeRenderer& t_renderer){
            playerInputSystem = new PlayerInputSystemClass(t_game_components, timingSystem, t_window);
            cameraUpdateSystem = new CameraUpdateSystemClass(t_game_components,*playerInputSystem,t_renderer);
            cyclePointLightsSystem = new CyclePointLightsSystemClass(t_game_components, timingSystem);
            updateUboSystem = new UpdateUboSystemClass(t_game_components, *cameraUpdateSystem, *cyclePointLightsSystem);
        }

        /// Destructor for this struct.
        ~GameSystems(){
            // Delete systems in the reverse order of how they were declared.
            delete updateUboSystem;
            updateUboSystem = nullptr;

            delete cyclePointLightsSystem;
            cyclePointLightsSystem = nullptr;

            delete cameraUpdateSystem;
            cameraUpdateSystem = nullptr;

            delete playerInputSystem;
            playerInputSystem = nullptr;
        }

        /// The TimingSystemClass instance for the game.
        TimingSystemClass timingSystem;

        /// The PlayerInputSystemClass instance for the game.
        PlayerInputSystemClass* playerInputSystem;

        /// The CameraUpdateSystemClass instance for the game.
        CameraUpdateSystemClass* cameraUpdateSystem;

        /// The UpdateUboSystemClass instance for the game.
        UpdateUboSystemClass* updateUboSystem;

        // Temporary Systems
        /// The CyclePointLightsSystemClass instance for the game.
        CyclePointLightsSystemClass* cyclePointLightsSystem;
    };
}