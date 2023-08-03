#include "point_light_entity.hpp"

namespace ae {

    PointLightEntity::PointLightEntity(GameComponents* t_gameComponents) : GameObjectEntity(t_gameComponents) {

        m_pointLightData = t_gameComponents->pointLightComponent.requiredByEntity(this->m_entityId);
        m_uboDataFlags = t_gameComponents->uboDataFlagsComponent.requiredByEntity(this->m_entityId);

        m_uboDataFlags->hasUboPointLightData = true;

	};

    PointLightEntity::~PointLightEntity() {};
}