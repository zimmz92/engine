/*! \file world_chunk_entity.hpp
    \brief The script defining world chunk entities.
    World chunk entities are defined.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "game_components.hpp"

namespace ae {

    /// Defines entities that are world chunks that comprise the game world.
    class WorldChunkEntity : public ae_ecs::AeEntity<WorldChunkEntity> {

    public:
        /// Creates the WorldChunkEntity using the specified game components.
        /// \param t_ecs The Entity-Component-System that will be handling this entity.
        /// \param t_gameComponents The game components this entity has available to utilize.
        explicit WorldChunkEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents);

        /// The destructor of the WorldVoxelEntity.
        ~WorldChunkEntity();

        /// A reference to the world position of the voxel.
        WorldPositionComponentStruct& m_worldPosition;

        /// A reference to the properties of the voxel.
        WorldChunkComponentStruct& m_worldChunkProperties;

    private:

    protected:
    };

};