/// \file test_rotate_object_system.cpp
/// \brief The script implementing the system that cycles the point lights.
/// The cycle point light system is implemented.

#include "test_rotate_object_system.hpp"

namespace ae {

    // Constructor implementation
    TestRotateObjectSystem::TestRotateObjectSystem(ae_ecs::AeECS& t_ecs, GameComponents& t_game_components, TimingSystem& t_timingSystem)
    : m_worldPositionComponent{t_game_components.worldPositionComponent},
    m_modelComponent{t_game_components.modelComponent},
    m_testRotationComponent{t_game_components.testRotationComponent},
    m_timingSystem{t_timingSystem},
    ae_ecs::AeSystem<TestRotateObjectSystem>(t_ecs) {

        // Register component dependencies
        m_worldPositionComponent.requiredBySystem(this->getSystemId());
        m_modelComponent.requiredBySystem(this->getSystemId());
        m_testRotationComponent.requiredBySystem(this->getSystemId());

        // Register system dependencies
        this->dependsOnSystem(m_timingSystem.getSystemId());

        // Enable the system so it will run.
        this->enableSystem();
    };



    // Destructor implementation
    TestRotateObjectSystem::~TestRotateObjectSystem(){};



    // Set up the system prior to execution. Currently not used.
    void TestRotateObjectSystem::setupSystem(){};



    // Update the positions of the point lights to make them move in a circle.
    void TestRotateObjectSystem::executeSystem(){

        // Get the entities that use the components this system depends on. Get enabled entities since this will be a
        // system that will conditionally update their component data no matter if previous systems have acted upon
        // them.
        std::vector<ecs_id> validEntityIds = m_systemManager.getEnabledSystemsEntities(this->getSystemId());

        // Loop through the valid entities and update their world position to make them rotate.
        for (ecs_id entityId : validEntityIds){

            ModelComponentStruct& entityModelData = m_modelComponent.getWriteableDataReference(entityId);
            const TestRotationComponentStruct& entityAngularMomentum = m_testRotationComponent.getReadOnlyDataReference(entityId);

            entityModelData.rotation += entityAngularMomentum.m_angularVelocity * m_timingSystem.getDt();
//            glm::vec3 test = {0.1f, 0.1f, 0.1f};
//            entityModelData.rotation += test * m_timingSystem.getDt();
            for(int i = 0; i<3;i++){
                if(entityModelData.rotation[i] > glm::two_pi<float>()){
                    entityModelData.rotation[i] = findMod(entityModelData.rotation[i], glm::two_pi<float>()) * glm::two_pi<float>();
                }
                if(entityModelData.rotation[i] < 0.0f){
                    entityModelData.rotation[i] = glm::two_pi<float>() - (findMod(entityModelData.rotation[i], glm::two_pi<float>()) * glm::two_pi<float>());
                }
            }
        };
    };



    // Clean up the system after execution. Currently not used.
    void TestRotateObjectSystem::cleanupSystem(){
        m_systemManager.clearSystemEntityUpdateSignatures(m_systemId);
    };

    float TestRotateObjectSystem::findMod(float a, float b)
    {
        float mod;
        // Handling negative values
        if (a < 0)
            mod = -a;
        else
            mod =  a;
        if (b < 0)
            b = -b;

        // Finding mod by repeated subtraction

        while (mod >= b)
            mod = mod - b;

        // Sign of result typically depends
        // on sign of "a".
        if (a < 0)
            return -mod;

        return mod;
    }
}
