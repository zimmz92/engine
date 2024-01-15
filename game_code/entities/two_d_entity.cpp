/// \file two_d_entity.cpp
/// \brief Implements the 2D game object entity.
/// The 2D game object entity is implemented.
#include "two_d_entity.hpp"

namespace ae {

    // Constructor implementation of the TwoDEntity
    TwoDEntity::TwoDEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents) :
            m_model{t_gameComponents.model2DComponent.requiredByEntityReference(this->m_entityId)},
            AeEntity(t_ecs) {};



    // Destructor implementation of the TwoDEntity
    TwoDEntity::~TwoDEntity() {};
}