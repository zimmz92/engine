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

namespace ae {

	class AeEntityManager {

	public:

		AeEntityManager();
		~AeEntityManager();

		void releaseEntityId(std::int64_t t_value);
		std::int64_t allocateEntityId();
		int64_t getAvailableEntities() { return m_entityIdStackTop + 1; };

		//template <class T>
		//int64_t allocateEntityTypeId() {
		//	m_entityTypeIdCounter++;
		//	return m_entityTypeIdCounter;
		//};

	private:

		std::int64_t m_entityIdStack[MAX_NUM_ENTITIES];
		std::int64_t m_entityIdStackTop = -1;

		//static std::int64_t m_entityTypeIdCounter = -1;

	protected:

	};
}