#include "Arundos.hpp"
#include "ae_rs_simple.hpp"

// libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace ae {

    Arundos::Arundos() {
        loadGameObjects();
    }

    Arundos::~Arundos() {
    }

    void Arundos::run() {
        AeRsSimple simpleRenderSystem(m_aeDevice, m_aeRenderer.getSwapChainRenderPass());

        while (!m_aeWindow.shouldClose()) {
            glfwPollEvents();
            
            if (auto commandBuffer = m_aeRenderer.beginFrame()) {
                m_aeRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, m_gameObjects);
                m_aeRenderer.endSwapChainRenderPass(commandBuffer);
                m_aeRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(m_aeDevice.device());
    }

    void Arundos::loadGameObjects() {
        std::vector<AeModel::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };
        auto aeModel = std::make_shared<AeModel>(m_aeDevice, vertices);

        auto triangle = AeGameObject::createGameObject();
        triangle.m_model = aeModel;
        triangle.m_color = { 0.1f, 0.8f, 0.1f };
        triangle.m_transform2d.translation.x = 0.2f;
        triangle.m_transform2d.scale = {2.0f, 0.5f};
        triangle.m_transform2d.rotation = 0.25f * glm::two_pi<float>(); // vulkan has -x is left on screen -y is up on screen

        m_gameObjects.push_back(std::move(triangle));
    }
}  // namespace ae