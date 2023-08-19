#include "ae_renderer.hpp"

#include <stdexcept>
#include <array>

namespace ae {

    // Construct the renderer.
    AeRenderer::AeRenderer(AeWindow& t_window, AeDevice& t_device) : m_aeWindow{ t_window }, m_aeDevice{ t_device } {
        recreateSwapChain();
        createCommandBuffers();
    }



    // Destructor for the renderer.
    AeRenderer::~AeRenderer() {
        freeCommandBuffers();
    }



    // Recreate the swap chain.
    void AeRenderer::recreateSwapChain() {
        // Get the current window size.
        auto extent = m_aeWindow.getExtent();

        // If one of the dimensions are 0 then the application should wait. This can also occur during minimization.
        // TODO: Create a glfw "window refresh callback" to redraw the contents of the window so the application does
        //  not experience a processing block during a window resize.
        while (extent.width == 0 || extent.height == 0) {
            extent = m_aeWindow.getExtent();
            glfwWaitEvents();
        }

        // Wait until the current swap chain is being used before creating a new swap chain.
        vkDeviceWaitIdle(m_aeDevice.device());

        // Check if there is a previous swap chain the new one should use on creation.
        if (m_aeSwapChain == nullptr) {
            // Make a new swap chain.
            m_aeSwapChain = std::make_unique<AeSwapChain>(m_aeDevice, extent);
        }
        else {
            // Move the old swap chain to a new temporary pointer that will be cleaned up, thus cleaning up the old swap
            // chain, after creating the new swap chain.
            std::shared_ptr<AeSwapChain> oldSwapChain = std::move(m_aeSwapChain);

            // Make the new swap chain reusing as much of the old swap chain as possible.
            m_aeSwapChain = std::make_unique<AeSwapChain>(m_aeDevice, extent, oldSwapChain);

            // Make sure the render pass is still compatible with the new swap chain.
            if (!oldSwapChain->compareSwapFormats(*m_aeSwapChain.get())) {
                throw std::runtime_error("Swap chain image (or depth) format has changed!");
                // TODO: Create a callback function that deals with a new incompatible render pass has being created
            }
        }
    };



    // Create a command buffer.
    void AeRenderer::createCommandBuffers() {

        // Allocate enough command buffers for all the frames that could be rendered.
        m_commandBuffers.resize(AeSwapChain::MAX_FRAMES_IN_FLIGHT);


        // Specify the struct for allocating a command buffer from the pool.
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_aeDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        // Allocate the command buffer from the device command buffer pool.
        if (vkAllocateCommandBuffers(m_aeDevice.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    };



    // Free a command buffer and clear it.s
    void AeRenderer::freeCommandBuffers() {

        // Free the command buffer and clear it out.s
        vkFreeCommandBuffers(m_aeDevice.device(), m_aeDevice.getCommandPool(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
        m_commandBuffers.clear();
    }



    // Start a new frame.
    VkCommandBuffer AeRenderer::beginFrame() {

        assert(!m_isFrameStarted && "Cannot call begin frame when a frame is in progress!");

        // Get the next image that will be rendered to.
        auto result = m_aeSwapChain->acquireNextImage(&m_currentImageIndex);

        // Recreate the swap chain if the screen has been resized or the surface has changed for some reason. If this
        // is the case do not allow a new frame to begin on this pass.
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        // Check to make sure that the swap chain still matches the surface properties of the device.
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }

        // The frame has started.
        m_isFrameStarted = true;

        // Get the command buffer to be used for rendering this image.
        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // Start recording to the command buffer for this image.
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        return commandBuffer;
    }



    // End a new frame.
    void AeRenderer::endFrame() {
        assert(m_isFrameStarted && "Cannot call endFrame while a frame is not in progress!");

        // Stop allowing recording to the command buffer.
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer.");
        }

        // Submit the command buffer to the swap chain.
        auto result = m_aeSwapChain->submitCommandBuffers(&commandBuffer, &m_currentImageIndex);

        // Make sure that the window was not resized, if it was we will want to re-create the swap chain before
        // rendering the image.
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_aeWindow.wasWindowResized()) {
            m_aeWindow.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image!");
        }

        // Indicate that a frame is in progress and increment the frame index for the next frame that will be rendered
        // after this one.
        m_isFrameStarted = false;
        m_currentFrameIndex = (m_currentFrameIndex + 1) % AeSwapChain::MAX_FRAMES_IN_FLIGHT;
    }



    // Start the render pass.
    void AeRenderer::beginSwapChainRenderPass(VkCommandBuffer t_commandBuffer) {
        assert(m_isFrameStarted && "Cannot call beginSwapChainRenderPass while a frame is not in progress!");
        assert(t_commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on a command buffer from a different frame!");

        // Set up the struct for this render pass with the swap chain associated with this renderer.
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_aeSwapChain->getRenderPass();
        renderPassInfo.framebuffer = m_aeSwapChain->getFrameBuffer(m_currentImageIndex);

        // Currently only rendering to the whole window.
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_aeSwapChain->getSwapChainExtent();

        // Specify the default color, to be used as the background, for the rendered image.
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        // Start the render pass.
        vkCmdBeginRenderPass(t_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Configure the dynamic viewport and scissor. This is to ensure that the correct window size for each frame is
        // submitted before the command buffer is executed even if the swap chain changes.
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



    // Ends the render pass.
    void AeRenderer::endSwapChainRenderPass(VkCommandBuffer t_commandBuffer) {
        assert(m_isFrameStarted && "Cannot call endSwapChainRenderPass while a frame is not in progress!");
        assert(t_commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on a command buffer from a different frame!");

        // End the render pass.
        vkCmdEndRenderPass(t_commandBuffer);
    }

}  // namespace ae