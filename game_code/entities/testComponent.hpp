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


    class testComponentClass : public AeComponent<testComponentStructure> {
    public:
        testComponentClass(AeComponentManager& t_componentManager) : AeComponent(t_componentManager) {};
        ~testComponentClass() {};
    private:
    protected:
    };
    
    AeComponentManager testComponentManager;
    testComponentClass testComponent(testComponentManager);
}