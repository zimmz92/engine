#pragma once

#include <cstdint>

namespace ae {

	class AeComponentManager {

		AeComponentManager();
		~AeComponentManager();

	public:

		template <typename T>
		void addEntityComponentData(std::uint64_t t_entityId, std::uint64_t t_componentId, T t_entityComponentData);

	private:

	protected:

	};
}