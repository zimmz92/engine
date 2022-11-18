#pragma once

#include "ae_entity.hpp"

#include <cstdint>

namespace ae {

	template <typename T>
	class AeComponent {
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
		std::uint64_t m_componentId;

		T* m_componentData;

	protected:

	};


}