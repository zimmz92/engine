#pragma once

#include "ae_ecs.hpp"
#include "model_component.hpp"
#include "world_position_component.hpp"

namespace ae {
	class GameObjectEntity : public ae_ecs::AeEntity<GameObjectEntity> {

	public:
		// Function to create an entity
		GameObjectEntity();

		// Function to destroy an entity
		~GameObjectEntity();

        modelComponentStruct* m_model;
        worldPositionComponentStruct* m_worldPosition;

	private:

	protected:
	};

};