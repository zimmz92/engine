/*! \file world_voxel_entity.hpp
    \brief The script defining world voxel entities.
    World voxel entities are defined.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "game_components.hpp"

namespace ae {

    /// Defines entities that are world voxels that comprise the game world.
    class WorldVoxelEntity : public ae_ecs::AeEntity<WorldVoxelEntity> {

    public:
        /// Creates the WorldVoxelEntity using the specified game components.
        /// \param t_ecs The Entity-Component-System that will be handling this entity.
        /// \param t_gameComponents The game components this entity has available to utilize.
        explicit WorldVoxelEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents);

        /// The destructor of the WorldVoxelEntity.
        ~WorldVoxelEntity();

        /// A reference to the world position of the voxel.
        WorldPositionComponentStruct& m_worldPosition;

        /// A reference to the model representing the voxel.
        ModelComponentStruct& m_model;

        /// A reference to the properties of the voxel.
        WorldVoxelComponentStruct& m_worldVoxelProperties;

    private:

    protected:
    };

};