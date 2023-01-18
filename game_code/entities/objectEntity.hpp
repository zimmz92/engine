#pragma once

#include "ae_ecs.hpp"
#include "model_component.hpp"

namespace ae {
	class ObjectEntity : public ae_ecs::AeEntity<ObjectEntity> {

	public:
		// Function to create an entity
		ObjectEntity( modelComponentStructure t_modelComponentDefinition, glm::vec3 t_worldPosition);

		// Function to destroy an entity
		~ObjectEntity() {};

	private:


	protected:


	};

};