#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_entity.hpp"

#include <cstdint>

namespace ae {

	template <typename T>
	class AeComponent {

		// ID for the specific component
		static const std::int64_t m_componentId;

	public:
		AeComponent() {
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

	};

	template <class T>
	const std::int64_t AeComponent<T>::m_componentId = AeIdCounters::allocateComponentId<T>();
}