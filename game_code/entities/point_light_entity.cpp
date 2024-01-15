/// \file point_light_entity.cpp
/// \brief Implements the point light entity.
/// The point light entity is implemented.
#include "point_light_entity.hpp"

namespace ae {

    // The constructor of the point light entity.
    PointLightEntity::PointLightEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents) :
            m_pointLightData{t_gameComponents.pointLightComponent.requiredByEntityReference(this->m_entityId) },
            m_uboDataFlags{t_gameComponents.uboDataFlagsComponent.requiredByEntityReference(this->m_entityId) },
            GameObjectEntity(t_ecs, t_gameComponents) {};



    // Destructor implementation of the PointLightEntity
    PointLightEntity::~PointLightEntity() {};
}