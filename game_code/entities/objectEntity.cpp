#pragma once

#include "ae_ecs.hpp"
#include "objectEntity.hpp"
#include "testComponent.hpp"

namespace ae {

	ObjectEntity::ObjectEntity(ae_ecs::AeComponentManager& t_componentManager, ae_ecs::AeEntityManager& t_entityManager,
		testComponentStructure t_testComponentDefinition) : AeEntity(t_componentManager, t_entityManager) {

		testComponent.useComponnt(this->m_entityId, t_testComponentDefinition);

	};

}