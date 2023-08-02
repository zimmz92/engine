/*! \file GameComponents.hpp
    \brief The script that declares the components of the game.

    The game components are declared.

*/
#pragma once

#include "camera_component.hpp"
#include "model_component.hpp"
#include "player_controlled_component.hpp"
#include "world_position_component.hpp"

namespace ae {
    struct GameComponents{
        cameraComponentClass cameraComponent{};
        modelComponentClass modelComponent{};
        playerControlledComponentClass playerControlledComponent{};
        worldPositionComponentClass worldPositionComponent{};
    };
}