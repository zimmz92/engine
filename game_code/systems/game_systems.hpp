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
    struct GameSystems{

        GameSystems(GameComponents* t_game_components, GLFWwindow* t_window, AeRenderer* t_renderer){
            playerInputSystem = new PlayerInputSystem(t_game_components,&timingSystem,t_window);
            cameraUpdateSystem = new CameraUpdateSystemClass(t_game_components,playerInputSystem,t_renderer);
            cyclePointLightsSystem = new CyclePointLightsSystem(t_game_components,&timingSystem);
            updateUboSystem = new UpdateUboSystem(t_game_components, cameraUpdateSystem, cyclePointLightsSystem);
        }

        ~GameSystems(){
            // Delete systems in the reverse order of how they were declared.
            delete cameraUpdateSystem;
            cameraUpdateSystem = nullptr;

            delete playerInputSystem;
            playerInputSystem = nullptr;
        }

        TimingSystem timingSystem;
        PlayerInputSystem* playerInputSystem;
        CameraUpdateSystemClass* cameraUpdateSystem;
        UpdateUboSystem* updateUboSystem;

        // Temporary Systems
        CyclePointLightsSystem* cyclePointLightsSystem;
    };
}