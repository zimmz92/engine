#include "Arundos.hpp"
#include "ae_rs_simple.hpp"
#include "ae_camera.hpp"
#include "keyboard_movement_controller.hpp"

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
        loadGameObjects();
    }

    Arundos::~Arundos() {
    }

    void Arundos::run() {
        AeRsSimple simpleRenderSystem(m_aeDevice, m_aeRenderer.getSwapChainRenderPass());
        AeCamera camera{};

        auto viewerObject = AeGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        
        
        while (!m_aeWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ(m_aeWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.m_transform.translation, viewerObject.m_transform.rotation);

            float aspect = m_aeRenderer.getAspectRatio();

            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

            if (auto commandBuffer = m_aeRenderer.beginFrame()) {
                m_aeRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, m_gameObjects, camera);
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
        flatVase.m_transform.translation = { -0.5f, 0.5f, 2.5f };
        flatVase.m_transform.scale = {3.0f, 1.5f, 3.0f };
        m_gameObjects.push_back(std::move(flatVase));


        aeModel = AeModel::createModelFromFile(m_aeDevice, "models/smooth_vase.obj");

        auto smoothVase = AeGameObject::createGameObject();
        smoothVase.m_model = aeModel;
        smoothVase.m_transform.translation = { 0.5f, 0.5f, 2.5f };
        smoothVase.m_transform.scale = { 3.0f, 1.5f, 3.0f };
        m_gameObjects.push_back(std::move(smoothVase));
    }
}  // namespace ae