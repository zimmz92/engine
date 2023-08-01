#include "game_object_entity.hpp"

namespace ae {

	GameObjectEntity::GameObjectEntity() : AeEntity() {
        m_worldPosition = worldPositionComponent.requiredByEntity(this->m_entityId);
        m_model = modelComponent.requiredByEntity(this->m_entityId);
	};

    GameObjectEntity::~GameObjectEntity() {

    };
}