#include "ae_component_manager.hpp"

#include <stdexcept>

namespace ae_ecs {

	AeComponentManager::AeComponentManager() {
		// Initilize the component ID array with all allowed component IDs
		for (ecs_id i = 0; i < MAX_COMPONENTS; i++) {
			releaseComponentId(MAX_COMPONENTS - 1 - i);
		}
	};
	AeComponentManager::~AeComponentManager() {};

	// Unassign a component ID and put it back on top of the stack
	void AeComponentManager::releaseComponentId(ecs_id t_value) {
		if (m_componentIdStackTop >= MAX_COMPONENTS - 1) {
			throw std::runtime_error("Entity ID Stack Overflow!");
		}
		else {
			m_componentIdStackTop++;
			m_componentIdStack[m_componentIdStackTop] = t_value;
		}
	};

	// Assign a component ID by taking the next avaiable off the stack
	ecs_id AeComponentManager::allocateComponentId() {
		if (m_componentIdStackTop <= -1) {
			throw std::runtime_error("Entity ID Stack Underflow! No more entities to give out!");
		}
		else {
			return m_componentIdStack[m_componentIdStackTop--];

		}
	};
}