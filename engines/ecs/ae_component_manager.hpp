/// \file ae_component_manager.hpp
/// \brief The script defining the component manager.
/// The component manager is defined.
#pragma once

#include "ae_ecs_constants.hpp"

#include <cstdint>
#include <bitset>
#include <array>
#include <memory>

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

        /// Checks to see if an entity uses a component.
        /// \param t_entityId The ID of the entity
        /// \param t_componentId The ID of the component.
        /// \return true if the entity uses the component.
        bool isComponentUsed(ecs_id t_entityId, ecs_id t_componentId);

        /// TODO: Document the usage of this function
        void removeEntity(ecs_id t_entityId);

        ///  A function that sets the field in the system component signature corresponding to the specific component.
        /// \param t_systemId The ID of the system.
        /// \param t_componentId The ID of the component to be added as required for the system.
        void setSystemComponentSignature(ecs_id t_systemId, ecs_id t_componentId);

        ///  A function that unsets the field in the system component signature corresponding to the specific component.
        /// \param t_systemId The ID of the system.
        /// \param t_componentId The ID of the component to be removed as required for the system.
        void unsetSystemComponentSignature(ecs_id t_systemId, ecs_id t_componentId);

        /// TODO: Document the usage of this function
        void removeSystem(ecs_id t_systemId);

        /// Compares system component signatures to the entity component signatures and returns a list of valid entities
        /// to the systems to act upon.
        /// TODO: Document the usage of this function
        void updateSystemsEntities();

		/// Function to allocate an ID to a specific component class so every component spawned from that class can be identified.
		/// \tparam T The component class being allocated an ID.
		/// \return The component class ID.
		template <class T>
		static const ecs_id allocateComponentTypeId();

	private:

		/// Component ID stack and a counter used for the stack
		ecs_id m_componentIdStack[MAX_NUM_COMPONENTS];

        /// The component ID stack current top value pointer.
        ecs_id m_componentIdStackTop = -1;

		/// Vector storing the components used for each entity, last bit is to indicate that the entity is fully
        /// initialized and ready to go live. After initialization adding or removing a component forces
        /// initialization data to be included.
		std::bitset<MAX_NUM_COMPONENTS + 1> m_entityComponentSignatures[MAX_NUM_ENTITIES] = {0};

        /// Vector storing the components required for each system, last bit is to indicate that the system is active.
        std::bitset<MAX_NUM_COMPONENTS + 1> m_systemComponentSignatures[MAX_NUM_ENTITIES] = {0};

	protected:

	};

    /// Declare the default component manager for the ecs.
    inline AeComponentManager ecsComponentManager;
}