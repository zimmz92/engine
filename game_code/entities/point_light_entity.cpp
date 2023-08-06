#include "point_light_entity.hpp"

namespace ae {

    PointLightEntity::PointLightEntity(GameComponentsStruct& t_gameComponents) :
            m_pointLightData{ t_gameComponents.pointLightComponent.getDataReference(this->m_entityId) },
            m_uboDataFlags{ t_gameComponents.uboDataFlagsComponent.getDataReference(this->m_entityId) },
            GameObjectEntity(t_gameComponents) {

        // Specify the components that define this entity and where this entity will store data.
        t_gameComponents.pointLightComponent.requiredByEntity(this->m_entityId);
        t_gameComponents.uboDataFlagsComponent.requiredByEntity(this->m_entityId);

	};



    // Destructor implementation of the PointLightEntity
    PointLightEntity::~PointLightEntity() {};
}