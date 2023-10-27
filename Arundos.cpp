/// \file Arundos.cpp
/// \brief Implements Arundos.
/// The Arundos application implemented.

#include "Arundos.hpp"
#include "ae_buffer.hpp"

#include "game_object_entity.hpp"
#include "camera_entity.hpp"
#include "point_light_entity.hpp"
#include "two_d_entity.hpp"
#include "ae_image.hpp"

// libraries
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace ae {

    // The constructor of the Arundos application. Sets up the base application.
    Arundos::Arundos() {
        // Load the default game objects into the scene.
        loadGameObjects();
    };



    // The destructor of the Arundos application.
    Arundos::~Arundos() {};



    // While the window should remain open continue to run the application and it's systems..
    void Arundos::run() {

        while (!m_aeWindow.shouldClose()) {
            // Check to see if there are any user input events.
            glfwPollEvents();

            // TODO: Update the components so that direction is with the entities world position data. Knowing which way
            //  an entity is facing in the world is a requirement!
            // TODO: Implement order independent transparency
            // Instruct the entity component system (ECS) to run it's system to update applicable entity component data.
            m_aeECS.runSystems();

            // TODO allow for option to limit frame timing, aka lock FPS, if desired but allow other systems to continue to run
            //time_delta = glm::min(time_delta, MAX_FRAME_TIME);
        }

        // Wait for the completion of all outstanding queue operations.
        vkDeviceWaitIdle(m_aeDevice.device());
    }



    // Loads the default game objects into the Arundos application.
    void Arundos::loadGameObjects() {

        //==============================================================================================================
        // Make the game camera using ECS
        CameraEntity cameraECS{m_aeECS,m_gameComponents};
        cameraECS.m_playerControlledData.isCurrentlyControlled = true;
        cameraECS.m_worldPosition.phi = -2.5f;
        cameraECS.m_cameraData.usePerspectiveProjection = true;
        cameraECS.m_cameraData.isMainCamera = true;
        cameraECS.m_uboDataFlags.hasUboCameraData = true;
        cameraECS.enableEntity();
        //==============================================================================================================



        //==============================================================================================================
        // Load the flat vase object model from the file
        std::shared_ptr<AeModel> aeModel = AeModel::createModelFromFile(m_aeDevice, "assets_NOTPRODUCTION/models/TEMP_flat_vase.obj");
        // ECS version of flatVase
        auto testFlatVase = GameObjectEntity(m_aeECS,m_gameComponents);
        testFlatVase.m_worldPosition = {-0.5f, 0.5f, 0.0f };
        testFlatVase.m_model.m_model = aeModel;
        testFlatVase.m_model.scale = {3.0f, 1.5f, 3.0f };
        testFlatVase.enableEntity();


        // Load the smooth vase object model from the file
        aeModel = AeModel::createModelFromFile(m_aeDevice, "assets_NOTPRODUCTION/models/TEMP_smooth_vase.obj");
        // ECS version of smoothVase
        auto testSmoothVase = GameObjectEntity(m_aeECS,m_gameComponents);
        testSmoothVase.m_worldPosition = {0.5f, 0.5f, 0.0f };
        testSmoothVase.m_model.m_model = aeModel;
        testSmoothVase.m_model.scale = {3.0f, 1.5f, 3.0f };
        testSmoothVase.enableEntity();

        // Load the floor object model from the file
        aeModel = AeModel::createModelFromFile(m_aeDevice, "assets_NOTPRODUCTION/models/TEMP_quad.obj");
        // ECS version of the floor
        auto testFloor = GameObjectEntity(m_aeECS,m_gameComponents);
        testFloor.m_worldPosition = {0.0f, 0.5f, 0.0f };
        testFloor.m_model.m_model = aeModel;
        testFloor.m_model.scale = {3.0f, 1.0f, 3.0f };
        testFloor.enableEntity();

        // Load the viking object model from the file
        aeModel = AeModel::createModelFromFile(m_aeDevice, "assets_NOTPRODUCTION/models/TEMP_viking_room.obj");
        std::shared_ptr<AeImage> aeImage = AeImage::createModelFromFile(m_aeDevice,"assets_NOTPRODUCTION/models/TEMP_viking_room.png");
        // ECS version of the floor
        auto vikingRoom = GameObjectEntity(m_aeECS,m_gameComponents);
        vikingRoom.m_worldPosition = {0.0f, 0.25f, 1.5f };
        vikingRoom.m_model.m_texture = aeImage;
        vikingRoom.m_model.m_sampler = m_aeSamplers.getDefaultSampler();
        vikingRoom.m_model.m_model = aeModel;
        vikingRoom.m_model.scale = {1.0f, 1.0f, 1.0f };
        vikingRoom.m_model.rotation = {0.25 * glm::two_pi<float>(), 0.75 * glm::two_pi<float>(), 0.5* glm::two_pi<float>()};
        vikingRoom.enableEntity();

        // Load the leaf enemy object model from the file
        aeModel = AeModel::createModelFromFile(m_aeDevice, "assets/models/leaf_enemy_w_tongue.obj");
        aeImage = AeImage::createModelFromFile(m_aeDevice,"assets/models/leaf_enemy_body_w_tongue.png");
        // ECS version of the floor
        auto leafEnemy = GameObjectEntity(m_aeECS,m_gameComponents);
        leafEnemy.m_worldPosition = {2.0f, 0.5f, 1.0f };
        leafEnemy.m_model.m_texture = aeImage;
        leafEnemy.m_model.m_sampler = m_aeSamplers.getDefaultSampler();
        leafEnemy.m_model.m_model = aeModel;
        leafEnemy.m_model.scale = {0.5f, 0.5f, 0.5f };
        leafEnemy.m_model.rotation = {0.0 * glm::two_pi<float>(), 0.0 * glm::two_pi<float>(), 0.0* glm::two_pi<float>()};
        m_gameMaterials.m_simpleMaterial.m_materialComponent.requiredByEntity(leafEnemy.getEntityId());
        auto leafEnemyMaterialProperties = m_gameMaterials.m_simpleMaterial.m_materialComponent.getWriteableDataReference(leafEnemy.getEntityId());
        leafEnemyMaterialProperties = aeImage;
        leafEnemy.enableEntity();
        //==============================================================================================================



        //==============================================================================================================
        // Make the point lights using the ECS for testing
        // Create Point Lights
        std::vector<glm::vec3> lightColors{
                {1.f, .1f, .1f},
                {.1f, .1f, 1.f},
                {.1f, 1.f, .1f},
                {1.f, 1.f, .1f},
                {.1f, 1.f, 1.f},
                {1.f, 1.f, 1.f}  //
        };

        for (int i = 0; i < lightColors.size(); i++) {
            auto pointLight = PointLightEntity(m_aeECS,m_gameComponents);
            pointLight.m_uboDataFlags.hasUboPointLightData = true;
            pointLight.m_pointLightData.lightIntensity = 0.2f;
            pointLight.m_pointLightData.m_color = lightColors[i];
            auto rotateLight = glm::rotate(
                    glm::mat4(1.0f),
                    (i * glm::two_pi<float>()) / lightColors.size(),
                    {0.0f, -1.0f, 0.0f});
            glm::vec3 worldPosition = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
            pointLight.m_worldPosition.rho = worldPosition.x;
            pointLight.m_worldPosition.theta = worldPosition.y;
            pointLight.m_worldPosition.phi = worldPosition.z;
            pointLight.enableEntity();
        }
        //==============================================================================================================


        //==============================================================================================================
        // Make a triangle to test UI render system.
        // Create Point Lights
        std::vector<Ae2DModel::Vertex2D> vertices{
                {{0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                {{0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
                {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                {{-0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                {{0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
        };

        aeImage = AeImage::createModelFromFile(m_aeDevice,"assets_NOTPRODUCTION/ui_textures/TEMP_statue.jpg");
        std::shared_ptr<Ae2DModel> ae2DModel = Ae2DModel::createModelFromFile(m_aeDevice, vertices);
        // ECS version of the floor
        auto triangle = TwoDEntity(m_aeECS,m_gameComponents);
        triangle.m_model.m_texture = aeImage;
        triangle.m_model.m_sampler = m_aeSamplers.getDefaultSampler();
        triangle.m_model.m_2d_model = ae2DModel;
        triangle.m_model.translation.x = 0.75f;
        triangle.m_model.translation.y = -0.75f;
        triangle.m_model.scale = {0.25f, 0.25f};
        triangle.m_model.rotation = 0.0 * glm::two_pi<float>();
        triangle.enableEntity();
        //==============================================================================================================
    }
}  // namespace ae