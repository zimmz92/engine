/// \file world_voxel_entity.cpp
/// \brief Implements world voxel entities.
/// World Voxel Entities are implemented.
#include "world_voxel_entity.hpp"

namespace ae {

    // Constructor implementation of the WorldVoxelEntity
	WorldVoxelEntity::WorldVoxelEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents) :
            m_worldPosition{t_gameComponents.worldPositionComponent.requiredByEntityReference(this->m_entityId)},
            m_worldVoxelProperties{t_gameComponents.worldVoxelComponent.requiredByEntityReference(this->m_entityId)},
            AeEntity(t_ecs) {};

    // Destructor implementation of the WorldVoxelEntity
    WorldVoxelEntity::~WorldVoxelEntity() = default;
}