#include "ecs_test.hpp"
#include "ae_ecs.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

void test_ecs() {
    struct testComponentStruct {
        double p;//position
        double v;//velocity
        double a;//acceleration
        double radius;
        double mass;
    };

    ae_ecs::AeComponent<testComponentStruct> liveTestComponentStruct(ae_ecs::ecsComponentManager);

    ae_ecs::AeEntityManager testEntityManager;

    class TestEntityClassA : public ae_ecs::AeEntity<TestEntityClassA> {
    public:
        using ae_ecs::AeEntity<TestEntityClassA>::AeEntity;
        ~TestEntityClassA() {};
    };

    class TestEntityClassB : public ae_ecs::AeEntity<TestEntityClassB> {
    public:
        using ae_ecs::AeEntity<TestEntityClassB>::AeEntity;
        ~TestEntityClassB() {};
    };

    TestEntityClassA TestEntityA(ae_ecs::ecsComponentManager, testEntityManager);
    TestEntityClassB TestEntityB(ae_ecs::ecsComponentManager, testEntityManager);
    TestEntityClassB TestEntityBTwo(ae_ecs::ecsComponentManager, testEntityManager);

    liveTestComponentStruct.updateComponentData(TestEntityA.getEntityId(), {1, 2, 3, 4, 5});
    liveTestComponentStruct.updateComponentData(TestEntityB.getEntityId(), {6, 7, 8, 9, 10});

    struct testComponentStruct readBackA = liveTestComponentStruct.getData(TestEntityA.getEntityId());
    struct testComponentStruct readBackB = liveTestComponentStruct.getData(TestEntityB.getEntityId());

    std::string readBackStringA = "Type ID of entity class A = " + std::to_string(TestEntityA.getEntityTypeId()) + "\n";
    std::cout << readBackStringA;
    readBackStringA = "Value of readBack mass entity A = " + std::to_string(readBackA.mass) + "\n";
    std::cout << readBackStringA;
    readBackStringA = "Value of Entity ID for entity A = " + std::to_string(TestEntityA.getEntityId()) + "\n\n";
    std::cout << readBackStringA;

    std::string readBackStringB = "Type ID of entity class B = " + std::to_string(TestEntityB.getEntityTypeId()) + "\n";
    std::cout << readBackStringB;
    readBackStringB = "Value of readBack mass entity B = " + std::to_string(readBackB.mass) + "\n";
    std::cout << readBackStringB;
    readBackStringB = "Value of Entity ID for entity B = " + std::to_string(TestEntityB.getEntityId()) + "\n\n";
    std::cout << readBackStringB;

    readBackStringB = "Type ID of entity class B2 = " + std::to_string(TestEntityBTwo.getEntityTypeId()) + "\n";
    std::cout << readBackStringB;
    readBackStringB = "Value of Entity ID for entity B2 = " + std::to_string(TestEntityBTwo.getEntityId()) + "\n\n";
    std::cout << readBackStringB;
};