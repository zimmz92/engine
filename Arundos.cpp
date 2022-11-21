#include "Arundos.hpp"
#include "ae_rs_simple.hpp"
#include "ae_rs_point_light.hpp"
#include "ae_camera.hpp"
#include "keyboard_movement_controller.hpp"
#include "ae_buffer.hpp"

// ECS
#include "ae_component.hpp"
#include "ae_component_manager.hpp"
#include "ae_entity.hpp"
#include <string>
#include <iostream>

// libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
#include <array>
#include <cassert>

namespace ae {

    Arundos::Arundos() {
        m_globalPool = AeDescriptorPool::Builder(m_aeDevice)
            .setMaxSets(AeSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, AeSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        loadGameObjects();
    }

    Arundos::~Arundos() {
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
        AeCamera camera{};

        auto viewerObject = AeGameObject::createGameObject();
        viewerObject.m_transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

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

        AeComponent<testComponentStruct> liveTestComponentStruct;

        AeComponentManager TestComponentManager;

        class TestEntityClassA : public AeEntity<TestEntityClassA>{
            public:
                using AeEntity<TestEntityClassA>::AeEntity;
                ~TestEntityClassA() {};
        };

        class TestEntityClassB : public AeEntity<TestEntityClassB> {
        public:
            using AeEntity<TestEntityClassB>::AeEntity;
            ~TestEntityClassB() {};
        };

        TestEntityClassA TestEntityA(TestComponentManager);
        TestEntityClassB TestEntityB(TestComponentManager);
        
        liveTestComponentStruct.updateData(TestEntityA.getEntityId(), { 1,2,3,4,5 });
        liveTestComponentStruct.updateData(TestEntityB.getEntityId(), { 6,7,8,9,10 });

        struct testComponentStruct readBackA = liveTestComponentStruct.getData(TestEntityA.getEntityId());
        struct testComponentStruct readBackB = liveTestComponentStruct.getData(TestEntityB.getEntityId());

        std::string readBackStringA = "Type ID of entity class A = " + std::to_string(TestEntityA.getEntityTypeId());
        std::cout << readBackStringA;
        readBackStringA = "Value of readBack mass entity A = " + std::to_string(readBackA.mass);
        std::cout << readBackStringA;

        std::string readBackStringB = "Type ID of entity class B = " + std::to_string(TestEntityB.getEntityTypeId());
        std::cout << readBackStringB;
        readBackStringB = "Value of readBack mass entity B = " + std::to_string(readBackB.mass);
        std::cout << readBackStringB;


        //================================================================
        // END Test ECS a little bit
        //================================================================

        
        while (!m_aeWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ(m_aeWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.m_transform.translation, viewerObject.m_transform.rotation);

            float aspect = m_aeRenderer.getAspectRatio();

            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

            if (auto commandBuffer = m_aeRenderer.beginFrame()) {
                int frameIndex = m_aeRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    m_gameObjects
                };

                // update
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
                pointLightSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                m_aeRenderer.beginSwapChainRenderPass(commandBuffer);

                // order here matters for transparency
                simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);

                m_aeRenderer.endSwapChainRenderPass(commandBuffer);
                m_aeRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(m_aeDevice.device());
    }

    void Arundos::loadGameObjects() {
        std::shared_ptr<AeModel> aeModel = AeModel::createModelFromFile(m_aeDevice, "models/flat_vase.obj");

        auto flatVase = AeGameObject::createGameObject();
        flatVase.m_model = aeModel;
        flatVase.m_transform.translation = { -0.5f, 0.5f, 0.0f };
        flatVase.m_transform.scale = {3.0f, 1.5f, 3.0f };
        m_gameObjects.emplace(flatVase.getID(), std::move(flatVase));


        aeModel = AeModel::createModelFromFile(m_aeDevice, "models/smooth_vase.obj");
        auto smoothVase = AeGameObject::createGameObject();
        smoothVase.m_model = aeModel;
        smoothVase.m_transform.translation = { 0.5f, 0.5f, 0.0f };
        smoothVase.m_transform.scale = { 3.0f, 1.5f, 3.0f };
        m_gameObjects.emplace(smoothVase.getID(), std::move(smoothVase));

        aeModel = AeModel::createModelFromFile(m_aeDevice, "models/quad.obj");
        auto floor = AeGameObject::createGameObject();
        floor.m_model = aeModel;
        floor.m_transform.translation = { 0.0f, 0.5f, 0.0f };
        floor.m_transform.scale = { 3.0f, 1.0f, 3.0f };
        m_gameObjects.emplace(floor.getID(), std::move(floor));

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