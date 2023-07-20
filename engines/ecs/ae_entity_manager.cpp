/// \file ae_entity_manager.cpp
/// \brief The script implementing the entity manager class.
/// The entity manager class is implemented.
#include "ae_entity_manager.hpp"

#include <stdexcept>

namespace ae_ecs {

	// Create the component manager and initialize the entity ID stack.
	AeEntityManager::AeEntityManager() {
		// Initialize the entity ID array with all allowed entity IDs
		for (ecs_id i = 0; i < MAX_NUM_ENTITIES; i++) {
			releaseEntityId(MAX_NUM_ENTITIES - 1 - i);
		}
	};

	// Destroy the entity manager.
	AeEntityManager::~AeEntityManager() {};

    // Release the entity ID by incrementing the top of stack pointer and putting the entity ID being released
    // at that location.
	void AeEntityManager::releaseEntityId(ecs_id t_entityId) {
		if (m_entityIdStackTop >= MAX_NUM_ENTITIES - 1) {
			throw std::runtime_error("Entity ID Stack Overflow! Releasing more entities than should have been able to exist!");
		}
		else {
			// Increment the entity stack pointer then set the top of the stack to the newly released entity ID
			// and remove that entity ID from the living entities array.
			m_entityIdStackTop = m_entityIdStackTop + 1;
			m_entityIdStack[m_entityIdStackTop] = t_entityId;
			m_livingEntities[t_entityId] = false;
		}
	};

    // Allocate an entity ID by popping the entity ID off the stack, indicated by the top of stack pointer, then
    // decrementing the top of stack pointer to point to the next available entity ID.
    ecs_id AeEntityManager::allocateEntityId() {
        if (m_entityIdStackTop <= -1) {
            throw std::runtime_error("Entity ID Stack Underflow! No more entities to give out!");
        } else {
            // Get the new entity ID being allocated from the top of the stack and add the popped entity ID to the living
            // entities array then decrement the stack counter.
            ecs_id allocatedId = m_entityIdStack[m_entityIdStackTop];
            m_livingEntities[allocatedId] = true;
            m_entityIdStackTop = m_entityIdStackTop - 1;
            return allocatedId;
        }
    };


    // Gets the number of available entities that can be allocated by returning the current value of the top of stack
    // pointer and adding one since the stack array starts at 0.
    ecs_id AeEntityManager::getNumEntitiesAvailable() { return m_entityIdStackTop + 1; };


    // Returns the variable that tracks the number of living entities.
    bool* AeEntityManager::getLivingEntities() { return m_livingEntities; };


    //  Give each entity class a unique type ID at runtime and increment the counter used generate the unique IDs.
    template <class T>
    const ecs_id AeEntityManager::allocateEntityTypeId() {
        static const ecs_id staticTypeId{ entityTypeIdCount++ };
        return staticTypeId;
    };
}