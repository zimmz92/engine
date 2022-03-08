#include "ae_rs_simple.hpp"

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

    AeRsSimple::AeRsSimple(AeDevice& t_device, VkRenderPass t_renderPass) : m_aeDevice{ t_device } {
        createPipelineLayout();
        createPipeline(t_renderPass);
    }

    AeRsSimple::~AeRsSimple() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    }

    void AeRsSimple::createPipelineLayout() {

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

    void AeRsSimple::createPipeline(VkRenderPass t_renderPass) {
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

        PipelineConfigInfo pipelineConfig{};
        AePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = t_renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_aePipeline = std::make_unique<AePipeline>(
            m_aeDevice,
            "engines/graphics/shaders/vert.spv",
            "engines/graphics/shaders/frag.spv",
            pipelineConfig);
    };

    void AeRsSimple::renderGameObjects(VkCommandBuffer t_commandBuffer, std::vector<AeGameObject> &t_gameObjects) {
        m_aePipeline->bind(t_commandBuffer);

        for (auto& obj : t_gameObjects) {
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