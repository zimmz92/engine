#include "ae_entity_manager.hpp"

#include <stdexcept>

namespace ae {

	AeEntityManager::AeEntityManager() {
		// Initilize the entity ID array with all allowed entity IDs
		for (std::int64_t i = 0; i < MAX_NUM_ENTITIES; i++) {
			releaseEntityId(MAX_NUM_ENTITIES - 1 - i);
		}
	};
	AeEntityManager::~AeEntityManager() {};

	// Unassign a entity ID and put it back on top of the stack
	void AeEntityManager::releaseEntityId(std::int64_t t_value) {
		if (m_entityIdStackTop >= MAX_NUM_ENTITIES - 1) {
			throw std::runtime_error("Entity ID Stack Overflow!");
		}
		else {
			m_entityIdStackTop++;
			m_entityIdStack[m_entityIdStackTop] = t_value;
		}
	};

	// Assign a entity ID by taking the next avaiable off the stack
	std::int64_t AeEntityManager::allocateEntityId() {
	   if(m_entityIdStackTop <= -1) {
			throw std::runtime_error("Entity ID Stack Underflow! No more entities to give out!");
		}
		else {
			return m_entityIdStack[m_entityIdStackTop--];
			
		}
	};

}