/// \file ae_system_manager.cpp
/// \brief The script implementing the system manager class.
/// The system manager class is implemented.
#include "ae_system_manager.hpp"

#include <stdexcept>

namespace ae_ecs {

    // Create the system manager and initialize the system ID stack.
    AeSystemManager::AeSystemManager() {
        // Initialize the system ID array with all allowed system IDs
        for (ecs_id i = 0; i < MAX_NUM_SYSTEMS; i++) {
            releaseSystemId(MAX_NUM_SYSTEMS - 1 - i);
        }
    };



    // Destroy the system manager.
    AeSystemManager::~AeSystemManager() {};


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



    // Set the system dependency signature bit that indicates that the system depends on another system.
    void AeSystemManager::setSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId) {
        m_systemDependencySignatures[t_systemId].set(t_predecessorSystemId);
    };



    // Reset the system dependency signature bit that indicates that the system depends on another system.
    void AeSystemManager::unsetSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId) {
        m_systemDependencySignatures[t_systemId].reset(t_predecessorSystemId);
    };



    // Set the last bit of the systemDependencySignatures high to indicate that the system is enabled and can begin
    // operation.
    void AeSystemManager::enableSystem(ecs_id t_systemId) {
        m_systemDependencySignatures[t_systemId].set(MAX_NUM_SYSTEMS);
        // TODO: When the entity is set live, force the component manager to update applicable systems lists of valid entities to act upon
    };



    // Reset the last bit of the systemDependencySignatures high to indicate that the system is disabled and should no
    // longer operate.
    void AeSystemManager::disableSystem(ecs_id t_systemId) {
        m_systemDependencySignatures[t_systemId].reset(MAX_NUM_SYSTEMS);
        // TODO: When the entity is disabled, force the component manager to update applicable systems lists of valid entities to act upon
    };
}