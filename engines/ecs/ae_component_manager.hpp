#pragma once

#include "ae_ecs_constants.hpp"

#include <cstdint>
#include <bitset>
#include <array>

namespace ae_ecs {

	class AeComponentManager {

		// component type ID counter variable
		static inline std::int64_t componentIdCount = 0;

	public:

		AeComponentManager();
		~AeComponentManager();

		void releaseComponentId(std::int64_t t_value);
		std::int64_t allocateComponentId();
		int64_t getAvailableComponents() { return m_componentIdStackTop + 1; };

		std::bitset<MAX_COMPONENTS>*  getComponentSignature() {	return m_componentSignatures; };

		void setEntityComponentSigature(std::int64_t t_entityId, int64_t t_componentId) {
			m_componentSignatures[t_entityId].set(t_componentId);
		};

		void unsetEntityComponentSigature(std::int64_t t_entityId, int64_t t_componentId) {
			m_componentSignatures[t_entityId].reset(t_componentId);
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

		// vector storing the components used for each entity
		std::bitset<MAX_COMPONENTS> m_componentSignatures[MAX_NUM_ENTITIES] = { 0 };

	protected:

	};
}