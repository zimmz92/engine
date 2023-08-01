#include "camera_entity.hpp"

namespace ae {

    CameraEntity::CameraEntity() : GameObjectEntity() {

        m_cameraData = cameraComponent.requiredByEntity(this->m_entityId);
        m_playerControlledData = playerControlledComponent.requiredByEntity(this->m_entityId);

	};

    CameraEntity::~CameraEntity() {

    };
}