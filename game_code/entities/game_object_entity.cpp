#include "game_object_entity.hpp"

namespace ae {

	GameObjectEntity::GameObjectEntity(GameComponents* t_gameComponents) : AeEntity() {
        m_worldPosition = t_gameComponents->worldPositionComponent.requiredByEntity(this->m_entityId);
        m_model = t_gameComponents->modelComponent.requiredByEntity(this->m_entityId);
	};

    GameObjectEntity::~GameObjectEntity() {

    };
}