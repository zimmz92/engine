/// \file ae_component_manager.cpp
/// \brief The script implementing the component manager class.
/// The component manager class is implemented.
#include "ae_component_manager.hpp"

#include <stdexcept>

namespace ae_ecs {

	// Initialize the component manager.
	AeComponentManager::AeComponentManager() {
		// Initialize the component ID array with all allowed component IDs
		for (ecs_id i = 0; i < MAX_NUM_COMPONENTS; i++) {
			releaseComponentId(MAX_NUM_COMPONENTS - 1 - i);
		};
	};

	// Destroy the component manager.
	AeComponentManager::~AeComponentManager() {};

	// Release the component ID by incrementing the top of stack pointer and putting the component ID being released
	// at that location.
	void AeComponentManager::releaseComponentId(ecs_id t_componentId) {
		if (m_componentIdStackTop >= MAX_NUM_COMPONENTS - 1) {
			throw std::runtime_error("Entity ID Stack Overflow!");
		}
		else {
			m_componentIdStackTop++;
			m_componentIdStack[m_componentIdStackTop] = t_componentId;
		}
	};

	// Allocate a component ID by popping the component ID off the stack, indicated by the top of stack pointer, then
	// decrementing the top of stack pointer to point to the next available component ID.
	ecs_id AeComponentManager::allocateComponentId() {
		if (m_componentIdStackTop <= -1) {
			throw std::runtime_error("Entity ID Stack Underflow! No more component IDs to give out!");
		}
		else {
			return m_componentIdStack[m_componentIdStackTop--];

		}
	};
}