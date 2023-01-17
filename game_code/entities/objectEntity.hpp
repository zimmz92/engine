#pragma once

#include "ae_ecs.hpp"
#include "testComponent.hpp"

namespace ae {
	class ObjectEntity : public ae_ecs::AeEntity<ObjectEntity> {

	public:
		// Function to create an entity
		ObjectEntity(ae_ecs::AeComponentManager& t_componentManager, ae_ecs::AeEntityManager& t_entityManager, testComponentStructure t_testComponentDefinition);

		// Function to destroy an entity
		~ObjectEntity() {};


	private:


	protected:


	};

};