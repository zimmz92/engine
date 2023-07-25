/// \file ae_system_manager.cpp
/// \brief The script implementing the system manager class.
/// The system manager class is implemented.
#include "ae_system_manager.hpp"
#include "ae_system_base.hpp"

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



    // Give the system an ID and inform the component manager that a new system exists.
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
    // been enabled. When a system is enabled ensure the systems are ordered correctly.
    void AeSystemManager::enableSystem(AeSystemBase* t_system) {
        m_systemDependencySignatures[t_system->m_systemId].set(MAX_NUM_SYSTEMS);
        // Add the system to the unordered map that indicates which systems need to be run.
        m_enabledSystems[t_system->m_systemId] = t_system;
    };



    // Reset the last bit of the systemDependencySignatures high to indicate that the system is disabled and should no
    // longer operate. When a system is disabled make sure to remove that system's entry from the ordered map.
    void AeSystemManager::disableSystem(AeSystemBase* t_system) {
        m_systemDependencySignatures[t_system->m_systemId].reset(MAX_NUM_SYSTEMS);

        // TODO: Check if any other list that is currently enabled depends on this system, if so error.

        // remove the system to the unordered map that indicates which systems need to be run.
        m_enabledSystems.erase(t_system->m_systemId);
        // Remove the system from the execution order list. Since we already checked that no other enabled system needs
        // this system to operate.
        m_systemExecutionOrder.remove(t_system);
    };



    // Remove the system completely by removing it from the system and component managers.
    void AeSystemManager::destroySystem(AeSystemBase* t_system) {

        disableSystem(t_system);
        m_componentManager.removeSystem(t_system->m_systemId);
        this->releaseSystemId(t_system->m_systemId);

    };

    // Orders the currently enabled systems to ensure they are executed in the proper order.
    // TODO: Implement
    void AeSystemManager::orderSystems() {
        // Clear the current system order list so the new list can be built up.
        m_systemExecutionOrder.clear();

        // Loop through the enable systems and put them into the execution order list based on their dependencies.
        for(const auto& [systemId , system] : m_enabledSystems ) {
            if(m_systemExecutionOrder.empty()){
                m_systemExecutionOrder.push_front(system);
            } else {
                // Check what other systems currently in the list this system depends on.
                for (auto& executionSystem : m_systemExecutionOrder) {
                    // Create the signature of the system already in the execution order list.
                    std::bitset<MAX_NUM_SYSTEMS> systemSignature;
                    systemSignature.set(executionSystem->m_systemId);

                    // Compare the signature of the system already in the list to the current system being
                    // evaluated for ordering.
                    if()
                };

                // Check what other systems depend on the current system.
                // Emplace the current system into the list after it's dependencies but before systems that depend on
                // it.
            };
        };
    };





    // Run Systems TODO: Properly implement
    void AeSystemManager::runSystems(){
        // Loop through the enabled systems
        for(auto & m_System : m_enabledSystems){
            m_System.second->setupSystem();
            m_System.second->executeSystem();
            m_System.second->cleanupSystem();
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