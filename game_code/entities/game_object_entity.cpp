#pragma once

#include "ae_ecs.hpp"
#include "game_object_entity.hpp"
#include "position_component.hpp"

namespace ae {

	GameObjectEntity::GameObjectEntity(modelComponentStructure t_modelComponentDefinition, glm::vec3 t_worldPosition) : AeEntity(ae_ecs::ecsEntityManager) {
        worldPositionComponent.requiredByEntity(this->m_entityId, t_worldPosition);
        modelComponent.requiredByEntity(this->m_entityId, t_modelComponentDefinition);

		ae_ecs::ecsComponentManager.enableEntity(this->m_entityId);
	};

    GameObjectEntity::~GameObjectEntity() = default;
}