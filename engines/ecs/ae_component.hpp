#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_component_manager.hpp"
#include "ae_entity.hpp"

#include <cstdint>

namespace ae_ecs {

	template <typename T>
	class AeComponent {

		// ID for the specific component
		static const std::int64_t m_componentTypeId;

	public:
		AeComponent(AeComponentManager& t_componentManager) : m_componentManager{ t_componentManager } {
			m_componentId = m_componentManager.allocateComponentId();

			// TODO: Allow the use of different memory architectures
			m_componentData = (T*)malloc(MAX_COMPONENTS * sizeof(T));
		};

		~AeComponent() {
			free(m_componentData);
		};

		std::uint64_t getComponentId() const { return m_componentId; }

		void useComponent(std::int64_t t_entityId, T t_entityComponentData) {
			m_componentManager.setEntityComponentSigature(t_entityId, m_componentId);
			updateData(t_entityId, t_entityComponentData);
			// TODO: If the entity has been enabled alert system manager that this entity no longer uses this component.
		};

		void removeComponent(std::int64_t t_entityId) {
			m_componentManager.unsetEntityComponentSigature(t_entityId, m_componentId);
			// TODO: Alert system manager that this entity no longer uses this component.
		};

		// Update component data for a specific entity
		void updateData(std::uint64_t t_index, T t_entityComponentData) {
			m_componentData[t_index] = t_entityComponentData;
		};

		// Get data for a specific entitiy
		T getData(std::uint64_t t_index) {
			return m_componentData[t_index];
		};

	private:

		T* m_componentData;

	protected:

		// ID for the unique component created
		std::int64_t m_componentId;

		// Pointer to the component manager
		AeComponentManager& m_componentManager;

	};

	// When a derivitive of the AeComponent class is defined the type ID will be set for the derivative class
	template <class T>
	const std::int64_t AeComponent<T>::m_componentTypeId = AeComponentManager::allocateComponentTypeId<T>();
}