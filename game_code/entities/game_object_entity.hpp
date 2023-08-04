#pragma once

#include "ae_ecs.hpp"
#include "game_components.hpp"

namespace ae {
	class GameObjectEntity : public ae_ecs::AeEntity<GameObjectEntity> {

	public:
		// Function to create an entity
		explicit GameObjectEntity(GameComponentsStruct* t_gameComponents);

		// Function to destroy an entity
		~GameObjectEntity();

        ModelComponentStruct* m_model;
        WorldPositionComponentStruct* m_worldPosition;

	private:

	protected:
	};

};