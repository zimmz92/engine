#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_component_base.hpp"

#include <cstdint>
#include <bitset>
#include <array>
#include <memory>

namespace ae_ecs {

	class AeComponentManager {

		// component type ID counter variable
		static inline ecs_id componentIdCount = 0;

	public:

		AeComponentManager();
		~AeComponentManager();

		void releaseComponentId(ecs_id t_value);
		ecs_id allocateComponentId();
		ecs_id getAvailableComponents() { return m_componentIdStackTop + 1; };

		std::bitset<MAX_COMPONENTS+1>*  getComponentSignature() {	return m_componentSignatures; };

		void setEntityComponentSigature(ecs_id t_entityId, ecs_id t_componentId) {
			m_componentSignatures[t_entityId].set(t_componentId);
		};

		void unsetEntityComponentSigature(ecs_id t_entityId, ecs_id t_componentId) {
			m_componentSignatures[t_entityId].reset(t_componentId);
		};

		// Checks to see if an entity uses a component
		bool isComponentUsed(ecs_id t_entityId, ecs_id t_componentId) {
			return m_componentSignatures[t_entityId].test(t_componentId);
		};

		// Enables entity from be acted upon by systems
		void enableEntity(ecs_id t_entityId) {
			m_componentSignatures[t_entityId].set(MAX_COMPONENTS);
			// TODO: Alert system manager that this entity is now enabled.
		};

		// Disables entity from be acted upon by systems
		void disableEntity(ecs_id t_entityId) {
			m_componentSignatures[t_entityId].reset(MAX_COMPONENTS);
			// TODO: Alert system manager that this entity is now disabled.
		};

		// Function to allocate an ID to a specific component class so every component spawned from that class can be identifed.
		template <class T>
		static const ecs_id allocateComponentTypeId() {
			static const ecs_id staticTypeId{ componentIdCount++ };
			return staticTypeId;
		};

	private:

		// componenet ID stack and a counter used for the stack
		ecs_id m_componentIdStack[MAX_COMPONENTS];
		ecs_id m_componentIdStackTop = -1;

		std::shared_ptr<AeComponentBase> m_components[MAX_COMPONENTS];

		// vector storing the components used for each entity, last bit is to indicate that the entity is living aka fully initialized. 
		// After initilization adding or removing a component forces intilization data to be included. 
		std::bitset<MAX_COMPONENTS+1> m_componentSignatures[MAX_NUM_ENTITIES] = { 0 };

	protected:

	};
}