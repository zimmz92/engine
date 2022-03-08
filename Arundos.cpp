#include "Arundos.hpp"

// libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace ae {

    struct SimplePushConstantData {
        glm::mat2 transform{ 1.0f };
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    Arundos::Arundos() {
        loadGameObjects();
        createPipelineLayout();
        createPipeline();
    }

    Arundos::~Arundos() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    }

    void Arundos::run() {
        while (!m_aeWindow.shouldClose()) {
            glfwPollEvents();
            
            if (auto commandBuffer = m_aeRenderer.beginFrame()) {
                m_aeRenderer.beginSwapChainRenderPass(commandBuffer);
                renderGameObjects(commandBuffer);
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

    void Arundos::createPipelineLayout() {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Send a small amount of data to shader program
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

    };

    void Arundos::createPipeline() {
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

        PipelineConfigInfo pipelineConfig{};
        AePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = m_aeRenderer.getSwapChainRenderPass();
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_aePipeline = std::make_unique<AePipeline>(
            m_aeDevice,
            "engines/graphics/shaders/vert.spv",
            "engines/graphics/shaders/frag.spv",
            pipelineConfig);
    };

    void Arundos::renderGameObjects(VkCommandBuffer t_commandBuffer) {
        m_aePipeline->bind(t_commandBuffer);

        for (auto& obj : m_gameObjects) {
            SimplePushConstantData push{};
            push.offset = obj.m_transform2d.translation;
            push.color = obj.m_color;
            push.transform = obj.m_transform2d.mat2();

            vkCmdPushConstants(t_commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

            obj.m_model->bind(t_commandBuffer);
            obj.m_model->draw(t_commandBuffer);
        }
    }
}  // namespace ae