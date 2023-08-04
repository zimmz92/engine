#pragma once

#include "game_object_entity.hpp"
#include "game_components.hpp"

namespace ae {
	class CameraEntity : public GameObjectEntity {

	public:
		// Function to create an entity
        explicit CameraEntity(GameComponentsStruct* t_gameComponents);

		// Function to destroy an entity
		~CameraEntity();

        CameraComponentStructure* m_cameraData;
        PlayerControlledComponentStruct* m_playerControlledData;
        UboDataFlagsComponentStruct* m_uboDataFlags;

	private:

	protected:

	};

};