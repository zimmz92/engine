#pragma once

#include "ae_component.hpp"
#include "ae_entity.hpp"

namespace ae {


	struct testComponentStruct {
		double p;//position
		double v;//velocity
		double a;//acceleration
		double radius;
		double* mass;
	};

	class TestComponent : public AeComponent<testComponentStruct> {};

	AeComponentManager TestComponentManager;
	AeEntity TestEntity(&TestComponentManager, 0, 0);


}
