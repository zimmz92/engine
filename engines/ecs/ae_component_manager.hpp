#pragma once

#include <cstdint>

namespace ae {

	class AeComponentManager {

		AeComponentManager();
		~AeComponentManager();

		static const int MAX_NUM_COMPONENTS = 1024;

	public:

		template <typename T>
		void addEntityComponentData(std::uint64_t t_entityId, std::uint64_t t_componentId, T t_entityComponentData);

	private:

	protected:

	};
}