#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_component_manager.hpp"
#include "ae_entity.hpp"

#include <cstdint>

namespace ae {

	template <typename T>
	class AeComponent {

	public:
		AeComponent(AeComponentManager& t_componentManager) : m_componentManager{ t_componentManager } {
			m_componentId = m_componentManager.allocateComponentId();
			m_componentData = (T*)malloc(MAX_COMPONENTS * sizeof(T));
		};
		~AeComponent() {
			free(m_componentData);
		};

		std::uint64_t getComponentId() const { return m_componentId; }

		void updateData(std::uint64_t t_index, T t_entityComponentData) {
			m_componentData[t_index] = t_entityComponentData;
		};

		T getData(std::uint64_t t_index) {
			return m_componentData[t_index];
		};

	private:

		T* m_componentData;

	protected:

		// ID for the unique component created
		std::int64_t m_componentId;

		// Pointer to the component manager
		AeComponentManager& m_componentManager;

	};

}