#pragma once

#include "ae_ecs_constants.hpp"

namespace ae_ecs {

	class AeComponentBase {

	public:

		AeComponentBase(){};
		virtual ~AeComponentBase(){};

	private:


	protected:

		// ID for the unique component created
		ecs_id m_componentId;

	};
}