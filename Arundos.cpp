#include "Arundos.hpp"
#include "ae_buffer.hpp"

#include "game_object_entity.hpp"
#include "camera_entity.hpp"
#include "point_light_entity.hpp"

// libraries
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace ae {

    Arundos::Arundos() {
        loadGameObjects();
    };

    Arundos::~Arundos() {};

    void Arundos::run() {

        /*
        //==============================================================================================================
        // Make the game camera using ECS
        // TODO: In ECS need to make entity manager keep a shared pointer to an entity (or malloc one) to keep it from
        //  being destroyed when scope changes. Should need to specify that an entity be destroyed if need be.
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
        std::shared_ptr<AeModel> aeModel = AeModel::createModelFromFile(m_aeDevice, "models/flat_vase.obj");
        // ECS version of flatVase
        auto testFlatVase = GameObjectEntity(m_aeECS,m_gameComponents);
        testFlatVase.m_worldPosition = {-0.5f, 0.5f, 0.0f };
        testFlatVase.m_model.m_model = aeModel;
        testFlatVase.m_model.scale = {3.0f, 1.5f, 3.0f };
        testFlatVase.enableEntity();


        // Load the smooth vase object model from the file
        aeModel = AeModel::createModelFromFile(m_aeDevice, "models/smooth_vase.obj");
        // ECS version of smoothVase
        auto testSmoothVase = GameObjectEntity(m_aeECS,m_gameComponents);
        testSmoothVase.m_worldPosition = {0.5f, 0.5f, 0.0f };
        testSmoothVase.m_model.m_model = aeModel;
        testSmoothVase.m_model.scale = {3.0f, 1.5f, 3.0f };
        testSmoothVase.enableEntity();

        // Load the floor object model from the file
        aeModel = AeModel::createModelFromFile(m_aeDevice, "models/quad.obj");
        // ECS version of the floor
        auto testFloor = GameObjectEntity(m_aeECS,m_gameComponents);
        testFloor.m_worldPosition = {0.0f, 0.5f, 0.0f };
        testFloor.m_model.m_model = aeModel;
        testFloor.m_model.scale = {3.0f, 1.0f, 3.0f };
        testFloor.enableEntity();


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

        std::vector<PointLightEntity*> testPointLights;
        for (int i = 0; i < lightColors.size(); i++) {
            testPointLights.push_back(new PointLightEntity(m_aeECS,m_gameComponents));

            auto pointLight = testPointLights.back();
            pointLight->m_uboDataFlags.hasUboPointLightData = true;
            pointLight->m_pointLightData.lightIntensity = 0.2f;
            pointLight->m_pointLightData.m_color = lightColors[i];
            auto rotateLight = glm::rotate(
                    glm::mat4(1.0f),
                    (i * glm::two_pi<float>()) / lightColors.size(),
                    {0.0f, -1.0f, 0.0f});
            glm::vec3 worldPosition = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
            pointLight->m_worldPosition.rho = worldPosition.x;
            pointLight->m_worldPosition.theta = worldPosition.y;
            pointLight->m_worldPosition.phi = worldPosition.z;
            pointLight->enableEntity();
        }
        //==============================================================================================================
         */

        
        while (!m_aeWindow.shouldClose()) {
            // Check to see if there are any user input events.
            glfwPollEvents();

            // TODO: Update the components so that direction is with the entities world position data. Knowing which way
            //  an entity is facing in the world is a requirement!
            m_aeECS.runSystems();

            // TODO allow for option to limit frame timing, aka lock FPS, if desired but allow other systems to continue to run
            //time_delta = glm::min(time_delta, MAX_FRAME_TIME);
        }

        // Destroy the test ECS point lights
        //for (auto pointLight : testPointLights) {
        //    delete pointLight;
        //}


        vkDeviceWaitIdle(m_aeDevice.device());
    }

    void Arundos::loadGameObjects() {

        //==============================================================================================================
        // Make the game camera using ECS
        // TODO: In ECS need to make entity manager keep a shared pointer to an entity (or malloc one) to keep it from
        //  being destroyed when scope changes. Should need to specify that an entity be destroyed if need be.
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
        std::shared_ptr<AeModel> aeModel = AeModel::createModelFromFile(m_aeDevice, "models/flat_vase.obj");
        // ECS version of flatVase
        auto testFlatVase = GameObjectEntity(m_aeECS,m_gameComponents);
        testFlatVase.m_worldPosition = {-0.5f, 0.5f, 0.0f };
        testFlatVase.m_model.m_model = aeModel;
        testFlatVase.m_model.scale = {3.0f, 1.5f, 3.0f };
        testFlatVase.enableEntity();


        // Load the smooth vase object model from the file
        aeModel = AeModel::createModelFromFile(m_aeDevice, "models/smooth_vase.obj");
        // ECS version of smoothVase
        auto testSmoothVase = GameObjectEntity(m_aeECS,m_gameComponents);
        testSmoothVase.m_worldPosition = {0.5f, 0.5f, 0.0f };
        testSmoothVase.m_model.m_model = aeModel;
        testSmoothVase.m_model.scale = {3.0f, 1.5f, 3.0f };
        testSmoothVase.enableEntity();

        // Load the floor object model from the file
        aeModel = AeModel::createModelFromFile(m_aeDevice, "models/quad.obj");
        // ECS version of the floor
        auto testFloor = GameObjectEntity(m_aeECS,m_gameComponents);
        testFloor.m_worldPosition = {0.0f, 0.5f, 0.0f };
        testFloor.m_model.m_model = aeModel;
        testFloor.m_model.scale = {3.0f, 1.0f, 3.0f };
        testFloor.enableEntity();


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
    }
}  // namespace ae