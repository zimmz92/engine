/// \file game_object_entity.cpp
/// \brief Implements the game object entity.
/// The game object entity is implemented.
#include "game_object_entity.hpp"

namespace ae {

    // Constructor implementation of the GameObjectEntity
	GameObjectEntity::GameObjectEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents) :
            m_worldPosition{t_gameComponents.worldPositionComponent.getDataReference(this->m_entityId)},
            m_model{t_gameComponents.modelComponent.getDataReference(this->m_entityId)},
            AeEntity(t_ecs) {

        // Specify the components that define this entity and where this entity will store data.
        t_gameComponents.worldPositionComponent.requiredByEntity(this->m_entityId);
        t_gameComponents.modelComponent.requiredByEntity(this->m_entityId);

	};



    // Destructor implementation of the GameObjectEntity
    GameObjectEntity::~GameObjectEntity() {

    };
}