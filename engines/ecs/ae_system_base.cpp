/// \file ae_system_base.cpp
/// \brief The script implementing the system base class.
/// The system base class is implemented.
#include "ae_system_base.hpp"

namespace ae_ecs {
    // Function to create the system defining a specific system manager
    AeSystemBase::AeSystemBase(AeSystemManager& t_systemManager) : m_systemManager{ t_systemManager} {
        // Get an ID for the system from the system manager
        m_systemId = m_systemManager.registerSystem();
    };



    // Function to destroy the system
    AeSystemBase::~AeSystemBase() {
        m_systemManager.destroySystem(this);
    };



    // Get the system ID
    ecs_id AeSystemBase::getSystemId() const { return m_systemId; }



    // Register system dependencies with the system manager
    void AeSystemBase::registerSystemDependency(ecs_id t_predecessorSystemId){
        m_systemManager.setSystemDependencySignature(m_systemId,t_predecessorSystemId);
    };



    // Get the frequency in which the system should run
    ecs_systemInterval AeSystemBase::getExecutionInterval() const {return m_executionInterval; };



    // Specify the frequency in which the system runs, for instance an object clean-up system may not run every frame
    void AeSystemBase::setExecutionInterval(ecs_systemInterval t_systemInterval){
        m_executionInterval = t_systemInterval;
    };



    // Tells the system manager to enable this system for execution.
    void AeSystemBase::enableSystem(){
        m_systemManager.enableSystem(this);
    };



    // Tells the system manager to disable this system.
    void AeSystemBase::disableSystem(){
        m_systemManager.disableSystem(this);
    };



    // Tells the system manager that this system is dependent on the specified system
    void AeSystemBase::dependsOnSystem(ecs_id t_systemId){
        m_systemManager.setSystemDependencySignature(this->m_systemId, t_systemId);
    };

    // Tells the system manager that this system is dependent on the specified system
    void AeSystemBase::independentOfSystem(ecs_id t_systemId){
        m_systemManager.unsetSystemDependencySignature(this->m_systemId, t_systemId);
    };

}