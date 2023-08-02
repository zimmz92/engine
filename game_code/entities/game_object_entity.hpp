#pragma once

#include "ae_ecs.hpp"
#include "game_components.hpp"

namespace ae {
	class GameObjectEntity : public ae_ecs::AeEntity<GameObjectEntity> {

	public:
		// Function to create an entity
		explicit GameObjectEntity(GameComponents* t_gameComponents);

		// Function to destroy an entity
		~GameObjectEntity();

        modelComponentStruct* m_model;
        worldPositionComponentStruct* m_worldPosition;

	private:

	protected:
	};

};