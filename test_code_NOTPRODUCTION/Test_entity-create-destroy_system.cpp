/// \file player_input_system.cpp
/// \brief The script implementing the player input system class.
/// The player input system class is implemented.

#include "Test_entity-create-destroy_system.hpp"

namespace ae {

    // Constructor implementation
    CreateDestroyTestSystem::CreateDestroyTestSystem(GLFWwindow* t_window,
                                         AeDevice& t_aeDevice,
                                         ae_ecs::AeECS& t_aeECS,
                                         GameComponents& t_gameComponents,
                                         GameMaterials& t_gameMaterials,
                                         AeSamplers& t_samplers)
            :m_aeDevice{t_aeDevice},
             m_aeECS{t_aeECS},
             m_gameComponents{t_gameComponents},
             m_gameMaterials{t_gameMaterials},
             m_aeSamplers{t_samplers},
             m_window{t_window},
             ae_ecs::AeSystem<CreateDestroyTestSystem>(t_aeECS) {

        // Enable the system so it will run.
        this->enableSystem();

    };



    // Destructor implementation
    CreateDestroyTestSystem::~CreateDestroyTestSystem(){};



    // Set up the system prior to execution. Currently not used.
    void CreateDestroyTestSystem::setupSystem() {};



    // Check for player inputs and apply them to controlled entities.
    void CreateDestroyTestSystem::executeSystem(){

        // Get the key presses that represent the players desire to rotate the model.
        if (glfwGetKey(m_window, keys.destroyCreate) == GLFW_PRESS){
            if(m_entitiesCreated){
                destroyEntities();
            }else{
                createEntities();
            };
        };

    };



    // Clean up the system after execution. Currently not used.
    void CreateDestroyTestSystem::cleanupSystem() {
        m_systemManager.clearSystemEntityUpdateSignatures(m_systemId);
    };

    void CreateDestroyTestSystem::createEntities(){

        for(int i = 0; i<numberOfEntities; i++) {
            for(int j=0; j<numberOfEntities; j++) {
                auto leafEnemy = GameObjectEntity(m_aeECS, m_gameComponents);
                leafEnemy.m_worldPosition = {2.0f+ (1.0f*(float)i), 0.5f, 1.0f +(1.0f*(float)j)};
                leafEnemy.m_model.m_texture = m_aeImage;
                leafEnemy.m_model.m_sampler = m_aeSamplers.getDefaultSampler();
                leafEnemy.m_model.m_model = m_aeModel;
                leafEnemy.m_model.scale = {0.5f , 0.5f, 0.5f};
                leafEnemy.m_model.rotation = {0.0 * glm::two_pi<float>(), 0.0 * glm::two_pi<float>(),
                                              0.0 * glm::two_pi<float>()};

                m_gameMaterials.m_simpleMaterial.m_materialComponent.requiredByEntity(leafEnemy.getEntityId());
                auto &leafEnemyMaterialProperties = m_gameMaterials.m_simpleMaterial.m_materialComponent.getWriteableDataReference(
                        leafEnemy.getEntityId());
                leafEnemyMaterialProperties.m_fragmentTextures[0].m_texture = m_aeImage;
                leafEnemyMaterialProperties.m_fragmentTextures[0].m_sampler = m_aeSamplers.getDefaultSampler();

                leafEnemy.enableEntity();

                m_testEntityIds[i][j] = leafEnemy.getEntityId();
            }
        }

        m_entitiesCreated = true;

    };


    void CreateDestroyTestSystem::destroyEntities(){

        for(int i = 0; i<numberOfEntities; i++) {
            for(int j=0; j<numberOfEntities; j++) {
                m_aeECS.destroyEntity(m_testEntityIds[i][j]);
            }
        }

        m_entitiesCreated = false;
    }
}
