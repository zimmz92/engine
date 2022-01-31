#include "Arundos.hpp"

#include <stdexcept>
#include <array>

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
            drawFrame();
        }

        vkDeviceWaitIdle(m_aeDevice.device());
    }

    void Arundos::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Send a small amount of data to shader program
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

    };
    void Arundos::createPipeline() {
        PipelineConfigInfo pipelineConfig{};
        AePipeline::defaultPipelineConfigInfo(
            pipelineConfig,
            m_aeSwapChain.width(),
            m_aeSwapChain.height());
        pipelineConfig.renderPass = m_aeSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_aePipeline = std::make_unique<AePipeline>(
            m_aeDevice,
            "engines/graphics/shaders/vert.spv",
            "engines/graphics/shaders/frag.spv",
            pipelineConfig);
    };

    void Arundos::createCommandBuffers() {
        m_commandBuffers.resize(m_aeSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_aeDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        if (vkAllocateCommandBuffers(m_aeDevice.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }

        for (int i = 0; i < m_commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_aeSwapChain.getRenderPass();
            renderPassInfo.framebuffer = m_aeSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = m_aeSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.1f, 0.1f, 0.1f, 0.1f };
            clearValues[1].depthStencil = { 1.0f, 0 };
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            m_aePipeline->bind(m_commandBuffers[i]);
            vkCmdDraw(m_commandBuffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(m_commandBuffers[i]);
            if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer.");
            }
        }
    };

    void Arundos::drawFrame() {
        uint32_t imageIndex;
        auto result = m_aeSwapChain.acquireNextImage(&imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to aquire swap chain image!");
        }

        result = m_aeSwapChain.submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image!");
        }

    };

}  // namespace ae