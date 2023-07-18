#include "ae_component_manager.hpp"

#include <stdexcept>

namespace ae_ecs {

    /// Create the component manager and initialize the component ID stack.
	AeComponentManager::AeComponentManager() {
		// Initialize the component ID array with all allowed component IDs
		for (ecs_id i = 0; i < MAX_COMPONENTS; i++) {
			releaseComponentId(MAX_COMPONENTS - 1 - i);
		}
	};

    /// Destroy the component manager.
	AeComponentManager::~AeComponentManager() {};

	/// Release the component ID and put it back on the top of the stack.
	/// \param t_componentId The component ID to be released.
	void AeComponentManager::releaseComponentId(ecs_id t_componentId) {
		if (m_componentIdStackTop >= MAX_COMPONENTS - 1) {
			throw std::runtime_error("Entity ID Stack Overflow!");
		}
		else {
			m_componentIdStackTop++;
			m_componentIdStack[m_componentIdStackTop] = t_componentId;
		}
	};

	/// Assign a component ID by taking the next available off the stack.
	/// \return A component ID.
	ecs_id AeComponentManager::allocateComponentId() {
		if (m_componentIdStackTop <= -1) {
			throw std::runtime_error("Entity ID Stack Underflow! No more entities to give out!");
		}
		else {
			return m_componentIdStack[m_componentIdStackTop--];

		}
	};
}