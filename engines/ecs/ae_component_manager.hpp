/*! \file ae_component_manager.hpp
    \brief The script defining the component manager.

    The component manager is defined.

*/
#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_component_base.hpp"

#include <cstdint>
#include <bitset>
#include <array>
#include <memory>

namespace ae_ecs {

	class AeComponentManager {

		/// component type ID counter variable
		static inline ecs_id componentIdCount = 0;

	public:

		AeComponentManager();
		~AeComponentManager();

		void releaseComponentId(ecs_id t_componentId);
		ecs_id allocateComponentId();

        /// Gets the number of components that have not been allocated from the stack and are therefore available for
        /// use.
        /// \return Number of components still available to be used.
		ecs_id getAvailableComponents() { return m_componentIdStackTop + 1; };

        /// Gets the component signature for an entity.
        /// \param t_entityId The ID of the entity.
        /// \return A bitset array that indicates the components utilized by an entity.
		std::bitset<MAX_COMPONENTS+1>  getComponentSignature(ecs_id t_entityId) {	return m_entityComponentSignatures[t_entityId]; };

        ///  A function that sets the field in the entity component signature corresponding to the specific component.
        /// \param t_entityId  The ID of the entity.
        /// \param t_componentId The ID of the component to be added as used for the entity.
		void setEntityComponentSignature(ecs_id t_entityId, ecs_id t_componentId) {
			m_entityComponentSignatures[t_entityId].set(t_componentId);
            // TODO: When the entity sets a component, and is live, force the component manager to update applicable systems lists of valid entities to act upon
		};

        /// A function that unsets the field in the entity component signature corresponding to the specific component.
        /// \param t_entityId The ID of the entity.
        /// \param t_componentId The ID of the component to be removed as used for the entity.
		void unsetEntityComponentSignature(ecs_id t_entityId, ecs_id t_componentId) {
			m_entityComponentSignatures[t_entityId].reset(t_componentId);
            // TODO: When the entity removes a component force the component manager to update applicable systems lists of valid entities to act upon
		};

        ///  A function that sets the field in the system component signature corresponding to the specific component.
        /// \param t_systemId The ID of the system.
        /// \param t_componentId The ID of the component to be added as required for the system.
        void setSystemComponentSignature(ecs_id t_systemId, ecs_id t_componentId) {
            m_systemComponentSignatures[t_systemId].set(t_componentId);
        };

        ///  A function that unsets the field in the system component signature corresponding to the specific component.
        /// \param t_systemId The ID of the system.
        /// \param t_componentId The ID of the component to be removed as required for the system.
        void unsetSystemComponentSignature(ecs_id t_systemId, ecs_id t_componentId) {
            m_systemComponentSignatures[t_systemId].reset(t_componentId);
        };

		/// Checks to see if an entity uses a component.
		/// \param t_entityId The ID of the entity
		/// \param t_componentId The ID of the component.
		/// \return true if the entity uses the component.
		bool isComponentUsed(ecs_id t_entityId, ecs_id t_componentId) {
			return m_entityComponentSignatures[t_entityId].test(t_componentId);
		};

		/// Enables entity from be acted upon by systems
		/// \param t_entityId The ID of the entity
		void enableEntity(ecs_id t_entityId) {
			m_entityComponentSignatures[t_entityId].set(MAX_COMPONENTS);
            // TODO: When the entity is set live, force the component manager to update applicable systems lists of valid entities to act upon
        };

		/// Disables entity from be acted upon by systems
		/// \param t_entityId The ID of the entity
		void disableEntity(ecs_id t_entityId) {
			m_entityComponentSignatures[t_entityId].reset(MAX_COMPONENTS);
            // TODO: When the entity is disabled, force the component manager to update applicable systems lists of valid entities to act upon
		};

        /// Compares system component signatures to the entity component signatures and returns a list of valid entities
        /// to the systems to act upon.
        void updateSystemsEntities() {
            // TODO: Create a class that compares system component signatures to the entity component signatures and returns a list of valid entities to the systems to act upon.
        }

		/// Function to allocate an ID to a specific component class so every component spawned from that class can be identified.
		/// \tparam T The component class being allocated an ID.
		/// \return The component class ID.
		template <class T>
		static const ecs_id allocateComponentTypeId() {
			static const ecs_id staticTypeId{ componentIdCount++ };
			return staticTypeId;
		};

	private:

		/// Component ID stack and a counter used for the stack
		ecs_id m_componentIdStack[MAX_COMPONENTS];

        /// The component ID stack current top value pointer.
        ecs_id m_componentIdStackTop = -1;

		/// Vector storing the components used for each entity, last bit is to indicate that the entity is fully
        /// initialized and ready to go live. After initialization adding or removing a component forces
        /// initialization data to be included.
		std::bitset<MAX_COMPONENTS+1> m_entityComponentSignatures[MAX_NUM_ENTITIES] = {0 };

        /// Vector storing the components required for each system, last bit is to indicate that the system is active.
        std::bitset<MAX_COMPONENTS+1> m_systemComponentSignatures[MAX_NUM_ENTITIES] = {0 };

	protected:

	};
}