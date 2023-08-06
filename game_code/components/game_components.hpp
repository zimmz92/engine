/*! \file GameComponentsStruct.hpp
    \brief The script that declares the components of the game.
    The game components are declared.
*/
#pragma once

#include "camera_component.hpp"
#include "model_component.hpp"
#include "player_controlled_component.hpp"
#include "world_position_component.hpp"
#include "ubo_data_flags_component.hpp"
#include "point_light_component.hpp"

namespace ae {

    /// A structure to declare the game components of this game.
    struct GameComponentsStruct{
        CameraComponentClass cameraComponent{};
        ModelComponentClass modelComponent{};
        PlayerControlledComponentClass playerControlledComponent{};
        WorldPositionComponentClass worldPositionComponent{};
        UboDataFlagsComponentClass uboDataFlagsComponent{};
        PointLightComponentClass pointLightComponent{};
    };
}