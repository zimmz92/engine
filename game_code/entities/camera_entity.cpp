#include "camera_entity.hpp"

namespace ae {

    CameraEntity::CameraEntity(GameComponents* t_gameComponents) : GameObjectEntity(t_gameComponents) {

        m_cameraData = t_gameComponents->cameraComponent.requiredByEntity(this->m_entityId);
        m_playerControlledData = t_gameComponents->playerControlledComponent.requiredByEntity(this->m_entityId);

	};

    CameraEntity::~CameraEntity() {

    };
}