#pragma once

#include "ae_ecs.hpp"

namespace ae {
    struct testComponentStructure {
        double p;//position
        double v;//velocity
        double a;//acceleration
        double radius;
        double mass;
    };

    //AeComponent<testComponentStructure> testComponent;


    class testComponentClass : public ae_ecs::AeComponent<testComponentStructure> {
    public:
        testComponentClass(ae_ecs::AeComponentManager& t_componentManager) : AeComponent(t_componentManager) {};
        ~testComponentClass() {};
    private:
    protected:
    };
    
    testComponentClass testComponent(ae_ecs::ecsComponentManager);
}