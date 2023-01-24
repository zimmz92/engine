#include "ae_entity_manager.hpp"

#include <stdexcept>

namespace ae_ecs {

	// AeEntityManager constructor. Initilizes the entity ID stack.
	AeEntityManager::AeEntityManager() {
		// Initilize the entity ID array with all allowed entity IDs
		for (ecs_id i = 0; i < MAX_NUM_ENTITIES; i++) {
			releaseEntityId(MAX_NUM_ENTITIES - 1 - i);
		}
	};

	// AeEntityManager destructor
	AeEntityManager::~AeEntityManager() {};

	// Unassign a entity ID and put it back on top of the stack.
	void AeEntityManager::releaseEntityId(ecs_id t_value) {
		if (m_entityIdStackTop >= MAX_NUM_ENTITIES - 1) {
			throw std::runtime_error("Entity ID Stack Overflow! Releasing more entities than should have been able to exist!");
		}
		else {
			// Increment the entity stack pointer then set the top of the stack to the newly released entity ID
			// and remove that entity ID from the living entities array.
			m_entityIdStackTop = m_entityIdStackTop + 1;
			m_entityIdStack[m_entityIdStackTop] = t_value;
			m_livingEntities[t_value] = false;
		}
	};

	// Assign a entity ID by taking the next avaiable off the stack.
	std::int64_t AeEntityManager::allocateEntityId() {
	   if(m_entityIdStackTop <= -1) {
			throw std::runtime_error("Entity ID Stack Underflow! No more entities to give out!");
		}
		else {
		   // Get the new entity ID being allocated from the top of the stack and add the popped entity ID to the living
		   // entities array then decrement the stack counter.
		   ecs_id allocatedId = m_entityIdStack[m_entityIdStackTop];
		   m_livingEntities[allocatedId] = true;
		   m_entityIdStackTop = m_entityIdStackTop - 1;
		   return allocatedId;
		}
	};
}