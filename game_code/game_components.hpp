/*! \file GameComponents.hpp
    \brief The script that declares the components of the game.
    The game components are declared.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "camera_component.hpp"
#include "model_component.hpp"
#include "player_controlled_component.hpp"
#include "world_position_component.hpp"
#include "ubo_data_flags_component.hpp"
#include "point_light_component.hpp"
#include "model_2d_component.hpp"
#include "world_voxel_component.hpp"
#include "world_chunk_component.hpp"

namespace ae {

    /// A structure to declare the game components of this game.
    struct GameComponents{

        GameComponents(ae_ecs::AeECS& t_ecs) : ecs{t_ecs} {};

        ~GameComponents(){};

        ae_ecs::AeECS& ecs;
        CameraComponent cameraComponent{ecs};
        ModelComponent modelComponent{ecs};
        PlayerControlledComponent playerControlledComponent{ecs};
        WorldPositionComponent worldPositionComponent{ecs};
        UboDataFlagsComponent uboDataFlagsComponent{ecs};
        PointLightComponent pointLightComponent{ecs};
        Model2dComponent model2DComponent{ecs};
        WorldVoxelComponent worldVoxelComponent{ecs};
        WorldChunkComponent worldChunkComponent{ecs};
    };
}