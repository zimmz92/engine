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
            playerInputSystem = new PlayerInputSystem(t_game_components, timingSystem, t_window);
            cameraUpdateSystem = new CameraUpdateSystem(t_game_components, *playerInputSystem, t_renderer);
            cyclePointLightsSystem = new CyclePointLightsSystem(t_game_components, timingSystem);
            updateUboSystem = new UpdateUboSystem(t_game_components, *cameraUpdateSystem, *cyclePointLightsSystem);
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

        /// The TimingSystem instance for the game.
        TimingSystem timingSystem;

        /// The PlayerInputSystem instance for the game.
        PlayerInputSystem* playerInputSystem;

        /// The CameraUpdateSystem instance for the game.
        CameraUpdateSystem* cameraUpdateSystem;

        /// The UpdateUboSystem instance for the game.
        UpdateUboSystem* updateUboSystem;

        // Temporary Systems
        /// The CyclePointLightsSystem instance for the game.
        CyclePointLightsSystem* cyclePointLightsSystem;
    };
}