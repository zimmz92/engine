/*! \file GameSystems.hpp
    \brief The script that declares the systems of the game.
    The game systems are declared.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "game_components.hpp"

#include "camera_update_system.hpp"
#include "player_input_system.hpp"
#include "timing_system.hpp"
#include "cycle_point_lights_system.hpp"
#include "update_ubo_system.hpp"
#include "renderer_system.hpp"

namespace ae {

    /// A structure to declare the game systems of this game.
    struct GameSystems{

        /// Constructor for this struct.
        GameSystems(ae_ecs::AeECS& t_ecs, GameComponents& t_game_components, GLFWwindow* t_window, AeDevice& t_device, AeRenderer& t_renderer) {
            timingSystem = new TimingSystem(t_ecs);
            playerInputSystem = new PlayerInputSystem(t_ecs, t_game_components, *timingSystem, t_window);
            cameraUpdateSystem = new CameraUpdateSystem(t_ecs, t_game_components, *playerInputSystem, t_renderer);
            cyclePointLightsSystem = new CyclePointLightsSystem(t_ecs, t_game_components, *timingSystem);
            updateUboSystem = new UpdateUboSystem(t_ecs, t_game_components, *cameraUpdateSystem, *cyclePointLightsSystem);
            rendererSystem = new RendererStartPassSystem(t_ecs, t_game_components, *updateUboSystem, *timingSystem, t_renderer, t_device);
        };

        /// Destructor for this struct.
        ~GameSystems(){
            // Delete systems in the reverse order of how they were declared.
            delete rendererSystem;
            rendererSystem = nullptr;

            delete updateUboSystem;
            updateUboSystem = nullptr;

            delete cyclePointLightsSystem;
            cyclePointLightsSystem = nullptr;

            delete cameraUpdateSystem;
            cameraUpdateSystem = nullptr;

            delete playerInputSystem;
            playerInputSystem = nullptr;
        };

        /// The TimingSystem instance for the game.
        TimingSystem* timingSystem;

        /// The PlayerInputSystem instance for the game.
        PlayerInputSystem* playerInputSystem;

        /// The CameraUpdateSystem instance for the game.
        CameraUpdateSystem* cameraUpdateSystem;

        /// The UpdateUboSystem instance for the game.
        UpdateUboSystem* updateUboSystem;

        /// The RendererStartPassSystem instance for the game.
        RendererStartPassSystem* rendererSystem;


        // Temporary Systems
        /// The CyclePointLightsSystem instance for the game.
        CyclePointLightsSystem* cyclePointLightsSystem;
    };
}