/// \file camera_entity.cpp
/// \brief Implements the camera entity.
/// The camera entity is implemented.
#include "camera_entity.hpp"

namespace ae {

    // Constructor implementation of the CameraEntity
    CameraEntity::CameraEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents) :
            m_cameraData{t_gameComponents.cameraComponent.requiredByEntityReference(this->m_entityId)},
            m_playerControlledData{t_gameComponents.playerControlledComponent.requiredByEntityReference(this->m_entityId)},
            m_uboDataFlags{t_gameComponents.uboDataFlagsComponent.requiredByEntityReference(this->m_entityId)},
            GameObjectEntity(t_ecs, t_gameComponents) {};



    // Destructor implementation of the GameObjectEntity
    CameraEntity::~CameraEntity() {

    };
}