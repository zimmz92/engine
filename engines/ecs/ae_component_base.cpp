/// \file ae_component_base.cpp
/// The ## class is implemented.
#include "ae_component_base.hpp"

// dependencies

// libraries

// std


namespace ae_ecs {

    // Function to create a component, specify the specific manager for the component, and allocate memory for the
    // component data.
    AeComponentBase::AeComponentBase(AeECS& t_ecs) : m_componentManager{ t_ecs.m_ecsComponentManager } {
        // Get an ID for the component from the component manager
        m_componentId = m_componentManager.allocateComponentId(this);
    };

    // Component destructor. Ensures that the component ID and the memory of the component are released.
    AeComponentBase::~AeComponentBase() {
        m_componentManager.releaseComponentId(m_componentId);
    };

    // Gets the ID of the component.
    const ecs_id AeComponentBase::getComponentId() { return m_componentId; }

    // Alerts the component manager that a specific entity uses a component.
    void AeComponentBase::requiredByEntity(ecs_id t_entityId) {
        m_componentManager.entityUsesComponent(t_entityId, m_componentId);
    };

    // Alerts the component manager that a specific entity no longer uses a component.
    void AeComponentBase::unrequiredByEntity(ecs_id t_entityId) {
        this->removeEntityData(t_entityId);
        m_componentManager.entityErstUsesComponent(t_entityId, m_componentId);
        // TODO: Alert system manager that this entity no longer uses this component.
    };

    // Checks with the component manager if an entity uses this component.
    bool AeComponentBase::doesEntityUseThis(ecs_id t_entityId){
        return m_componentManager.isComponentUsed(t_entityId, this->m_componentId);
    };

    // Fetches the entities that use this component.
    std::vector<ecs_id> AeComponentBase::getMyEntities(){
        return m_componentManager.getComponentEntities(this->m_componentId);
    };

    // Alerts the component manager that a system requires this component to operate.
    void AeComponentBase::requiredBySystem(ecs_id t_systemId) {
        m_componentManager.setSystemComponentSignature(t_systemId, m_componentId);
        // TODO: Alert system manager that this system uses this component.
    };

    // Alerts the component manager that a system does not require this component to operate.
    void AeComponentBase::unrequiredBySystem(ecs_id t_systemId) {
        m_componentManager.unsetSystemComponentSignature(t_systemId, m_componentId);
        // TODO: Alert system manager that this system uses this component.
    };

} //namespace ae