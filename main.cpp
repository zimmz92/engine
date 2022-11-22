#include "Arundos.hpp"

// Test ECS
#include "ae_component.hpp"
#include "ae_component_manager.hpp"
#include "ae_entity_manager.hpp"
#include "ae_entity.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {

        //================================================================
        // START Test ECS a little bit
        //================================================================
        struct testComponentStruct {
            double p;//position
            double v;//velocity
            double a;//acceleration
            double radius;
            double mass;
        };

        ae::AeComponent<testComponentStruct> liveTestComponentStruct;

        ae::AeComponentManager TestComponentManager;

        ae::AeEntityManager testEntityManager;

        class TestEntityClassA : public ae::AeEntity<TestEntityClassA> {
        public:
            using ae::AeEntity<TestEntityClassA>::AeEntity;
            ~TestEntityClassA() {};
        };

        class TestEntityClassB : public ae::AeEntity<TestEntityClassB> {
        public:
            using ae::AeEntity<TestEntityClassB>::AeEntity;
            ~TestEntityClassB() {};
        };

        TestEntityClassA TestEntityA(TestComponentManager, testEntityManager);
        TestEntityClassB TestEntityB(TestComponentManager, testEntityManager);

        liveTestComponentStruct.updateData(TestEntityA.getEntityId(), { 1,2,3,4,5 });
        liveTestComponentStruct.updateData(TestEntityB.getEntityId(), { 6,7,8,9,10 });

        struct testComponentStruct readBackA = liveTestComponentStruct.getData(TestEntityA.getEntityId());
        struct testComponentStruct readBackB = liveTestComponentStruct.getData(TestEntityB.getEntityId());

        std::string readBackStringA = "Type ID of entity class A = " + std::to_string(TestEntityA.getEntityTypeId())+"\n";
        std::cout << readBackStringA;
        readBackStringA = "Value of readBack mass entity A = " + std::to_string(readBackA.mass)+"\n";
        std::cout << readBackStringA;
        readBackStringA = "Value of Entity ID for entity A = " + std::to_string(TestEntityA.getEntityId()) + "\n\n";
        std::cout << readBackStringA;

        std::string readBackStringB = "Type ID of entity class B = " + std::to_string(TestEntityB.getEntityTypeId())+"\n";
        std::cout << readBackStringB;
        readBackStringB = "Value of readBack mass entity B = " + std::to_string(readBackB.mass)+"\n";
        std::cout << readBackStringB;
        readBackStringB = "Value of Entity ID for entity B = " + std::to_string(TestEntityB.getEntityId()) + "\n\n";
        std::cout << readBackStringB;


        //================================================================
        // END Test ECS a little bit
        //================================================================


    ae::Arundos app{};

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}