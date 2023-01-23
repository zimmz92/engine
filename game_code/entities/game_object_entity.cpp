#pragma once

#include "ae_ecs.hpp"
#include "game_object_entity.hpp"
#include "position_component.hpp"

namespace ae {

	GameObjectEntity::GameObjectEntity(modelComponentStructure t_modelComponentDefinition, glm::vec3 t_worldPosition) : AeEntity(ae_ecs::ecsComponentManager, ae_ecs::ecsEntityManager) {
		worldPositionComponent.useComponent(this->m_entityId, t_worldPosition);
		modelComponent.useComponent(this->m_entityId, t_modelComponentDefinition);

		ae_ecs::ecsComponentManager.enableEntity(this->m_entityId);
	};

}