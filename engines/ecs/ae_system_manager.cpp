#include "ae_system_manager.hpp"

#include <stdexcept>

namespace ae_ecs {

    /// Create the system manager and initialize the system ID stack.
    AeSystemManager::AeSystemManager() {
        // Initialize the system ID array with all allowed system IDs
        for (ecs_id i = 0; i < MAX_NUM_SYSTEMS; i++) {
            releaseSystemId(MAX_NUM_SYSTEMS - 1 - i);
        }
    };

    /// Destroy the system manager.
    AeSystemManager::~AeSystemManager() {};

    /// Release the system ID and put it back on the top of the stack.
    /// \param t_systemId The system ID to be released.
    void AeSystemManager::releaseSystemId(ecs_id t_systemId) {
        if (m_systemIdStackTop >= MAX_NUM_SYSTEMS - 1) {
            throw std::runtime_error("Entity ID Stack Overflow!");
        }
        else {
            m_systemIdStackTop++;
            m_systemIdStack[m_systemIdStackTop] = t_systemId;
        }
    };

    /// Assign a system ID by taking the next available off the stack.
    /// \return A system ID.
    ecs_id AeSystemManager::allocateSystemId() {
        if (m_systemIdStackTop <= -1) {
            throw std::runtime_error("Entity ID Stack Underflow! No more system IDs to give out!");
        }
        else {
            return m_systemIdStack[m_systemIdStackTop--];

        }
    };
}