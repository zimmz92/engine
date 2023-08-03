#include "Arundos.hpp"
#include "ae_rs_simple.hpp"
#include "ae_rs_point_light.hpp"
#include "ae_buffer.hpp"

#include "game_object_entity.hpp"
#include "camera_entity.hpp"
#include "point_light_entity.hpp"

// libraries
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <chrono>
#include <array>

namespace ae {

    Arundos::Arundos() {
        m_globalPool = AeDescriptorPool::Builder(m_aeDevice)
            .setMaxSets(AeSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, AeSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        m_gameSystems = new GameSystems(&m_gameComponents, m_aeWindow.getGLFWwindow(), &m_aeRenderer);

        loadGameObjects();
    }

    Arundos::~Arundos() {
        delete m_gameSystems;
        m_gameSystems = nullptr;
    }

    void Arundos::run() {
        std::vector<std::unique_ptr<AeBuffer>> uboBuffers(AeSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<AeBuffer>(
                m_aeDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }


        // TODO: Implement master render system that handles sub-render systems like skybox and models etc.
        auto globalSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(AeSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            AeDescriptorWriter(*globalSetLayout, *m_globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        AeRsSimple simpleRenderSystem(m_aeDevice, m_aeRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        AeRsPointLight pointLightSystem(m_aeDevice, m_aeRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());

        //==============================================================================================================
        // Make the game camera using ECS
        // TODO: In ECS need to make entity manager keep a shared pointer to an entity (or malloc one) to keep it from
        //  being destroyed when scope changes. Should need to specify that an entity be destroyed if need be.
        CameraEntity cameraECS{&m_gameComponents};
        cameraECS.m_playerControlledData->isCurrentlyControlled = true;
        cameraECS.m_worldPosition->phi = -2.5f;
        cameraECS.m_cameraData->usePerspectiveProjection = true;
        cameraECS.m_cameraData->isMainCamera = true;
        cameraECS.enableEntity();
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
            testPointLights.push_back(new PointLightEntity(&m_gameComponents));

            auto pointLight = testPointLights.back();
            pointLight->m_pointLightData->lightIntensity = 0.2f;
            pointLight->m_pointLightData->m_color = lightColors[i];
            auto rotateLight = glm::rotate(
                    glm::mat4(1.0f),
                    (i * glm::two_pi<float>()) / lightColors.size(),
                    {0.0f, -1.0f, 0.0f});
            glm::vec3 worldPosition = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
            pointLight->m_worldPosition->rho = worldPosition.x;
            pointLight->m_worldPosition->theta = worldPosition.y;
            pointLight->m_worldPosition->phi = worldPosition.z;
            pointLight->enableEntity();
        }
        //==============================================================================================================



        auto currentTime = std::chrono::high_resolution_clock::now();
        
        while (!m_aeWindow.shouldClose()) {
            // Check to see if there are any user input events.
            glfwPollEvents();

            ae_ecs::ecsSystemManager.runSystems();

            // TODO allow for option to limit frame timing, aka lock FPS, if desired but allow other systems to continue to run
            //time_delta = glm::min(time_delta, MAX_FRAME_TIME);


            if (auto commandBuffer = m_aeRenderer.beginFrame()) {

                // update
                int frameIndex = m_aeRenderer.getFrameIndex();
                FrameInfo frameInfo{
                        frameIndex,
                        m_gameSystems->timingSystem.getDt(),
                        commandBuffer,
                        cameraECS.getEntityId(),
                        globalDescriptorSets[frameIndex],
                        m_gameObjects
                };
                GlobalUbo ubo{};
                ubo.projection = cameraECS.m_cameraData->m_projectionMatrix;
                ubo.view = cameraECS.m_cameraData->m_viewMatrix;
                ubo.inverseView = cameraECS.m_cameraData->m_inverseViewMatrix;
                pointLightSystem.update(frameInfo, ubo);

                // render
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
                m_aeRenderer.beginSwapChainRenderPass(commandBuffer);

                // order here matters for transparency
                simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo,&m_gameComponents);

                m_aeRenderer.endSwapChainRenderPass(commandBuffer);
                m_aeRenderer.endFrame();
            }
        }

        // Destroy the test ECS point lights
        for (auto pointLight : testPointLights) {
            delete pointLight;
        }


        vkDeviceWaitIdle(m_aeDevice.device());
    }

    void Arundos::loadGameObjects() {
        // Load the flat vase object model from the file
        std::shared_ptr<AeModel> aeModel = AeModel::createModelFromFile(m_aeDevice, "models/flat_vase.obj");

        // Non-ECS version of putting the flat vase into the game
        // TODO Remove once a system is implemented for gathering entities with the model component that are in a scene
        auto flatVase = AeGameObject::createGameObject();
        flatVase.m_model = aeModel;
        flatVase.m_transform.translation = { -0.5f, 0.5f, 0.0f };
        flatVase.m_transform.scale = {3.0f, 1.5f, 3.0f };
        m_gameObjects.emplace(flatVase.getID(), std::move(flatVase));

        // ECS version of flatVase
        auto testFlatVase = GameObjectEntity(&m_gameComponents);
        *testFlatVase.m_worldPosition = {-0.5f, 0.5f, 0.0f };
        testFlatVase.m_model->m_model = aeModel;
        testFlatVase.m_model->scale = {3.0f, 1.5f, 3.0f };
        testFlatVase.enableEntity();


        // Non-ECS version of putting the smooth vase into the game
        // TODO Remove once a system is implemented for gathering entities with the model component that are in a scene
        aeModel = AeModel::createModelFromFile(m_aeDevice, "models/smooth_vase.obj");
        auto smoothVase = AeGameObject::createGameObject();
        smoothVase.m_model = aeModel;
        smoothVase.m_transform.translation = { 0.5f, 0.5f, 0.0f };
        smoothVase.m_transform.scale = { 3.0f, 1.5f, 3.0f };
        m_gameObjects.emplace(smoothVase.getID(), std::move(smoothVase));

        // ECS version of smoothVase
        auto testSmoothVase = GameObjectEntity(&m_gameComponents);
        *testSmoothVase.m_worldPosition = {0.5f, 0.5f, 0.0f };
        testSmoothVase.m_model->m_model = aeModel;
        testSmoothVase.m_model->scale = {3.0f, 1.5f, 3.0f };
        testSmoothVase.enableEntity();

        // Non-ECS version of putting the floor plane into the game
        // TODO Remove once a system is implemented for gathering entities with the model component that are in a scene
        aeModel = AeModel::createModelFromFile(m_aeDevice, "models/quad.obj");
        auto floor = AeGameObject::createGameObject();
        floor.m_model = aeModel;
        floor.m_transform.translation = { 0.0f, 0.5f, 0.0f };
        floor.m_transform.scale = { 3.0f, 1.0f, 3.0f };
        m_gameObjects.emplace(floor.getID(), std::move(floor));

        // ECS version of the floor
        auto testFloor = GameObjectEntity(&m_gameComponents);
        *testFloor.m_worldPosition = {0.0f, 0.5f, 0.0f };
        testFloor.m_model->m_model = aeModel;
        testFloor.m_model->scale = {3.0f, 1.0f, 3.0f };
        testFloor.enableEntity();


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
            auto pointLight = AeGameObject::makePointLight(0.2f);
            pointLight.m_color = lightColors[i];
            auto rotateLight = glm::rotate(
                glm::mat4(1.0f), 
                (i * glm::two_pi<float>()) / lightColors.size(), 
                {0.0f, -1.0f, 0.0f});
            pointLight.m_transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
            m_gameObjects.emplace(pointLight.getID(), std::move(pointLight));
        }
    }
}  // namespace ae