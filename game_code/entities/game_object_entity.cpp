/// \file game_object_entity.cpp
/// \brief Implements the game object entity.
/// The game object entity is implemented.
#include "game_object_entity.hpp"

namespace ae {

    // Constructor implementation of the GameObjectEntity
	GameObjectEntity::GameObjectEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents) :
            m_worldPosition{t_gameComponents.worldPositionComponent.requiredByEntityReference(this->m_entityId)},
            m_model{t_gameComponents.modelComponent.requiredByEntityReference(this->m_entityId)},
            AeEntity(t_ecs) {};

    // Destructor implementation of the GameObjectEntity
    GameObjectEntity::~GameObjectEntity() = default;
}