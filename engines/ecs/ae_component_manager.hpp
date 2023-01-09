#pragma once

#include "ae_ecs_constants.hpp"

#include <cstdint>

namespace ae {

	class AeComponentManager {

	public:

		AeComponentManager();
		~AeComponentManager();

		void releaseComponentId(std::int64_t t_value);
		std::int64_t allocateComponentId();
		int64_t getAvailableComponents() { return m_componentIdStackTop + 1; };

		template <typename T> void addEntityComponentData(std::uint64_t t_entityId, std::uint64_t t_componentId, T t_entityComponentData) {
			
		};

	private:

		std::int64_t m_componentIdStack[MAX_COMPONENTS];
		std::int64_t m_componentIdStackTop = -1;

	protected:

	};
}