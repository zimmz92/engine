#pragma once

#include "ae_ecs.hpp"
#include "model_component.hpp"

namespace ae {
	class GameObjectEntity : public ae_ecs::AeEntity<GameObjectEntity> {

	public:
		// Function to create an entity
		GameObjectEntity( modelComponentStructure t_modelComponentDefinition, glm::vec3 t_worldPosition);

		// Function to destroy an entity
		~GameObjectEntity();

	private:


	protected:


	};

};