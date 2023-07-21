/// \file ae_system_manager.cpp
/// \brief The script implementing the system manager class.
/// The system manager class is implemented.
#include "ae_system_manager.hpp"
#include "ae_system_base.hpp"

#include <stdexcept>

namespace ae_ecs {

    // Create the system manager and initialize the system ID stack.
    AeSystemManager::AeSystemManager(AeComponentManager& t_componentManager) : m_componentManager{t_componentManager} {
        // Initialize the system ID array with all allowed system IDs
        for (ecs_id i = 0; i < MAX_NUM_SYSTEMS; i++) {
            releaseSystemId(MAX_NUM_SYSTEMS - 1 - i);
        }
    };



    // Destroy the system manager.
    AeSystemManager::~AeSystemManager() {};




    ecs_id AeSystemManager::registerSystem(){
        ecs_id new_system_id = allocateSystemId();
        m_componentManager.registerSystem(new_system_id);
        return new_system_id;
    };



    // Set the system dependency signature bit that indicates that the system depends on another system.
    void AeSystemManager::setSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId) {
        m_systemDependencySignatures[t_systemId].set(t_predecessorSystemId);
    };



    // Reset the system dependency signature bit that indicates that the system depends on another system.
    void AeSystemManager::unsetSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId) {
        m_systemDependencySignatures[t_systemId].reset(t_predecessorSystemId);
    };

    // Enable the system by adding it to the enabled systems map and alerting the component manager that the system has
    // been enabled.
    void AeSystemManager::enableSystem(AeSystemBase* t_system) {
        m_systemDependencySignatures[t_system->m_systemId].set(MAX_NUM_SYSTEMS);
        // Add the system to the unordered map that indicates which systems need to be run.
        m_enabledSystems[t_system->m_systemId] = t_system;
    };



    // Reset the last bit of the systemDependencySignatures high to indicate that the system is disabled and should no
    // longer operate.
    void AeSystemManager::disableSystem(AeSystemBase* t_system) {
        m_systemDependencySignatures[t_system->m_systemId].reset(MAX_NUM_SYSTEMS);
        // remove the system to the unordered map that indicates which systems need to be run.
        m_enabledSystems.erase(t_system->m_systemId);
    };



    // Remove the system completely by removing it from the system and component managers.
    void AeSystemManager::destroySystem(AeSystemBase* t_system) {

        disableSystem(t_system);
        m_componentManager.removeSystem(t_system->m_systemId);
        this->releaseSystemId(t_system->m_systemId);

    };



    // Orders the currently enabled systems to ensure they are executed in the proper order.
    // TODO: Implement
    void orderSystems() {

    };



    // Run Systems TODO: Properly implement
    void AeSystemManager::runSystems(){
        // Loop through the enabled systems
        for(auto & m_System : m_enabledSystems){
            m_System.second->systemSetup();
            m_System.second->systemRun();
            m_System.second->systemCleanup();
        };
    };



    // Release the system ID by incrementing the top of stack pointer and putting the system ID being released
    // at that location.
    void AeSystemManager::releaseSystemId(ecs_id t_systemId) {
        if (m_systemIdStackTop >= MAX_NUM_SYSTEMS - 1) {
            throw std::runtime_error("Entity ID Stack Overflow!");
        }
        else {
            m_systemIdStackTop++;
            m_systemIdStack[m_systemIdStackTop] = t_systemId;
        }
    };



    // Allocate a system ID by popping the system ID off the stack, indicated by the top of stack pointer, then
    // decrementing the top of stack pointer to point to the next available system ID.
    ecs_id AeSystemManager::allocateSystemId() {
        if (m_systemIdStackTop <= -1) {
            throw std::runtime_error("Entity ID Stack Underflow! No more system IDs to give out!");
        }
        else {
            return m_systemIdStack[m_systemIdStackTop--];

        }
    };
}