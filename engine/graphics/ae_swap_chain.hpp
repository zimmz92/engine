#pragma once

#include "ae_engine_constants.hpp"
#include "ae_device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace ae {

    class AeSwapChain {
    public:
        /// Create the swap chain.
        /// \param t_deviceRef Reference to the graphics device the swap chain will be created for.
        /// \param t_windowExtent The window the swap chain will be interfacing with.
        AeSwapChain(AeDevice& t_deviceRef, VkExtent2D t_windowExtent);

        /// Create a new swap chain from a previous swap chain, this must be done whenever the window size is changed.
        /// \param t_deviceRef Reference to the graphics device the swap chain will be created for.
        /// \param t_windowExtent The window the swap chain will be interfacing with.
        /// \param t_previous The previous swap chain the new swap chain shall use a reference during it's creation.
        AeSwapChain(AeDevice& t_deviceRef, VkExtent2D t_windowExtent, std::shared_ptr<AeSwapChain> t_previous);

        /// Function that destroys a swap chain object.
        ~AeSwapChain();

        /// Do not allow this class to be copied (2 lines below).
        AeSwapChain(const AeSwapChain&) = delete;
        AeSwapChain& operator=(const AeSwapChain&) = delete;

        /// Get the specified frame buffer.
        /// \param t_index The index to which frame buffer should be returned.
        /// \return The frame buffer at the specified index.
        VkFramebuffer getFrameBuffer(int t_index) { return m_swapChainFramebuffers[t_index]; }

        /// Get the current render pass.
        /// \return The current render pass.
        VkRenderPass getRenderPass() { return m_renderPass; }

        /// Get the image view at the specified index.
        /// \param t_index The index of which image view should be returned.
        /// \return The image view at the specified index.
        VkImageView getImageView(int t_index) { return m_swapChainImageViews[t_index]; }

        /// Get the number of images in the swap chain.
        /// \return The number of images in the swap chain.
        size_t imageCount() { return m_swapChainImages.size(); }

        /// Get the image format of the swap chain.
        /// \return The image format of the swap chain.
        VkFormat getSwapChainImageFormat() { return m_swapChainImageFormat; }

        /// Get the extent of the swap chain.
        /// \return The 2D extent of the swap chain.
        VkExtent2D getSwapChainExtent() { return m_swapChainExtent; }

        /// Get the width of the swap chain extent.
        /// \return The width of the swap chain extent.
        uint32_t width() { return m_swapChainExtent.width; }

        /// Get the height of the swap chain extent.
        /// \return The height of the swap chain extent.
        uint32_t height() { return m_swapChainExtent.height; }

        /// Get the aspect ratio of the swap chain extent.
        /// \return The aspect ratio of the swap chain extent as a float.
        float extentAspectRatio() {
            return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
        }

        /// Get the depth format of the swap chain.
        /// \return The depth format of the swap chain.
        VkFormat findDepthFormat();

        /// Acquire the next image from the swap chain by setting the input pointer to the index of the next image.
        /// \param t_imageIndex Pointer to be populated with the index of the next image of the swap chain.
        /// \return VK_SUCCESS if the provided index pointer was successfully set.
        VkResult acquireNextImage(uint32_t* t_imageIndex);

        /// Submit command buffers to the swap chain to create the image at the specified index.
        /// \param t_graphicsBuffer The command buffers which provide the data for creating the image.
        /// \param t_imageIndex The index of the image the command buffers are providing data to create.
        /// \return VK_SUCCESS if the command buffers were submitted successfully.
        VkResult submitCommandBuffers(const VkCommandBuffer* t_graphicsBuffer, const VkCommandBuffer* t_computeBuffer, const uint32_t* t_imageIndex);

        /// Compare the specified swap chain format to this swap chain's format.
        /// \param t_swapchain The swap chain who's format is to be compared to this swap chain's format.
        /// \return True if the swap chain formats are identical.
        bool compareSwapFormats(const AeSwapChain &t_swapchain) const {
            return t_swapchain.m_swapChainDepthFormat == m_swapChainDepthFormat && 
                   t_swapchain.m_swapChainImageFormat == m_swapChainImageFormat;
        }

    private:

        /// Creates and initializes the swap chain.
        void init();

        /// Creates the base swap chain object.
        void createSwapChain();

        /// Creates image views for a swap chain.
        void createImageViews();

        /// Add depth resources to the swap chain.
        void createDepthResources();

        /// Create the swap chain's render pass.
        void createRenderPass();

        /// Create frame buffers for the swap chain.
        void createFramebuffers();

        /// Create objects to synchronize the swap chain image views with what has been rendered.
        void createSyncObjects();

        /// Choose a swap surface format from the provided list of available formats which best suite this application.
        /// \param t_availableFormats The swap surface formats available to choose from.
        /// \return The swap surface format, VkSurfaceFormatKHR, most suitable for this application from the list of
        /// available formats.
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& t_availableFormats);

        /// Select how images shall synchronize with screen refreshing from the available present modes.\
        /// \param t_availablePresentModes The presentation formats available to choose from.
        /// \return The presentation format, VkPresentModeKHR, most suitable for this application from the list of
        /// available formats.
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& t_availablePresentModes);

        /// Function to set the image extent based on the size of the given surface capabilities.
        /// \param t_capabilities The surface capabilities that the swap chain must use to choose a suitable image
        /// extent from.
        /// \return The image extent, VkExtent2D, best suited for this application.
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& t_capabilities);

        // Member variables
        /// The image format of the swap chain.
        VkFormat m_swapChainImageFormat;

        /// The depth format of the swap chain.
        VkFormat m_swapChainDepthFormat;

        /// The image extent of the swap chain.
        VkExtent2D m_swapChainExtent;

        /// The swap chain frame buffers.
        std::vector<VkFramebuffer> m_swapChainFramebuffers;

        /// The swap chain render pass.
        VkRenderPass m_renderPass;

        /// The depth images of the swap chain.
        std::vector<VkImage> m_depthImages;

        /// The memory for the depth images.
        std::vector<VkDeviceMemory> m_depthImageMemories;

        /// The view of the depth images.
        std::vector<VkImageView> m_depthImageViews;

        /// The images in the swap chain.
        std::vector<VkImage> m_swapChainImages;

        /// The views of the images in the swap chain.
        std::vector<VkImageView> m_swapChainImageViews;

        /// A reference to the device this swap chain is executed on.
        AeDevice& m_device;

        /// The window extent of the GLFW window this swap chain interfaces with.
        VkExtent2D m_windowExtent;

        /// The swap chain vulkan interface.
        VkSwapchainKHR m_swapChain;

        /// A pointer to the old swap chain this swap chain creation is based on. This will be retired (not deleted)
        /// when this swap chain is created.
        std::shared_ptr<AeSwapChain> m_oldSwapChain;

        /// A synchronization primitive used to ensure an image is ready before rendering to it.
        std::vector<VkSemaphore> m_imageAvailableSemaphores;

        /// A synchronization primitive used to ensure a render has finished before presenting it.
        std::vector<VkSemaphore> m_renderFinishedSemaphores;

        /// A synchronization primitive used to ensure the compute queue has finished before submitting the graphics
        /// queue.
        std::vector<VkSemaphore> m_computeFinishedSemaphores;

        /// Fence that tracks that the command buffer corresponding to a frame has finished execution.
        std::vector<VkFence> m_commandBufferInFlightFences;

        /// Fence that tracks that the command buffers corresponding to an image have finished.
        std::vector<VkFence> m_imagesInFlightFences;

        /// Fence that tracks that the command buffers compute commands have finished.
        std::vector<VkFence> m_computeInFlightFences;

        /// Track which of the frames is the one that currently should be used for rendering.
        size_t m_currentFrame = 0;
    };

}  // namespace ae