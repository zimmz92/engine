#pragma once

#include "ae_ecs.hpp"
#include "objectEntity.hpp"
#include "position_component.hpp"

namespace ae {

	ObjectEntity::ObjectEntity(modelComponentStructure t_modelComponentDefinition, glm::vec3 t_worldPosition) : AeEntity(ae_ecs::ecsComponentManager, ae_ecs::ecsEntityManager) {
		worldPositionComponent.useComponnt(this->m_entityId, t_worldPosition);
		modelComponent.useComponnt(this->m_entityId, t_modelComponentDefinition);
	};

}