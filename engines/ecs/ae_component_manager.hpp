#pragma once

#include "ae_ecs_constants.hpp"

#include <cstdint>

namespace ae {

	class AeComponentManager {

	public:

		AeComponentManager() {};
		~AeComponentManager() {};

		template <typename T> void addEntityComponentData(std::uint64_t t_entityId, std::uint64_t t_componentId, T t_entityComponentData) {};

	private:

	protected:

	};
}