#include "camera_entity.hpp"

namespace ae {

    // Constructor implementation of the CameraEntity
    CameraEntity::CameraEntity(GameComponentsStruct& t_gameComponents) :
            m_cameraData{t_gameComponents.cameraComponent.getDataReference(this->m_entityId)},
            m_playerControlledData{t_gameComponents.playerControlledComponent.getDataReference(this->m_entityId)},
            m_uboDataFlags{t_gameComponents.uboDataFlagsComponent.getDataReference(this->m_entityId)},
            GameObjectEntity(t_gameComponents) {

        // Specify the components that define this entity and where this entity will store data.
        t_gameComponents.cameraComponent.requiredByEntity(this->m_entityId);
        t_gameComponents.playerControlledComponent.requiredByEntity(this->m_entityId);
        t_gameComponents.uboDataFlagsComponent.requiredByEntity(this->m_entityId);

	};



    // Destructor implementation of the GameObjectEntity
    CameraEntity::~CameraEntity() {

    };
}