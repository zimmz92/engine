/// \file ae_component_manager.hpp
/// \brief The script defining the component manager.
/// The component manager is defined.
#pragma once

#include "ae_ecs_constants.hpp"

#include <cstdint>
#include <bitset>
#include <array>
#include <memory>
#include <vector>
#include <unordered_map>

namespace ae_ecs {

    /// A class that is used to register and organize components and correlate them to entities and systems.
	class AeComponentManager {

		/// component type ID counter variable
		static inline ecs_id componentIdCount = 0;

	public:

        /// Create the component manager and initialize the component ID stack.
		AeComponentManager();

        /// Destroy the component manager.
		~AeComponentManager();

        /// Release the component ID and put it back on the top of the stack.
        /// \param t_componentId The component ID to be released.
		void releaseComponentId(ecs_id t_componentId);

        /// Assign a component ID by taking the next available off the stack.
        /// \return A component ID.
		ecs_id allocateComponentId();

        /// Gets the number of components that have not been allocated from the stack and are therefore available for
        /// use.
        /// \return Number of components still available to be used.
		ecs_id getNumAvailableComponents();

        /// Gets the component signature for an entity.
        /// \param t_entityId The ID of the entity.
        /// \return A bitset array that indicates the components utilized by an entity.
		std::bitset<MAX_NUM_COMPONENTS + 1>  getComponentSignature(ecs_id t_entityId);

        ///  A function that sets the field in the entity component signature corresponding to the specific component.
        /// \param t_entityId  The ID of the entity.
        /// \param t_componentId The ID of the component to be added as used for the entity.
		void setEntityComponentSignature(ecs_id t_entityId, ecs_id t_componentId);

        /// A function that unsets the field in the entity component signature corresponding to the specific component.
        /// \param t_entityId The ID of the entity.
        /// \param t_componentId The ID of the component to be removed as used for the entity.
		void unsetEntityComponentSignature(ecs_id t_entityId, ecs_id t_componentId);

        /// Enables entity allowing it to be acted upon by systems
        /// \param t_entityId The ID of the entity
        void enableEntity(ecs_id t_entityId);

        /// Disables entity preventing it from being acted upon by systems
        /// \param t_entityId The ID of the entity
        void disableEntity(ecs_id t_entityId);

        /// Informs the component manager that an entities component data has been updated and sets a flag used to
        /// inform systems using that component of the change in an entities data.
        /// \param t_entityId The ID of the entity
        /// \param t_componentId The ID of the component.
        void entitiesComponentUpdated(ecs_id t_entityId, ecs_id t_componentId);

        /// Checks to see if an entity uses a component.
        /// \param t_entityId The ID of the entity
        /// \param t_componentId The ID of the component.
        /// \return true if the entity uses the component.
        bool isComponentUsed(ecs_id t_entityId, ecs_id t_componentId);

        /// Resets/removes the entity data from a component.
        /// \param t_entityId The ID of the entity
        void destroyEntity(ecs_id t_entityId);

        /// Register the system with the component system
        /// \param t_systemId The ID of the system to be registered.
        void registerSystem(ecs_id t_systemId);

        ///  A function that sets the field in the system component signature corresponding to the specific component.
        /// \param t_systemId The ID of the system.
        /// \param t_componentId The ID of the component to be added as required for the system.
        void setSystemComponentSignature(ecs_id t_systemId, ecs_id t_componentId);

        ///  A function that unsets the field in the system component signature corresponding to the specific component.
        /// \param t_systemId The ID of the system.
        /// \param t_componentId The ID of the component to be removed as required for the system.
        void unsetSystemComponentSignature(ecs_id t_systemId, ecs_id t_componentId);

        /// Clears the system entity update signature for the specified system. Usually used at the end of a system's
        /// execution loop.
        /// \param t_systemId The ID of the system.
        void clearSystemEntityUpdateSignatures(ecs_id t_systemId);

        /// A function to remove a systems component usage information from the component manager.
        /// \param t_systemId The ID of the system to be removed.
        void removeSystem(ecs_id t_systemId);

        /// Compares system component signatures to the entity component signatures and returns a list of all enabled
        /// entities compatible with the system.
        /// \param t_systemId The ID of the system to be removed.
        std::vector<ecs_id> getEnabledSystemsEntities(ecs_id t_systemId);

        /// Returns a list of enabled, compatible, entities that the system is to utilize that have had data been updated since the
        /// last system run loop.
        /// \param t_systemId The ID of the system to be removed.
        std::vector<ecs_id> getUpdatedSystemEntities(ecs_id t_systemId);

        /// Returns a list of entities that have been destroyed since the system last ran.
        /// \param t_systemId The ID of the system to be removed.
        std::vector<ecs_id> getDestroyedSystemEntities(ecs_id t_systemId);

        /// Returns the entities that use the specified component;
        /// \param t_componentId The component ID the list of entities should be returned for.
        std::vector<ecs_id> getComponentEntities(ecs_id t_componentId);

		/// Function to allocate an ID to a specific component class so every component spawned from that class can be identified.
		/// \tparam T The component class being allocated an ID.
		/// \return The component class ID.
		template <class T>
		static ecs_id allocateComponentTypeId() {
            //  Give each component class a unique type ID at runtime and increment the counter used generate the unique IDs.
            static const ecs_id staticTypeId{ componentIdCount++ };
            return staticTypeId;
        };

	private:

		/// Component ID stack and a counter used for the stack
		ecs_id m_componentIdStack[MAX_NUM_COMPONENTS];

        /// The component ID stack current top value pointer.
        ecs_id m_componentIdStackTop = -1;

		/// Vector storing the components used for each entity, last bit is to indicate that the entity is fully
        /// initialized and ready to go live. After initialization adding or removing a component forces
        /// initialization data to be included.
		std::bitset<MAX_NUM_COMPONENTS + 1> m_entityComponentSignatures[MAX_NUM_ENTITIES] = {0};

        /// Unordered map storing the components required for each active system.
        std::unordered_map<ecs_id,std::bitset<MAX_NUM_COMPONENTS + 1>> m_systemComponentSignatures;

        /// Unordered map storing the entity component update status.
        std::unordered_map<ecs_id,std::vector<ecs_id>> m_systemEntityUpdateSignatures;

        /// Unordered map storing the entity destruction status.
        std::unordered_map<ecs_id,std::vector<ecs_id>> m_systemEntityDestroyedSignatures;

	protected:

	};
}