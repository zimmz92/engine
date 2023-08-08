#pragma once

#include "game_object_entity.hpp"
#include "game_components.hpp"

namespace ae {

    /// Defines entities that are cameras.
	class CameraEntity : public GameObjectEntity {

	public:
        /// Creates the CameraEntity using the specified game components.
        explicit CameraEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents);

        /// The destructor of the CameraEntity.
		~CameraEntity();

        /// Holds the camera specific data for the entity.
        CameraComponentStruct& m_cameraData;

        /// Specifies if the camera entity is currently controlled by the player.
        PlayerControlledComponentStruct& m_playerControlledData;

        /// Specifies if the camera entity has data for the ubo.
        UboDataFlagsComponentStruct& m_uboDataFlags;

	private:

	protected:

	};

};