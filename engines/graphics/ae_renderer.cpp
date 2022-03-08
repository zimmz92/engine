#include "ae_renderer.hpp"

#include <stdexcept>
#include <array>

namespace ae {

    AeRenderer::AeRenderer(AeWindow& t_window, AeDevice& t_device) : m_aeWindow{ t_window }, m_aeDevice{ t_device } {
        recreateSwapChain();
        createCommandBuffers();
    }

    AeRenderer::~AeRenderer() { freeCommandBuffers(); }

    void AeRenderer::recreateSwapChain() {
        auto extent = m_aeWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = m_aeWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_aeDevice.device());

        if (m_aeSwapChain == nullptr) {
            m_aeSwapChain = std::make_unique<AeSwapChain>(m_aeDevice, extent);
        }
        else {
            m_aeSwapChain = std::make_unique<AeSwapChain>(m_aeDevice, extent, std::move(m_aeSwapChain));
            if (m_aeSwapChain->imageCount() != m_commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        // TODO: If render pass compatible remove following line
    };

    void AeRenderer::createCommandBuffers() {
        m_commandBuffers.resize(m_aeSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_aeDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        if (vkAllocateCommandBuffers(m_aeDevice.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    };

    void AeRenderer::freeCommandBuffers() {
        vkFreeCommandBuffers(m_aeDevice.device(), m_aeDevice.getCommandPool(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
        m_commandBuffers.clear();
    }

    VkCommandBuffer AeRenderer::beginFrame() {
        assert(!m_isFrameStarted && "Cannot call begin frame when a frame is in progress!");

        auto result = m_aeSwapChain->acquireNextImage(&m_currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to aquire swap chain image!");
        }

        m_isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        return commandBuffer;
    }


    void AeRenderer::endFrame() {
        assert(m_isFrameStarted && "Cannot call endFrame while a frame is not in progress!");
        auto commandBuffer = getCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer.");
        }

        auto result = m_aeSwapChain->submitCommandBuffers(&commandBuffer, &m_currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_aeWindow.wasWindowResized()) {
            m_aeWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image!");
        }

        m_isFrameStarted = false;
    }


    void AeRenderer::beginSwapChainRenderPass(VkCommandBuffer t_commandBuffer) {
        assert(m_isFrameStarted && "Cannot call beginSwapChainRenderPass while a frame is not in progress!");
        assert(t_commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on a command buffer from a different frame!");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_aeSwapChain->getRenderPass();
        renderPassInfo.framebuffer = m_aeSwapChain->getFrameBuffer(m_currentImageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_aeSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(t_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_aeSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(m_aeSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, m_aeSwapChain->getSwapChainExtent() };
        vkCmdSetViewport(t_commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(t_commandBuffer, 0, 1, &scissor);
    }

    void AeRenderer::endSwapChainRenderPass(VkCommandBuffer t_commandBuffer) {
        assert(m_isFrameStarted && "Cannot call endSwapChainRenderPass while a frame is not in progress!");
        assert(t_commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on a command buffer from a different frame!");

        vkCmdEndRenderPass(t_commandBuffer);
    }

}  // namespace ae