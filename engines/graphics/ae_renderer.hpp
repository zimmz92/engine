#pragma once

#include "ae_graphics_constants.hpp"
#include "ae_device.hpp"
#include "ae_swap_chain.hpp"
#include "ae_window.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace ae {

    /// Defines the renderer class of the Arundos engine.
    class AeRenderer {
    public:

        /// Constructs a new renderer.
        /// \param t_window The window that the renderer will be rendering images for.
        /// \param t_device The graphics device that the renderer will be using to render images.
        AeRenderer(AeWindow& t_window, AeDevice& t_device);

        /// Renderer destructor.
        ~AeRenderer();

        /// Do not allow this class to be copied (2 lines below)
        AeRenderer(const AeRenderer&) = delete;
        AeRenderer& operator=(const AeRenderer&) = delete;

        /// Get the current render pass of the swap chain.
        /// \return The current render pass of the swap chain.
        VkRenderPass getSwapChainRenderPass() const { return m_aeSwapChain->getRenderPass(); }

        /// Get the aspect ratio currently being used by the swap chain.
        /// \return A float representing the current aspect ratio used by the swap chain.
        float getAspectRatio() const { return m_aeSwapChain->extentAspectRatio(); }

        /// Check if a frame is currently being rendered.
        /// \return True if a frame is currently being rendered.
        bool isFrameInProgress() const { return m_isFrameStarted; };

        /// If a frame is not current in progress will return the command buffer for the current frame.
        /// \return Command buffer for the current frame.
        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress");
            return m_commandBuffers[m_currentFrameIndex];
        }

        /// Retrieve the frame index of the frame currently being rendered.
        /// \return The frame index of the frame currently being rendered as an integer.
        int getFrameIndex() const {
            assert(m_isFrameStarted && "Cannot get frame index when a frame not in progress");
            return m_currentFrameIndex;
        }

        /// Begins rendering a new frame.
        /// \return Returns the command buffer to record commands to for rendering the new frame.
        VkCommandBuffer beginFrame();

        /// Executes the command buffer, rendering the new frame as an image.
        void endFrame();

        /// Starts the swap chain render pass using the supplied command buffer.
        /// \note This allows the command buffer to be recorded to before actually executing the render.
        /// \param t_commandBuffer The command buffer the swap chain will be getting it's commands from to create the
        /// image for the current frame being rendered.
        void beginSwapChainRenderPass(VkCommandBuffer t_commandBuffer);

        /// Executes the render using the supplied command buffer then ends the swap chain render pass.
        /// \param t_commandBuffer The command buffer the swap chain will be getting it's commands from to create the
        /// image for the current frame being rendered.
        void endSwapChainRenderPass(VkCommandBuffer t_commandBuffer);


    private:

        /// Create the command buffers for this renderer.
        void createCommandBuffers();

        /// Free the command buffers for this renderer.
        void freeCommandBuffers();

        /// Recreate the swap chain this renderer uses.
        void recreateSwapChain();

        /// A reference to the window that this renderer renders to.
        AeWindow& m_aeWindow;

        /// The device this renderer uses to perform it's rendering operations.
        AeDevice& m_aeDevice;

        /// The swap chain utilized by this renderer.
        std::unique_ptr<AeSwapChain> m_aeSwapChain;

        /// The command buffers this renderer utilizes and manages.
        std::vector<VkCommandBuffer> m_commandBuffers;

        /// The index of the current image being rendered.
        uint32_t m_currentImageIndex;

        /// The index of the current frame being rendered.
        int m_currentFrameIndex{ 0 };

        /// The status of the current frame. True if the frame is being rendered.
        bool m_isFrameStarted{ false };
    };
}  // namespace ae