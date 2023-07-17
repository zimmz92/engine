#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_component_manager.hpp"
#include "ae_entity.hpp"
#include "ae_component_base.hpp"

#include <cstdint>

namespace ae_ecs {

	template <typename T>
	class AeComponent : public AeComponentBase {

		// ID for the specific component
		static const ecs_id m_componentTypeId;

	public:
        // When creating the component ensure to allocate space for the component data
		AeComponent(AeComponentManager& t_componentManager) : m_componentManager{ t_componentManager } {
            // Get an ID for the component from the component manager
			m_componentId = m_componentManager.allocateComponentId();

			// TODO: Allow the use of different memory architectures
			m_componentData = (T*)malloc(MAX_COMPONENTS * sizeof(T));
		};

		~AeComponent() {
			free(m_componentData);
		};

		ecs_id getComponentId() const { return m_componentId; }

		void useComponent(ecs_id t_entityId, T t_entityComponentData) {
            m_componentManager.setEntityComponentSignature(t_entityId, m_componentId);
			updateData(t_entityId, t_entityComponentData);
			// TODO: If the entity has been enabled alert system manager that this entity uses this component.
		};

		void removeComponent(ecs_id t_entityId) {
            m_componentManager.unsetEntityComponentSignature(t_entityId, m_componentId);
			// TODO: Alert system manager that this entity no longer uses this component.
		};

		// Update component data for a specific entity
		void updateData(ecs_id t_index, T t_entityComponentData) {
			// Check that an entity has actually registered that it uses the component before 
			// updating it's data to ensure data will not collide
			m_componentManager.isComponentUsed(t_index, this->m_componentId);
			m_componentData[t_index] = t_entityComponentData;
		};

		// Get data for a specific entitiy
		T getData(ecs_id t_index) {
			return m_componentData[t_index];
		};

	private:

		T* m_componentData;

	protected:

		// Pointer to the component manager
		AeComponentManager& m_componentManager;

	};

	// When a derivitive of the AeComponent class is defined the type ID will be set for the derivative class
	template <class T>
	const ecs_id AeComponent<T>::m_componentTypeId = AeComponentManager::allocateComponentTypeId<T>();
}