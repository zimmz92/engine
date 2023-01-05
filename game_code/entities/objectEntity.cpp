
#include "ae_ecs.hpp"
#include "objectEntity.hpp"
#include "testComponent.hpp"

namespace ae {

	ObjectEntity::ObjectEntity(AeComponentManager& t_componentManager, AeEntityManager& t_entityManager, testComponentStructure t_testComponentDefinition) : AeEntity(t_componentManager, t_entityManager) {

		useComponent(testComponent.getComponentId());
		testComponent.updateData(this->getEntityId(), t_testComponentDefinition);


	};

}