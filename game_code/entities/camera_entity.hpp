#pragma once

#include "game_object_entity.hpp"
#include "game_components.hpp"

namespace ae {
	class CameraEntity : public GameObjectEntity {

	public:
		// Function to create an entity
        explicit CameraEntity(GameComponents* t_gameComponents);

		// Function to destroy an entity
		~CameraEntity();

        cameraComponentStructure* m_cameraData;
        playerControlledComponentStructure* m_playerControlledData;

	private:

	protected:

	};

};