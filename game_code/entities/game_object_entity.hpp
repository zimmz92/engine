#pragma once

#include "ae_ecs.hpp"
#include "game_components.hpp"

namespace ae {

    /// Defines entities that will, or could be, physical objects in the game world.
	class GameObjectEntity : public ae_ecs::AeEntity<GameObjectEntity> {

	public:
        /// Creates the GameObjectEntity using the specified game components.
		explicit GameObjectEntity(GameComponentsStruct& t_gameComponents);

        /// The destructor of the GameObjectEntity.
		~GameObjectEntity();

        /// A reference to the model data of the entity.
        ModelComponentStruct& m_model;

        /// A reference to the world position of the entity.
        WorldPositionComponentStruct& m_worldPosition;

	private:

	protected:
	};

};