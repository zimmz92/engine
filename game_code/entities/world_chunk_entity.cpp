/// \file world_chunk_entity.cpp
/// \brief Implements world chunk entities.
/// World Chunk Entities are implemented.
#include "world_chunk_entity.hpp"

namespace ae {

    // Constructor implementation of the WorldChunkEntity
    WorldChunkEntity::WorldChunkEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents) :
            m_worldPosition{t_gameComponents.worldPositionComponent.requiredByEntityReference(this->m_entityId)},
            m_worldChunkProperties{t_gameComponents.worldChunkComponent.requiredByEntityReference(this->m_entityId)},
            AeEntity(t_ecs) {};

    // Destructor implementation of the WorldChunkEntity
    WorldChunkEntity::~WorldChunkEntity() = default;
}