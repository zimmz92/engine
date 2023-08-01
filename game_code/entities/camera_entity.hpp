#pragma once

#include "game_object_entity.hpp"
#include "player_controlled_component.hpp"
#include "camera_component.hpp"

namespace ae {
	class CameraEntity : public GameObjectEntity {

	public:
		// Function to create an entity
        CameraEntity();

		// Function to destroy an entity
		~CameraEntity();

        cameraComponentStructure* m_cameraData;
        playerControlledComponentStructure* m_playerControlledData;

	private:

	protected:

	};

};