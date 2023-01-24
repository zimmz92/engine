#pragma once

#include "ae_ecs_constants.hpp"

#include <cstdint>
#include <stack>

/*
 The entity manager is responsible for:
	- Allocation/Deallocation of entity IDs
	- Allocation and tracking of entity types
	- Limiting the total number of entities

	- TODO make maximum number of allowed entities could be based off hardware and user settings
*/

namespace ae_ecs {

	class AeEntityManager {

		// entity type ID counter variable
		static inline ecs_id entityTypeIdCount = 0;

	public:

		AeEntityManager();
		~AeEntityManager();

		void releaseEntityId(ecs_id t_value);
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
}