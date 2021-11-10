#include "Arundos.hpp"

#include <stdexcept>

namespace ae {

    Arundos::Arundos() {
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    Arundos::~Arundos() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    }

    void Arundos::run() {
        while (!m_aeWindow.shouldClose()) {
            glfwPollEvents();
        }
    }

    void Arundos::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

    };
    void Arundos::createPipeline() {
        auto pipelineConfig = AePipeline::defaultPipelineConfigInfo(m_aeSwapChain.width(), m_aeSwapChain.height());
        pipelineConfig.renderPass = m_aeSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_aePipeline = std::make_unique<AePipeline>(
            m_aeDevice,
            "engines/graphics/shaders/vert.spv",
            "engines/graphics/shaders/frag.spv",
            pipelineConfig);
    };

    void Arundos::createCommandBuffers() {
    };

    void Arundos::drawFrame() {
    };

}  // namespace ae