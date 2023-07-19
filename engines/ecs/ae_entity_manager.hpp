/// \file ae_entity_manager.hpp
/// \brief The script defining the entity manager.
/// The entity manager is defined.
#pragma once

#include "ae_ecs_constants.hpp"

#include <cstdint>
#include <stack>

namespace ae_ecs {

    /// A class that is used to register and track entities.
    //TODO make maximum number of allowed entities could be based off hardware and user settings
	class AeEntityManager {

		/// Entity type ID counter variable
		static inline ecs_id entityTypeIdCount = 0;

	public:

        /// Create the entity manager and initialize the entity ID stack.
		AeEntityManager();

        /// Destroy the entity manager.
		~AeEntityManager();

        /// Retract the ID from an entity and put it back on top of the stack.
        /// \param t_entityId The entity ID to be released.
		void releaseEntityId(ecs_id t_entityId);

        /// Assign an entity ID by taking the next available off the stack.
        /// \return A entity ID.
		ecs_id allocateEntityId();


		ecs_id getNumEntitiesAvailable() { return m_entityIdStackTop + 1; };
		bool* getLivingEntities() { return m_livingEntities; };

		// Function to allocate an ID to a specific entity class so every entity spawned from that class can be identifed.
		template <class T>
		static const ecs_id allocateEntityTypeId() {
			static const ecs_id staticTypeId{ entityTypeIdCount++ };
			return staticTypeId;
		};

	private:

		// This is the entity ID stack
		ecs_id m_entityIdStack[MAX_NUM_ENTITIES];
		// Tracks the current top of the entity ID stack
		ecs_id m_entityIdStackTop = -1;
		// Tracks which entities are currently "still alive"
		bool m_livingEntities[MAX_NUM_ENTITIES] = { false };

	protected:

	};

    inline AeEntityManager ecsEntityManager;
}