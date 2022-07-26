#include "ae_rs_point_light.hpp"

// libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <cassert>
#include <stdexcept>
#include <array>

namespace ae {

    AeRsPointLight::AeRsPointLight(AeDevice& t_device, VkRenderPass t_renderPass, VkDescriptorSetLayout t_globalSetLayout) : m_aeDevice{ t_device } {
        createPipelineLayout(t_globalSetLayout);
        createPipeline(t_renderPass);
    }

    AeRsPointLight::~AeRsPointLight() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    }

    void AeRsPointLight::createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout) {

        // VkPushConstantRange pushConstantRange{};
        // pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        // pushConstantRange.offset = 0;
        // pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ t_globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Send a small amount of data to shader program
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

    };

    void AeRsPointLight::createPipeline(VkRenderPass t_renderPass) {
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

        PipelineConfigInfo pipelineConfig{};
        AePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.renderPass = t_renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_aePipeline = std::make_unique<AePipeline>(
            m_aeDevice,
            "engines/graphics/shaders/point_light_vert.spv",
            "engines/graphics/shaders/point_light_frag.spv",
            pipelineConfig);
    };

    void AeRsPointLight::render(FrameInfo& t_frameInfo) {
        m_aePipeline->bind(t_frameInfo.m_commandBuffer);

        vkCmdBindDescriptorSets(
            t_frameInfo.m_commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipelineLayout,
            0,
            1,
            &t_frameInfo.m_globalDescriptorSet,
            0,
            nullptr);

        vkCmdDraw(t_frameInfo.m_commandBuffer, 6, 1, 0, 0);
    }
}  // namespace ae