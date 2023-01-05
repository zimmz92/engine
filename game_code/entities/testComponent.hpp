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

    AeComponent<testComponentStructure> testComponent;

    //class testComponentClass : public AeComponent<testComponentStructure> {
    //    using AeComponent<testComponentStructure>::AeComponent;
    //};
        
    //testComponentClass testComponent;
}