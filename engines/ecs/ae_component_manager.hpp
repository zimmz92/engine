#pragma once

#include "ae_ecs_constants.hpp"

#include <cstdint>

namespace ae {

	class AeComponentManager {

		// component type ID counter variable
		static inline std::int64_t componentIdCount = 0;

	public:

		AeComponentManager();
		~AeComponentManager();

		void releaseComponentId(std::int64_t t_value);
		std::int64_t allocateComponentId();
		int64_t getAvailableComponents() { return m_componentIdStackTop + 1; };

		template <typename T> void addEntityComponentData(std::uint64_t t_entityId, std::uint64_t t_componentId, T t_entityComponentData) {
			
		};

		// Function to allocate an ID to a specific component class so every component spawned from that class can be identifed.
		template <class T>
		static const std::int64_t allocateComponentTypeId() {
			static const std::int64_t staticTypeId{ componentIdCount++ };
			return staticTypeId;
		};

	private:

		std::int64_t m_componentIdStack[MAX_COMPONENTS];
		std::int64_t m_componentIdStackTop = -1;

	protected:

	};
}