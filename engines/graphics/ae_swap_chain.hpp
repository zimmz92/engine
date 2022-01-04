#pragma once

#include "ae_device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>

namespace ae {

    class AeSwapChain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        // Function that creates a swap chain object
        AeSwapChain(AeDevice& t_deviceRef, VkExtent2D t_windowExtent);

        // Function that destroys a swap chain object
        ~AeSwapChain();

        // Do not allow this class to be copied (2 lines below)
        AeSwapChain(const AeSwapChain&) = delete;
        void operator=(const AeSwapChain&) = delete;

        // Function to return the specified frame bufffer
        VkFramebuffer getFrameBuffer(int t_index) { return m_swapChainFramebuffers[t_index]; }

        // Function to return the render pass
        VkRenderPass getRenderPass() { return m_renderPass; }

        // Function to return the image view
        VkImageView getImageView(int t_index) { return m_swapChainImageViews[t_index]; }

        // Funciton to return the number of images in the swap chain
        size_t imageCount() { return m_swapChainImages.size(); }

        // Funciton to return the image format of the swap chain
        VkFormat getSwapChainImageFormat() { return m_swapChainImageFormat; }

        // Function to return the extent of the swap chain
        VkExtent2D getSwapChainExtent() { return m_swapChainExtent; }

        // Function to return the width of the swap chain extent
        uint32_t width() { return m_swapChainExtent.width; }

        // Function to return the height of the swap chain extent
        uint32_t height() { return m_swapChainExtent.height; }

        // Function to return the aspect ratio of the swap chain extent
        float extentAspectRatio() {
            return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
        }

        // Function to return the depth format of the swap chain
        VkFormat findDepthFormat();

        // Function to aquire the next image from the swap chain
        VkResult acquireNextImage(uint32_t* t_imageIndex);

        // Function to submit command buffers to the swap chain
        VkResult submitCommandBuffers(const VkCommandBuffer* t_buffers, uint32_t* t_imageIndex);

    private:
        // Function to create a swap chain object
        void createSwapChain();

        // Function to create image views for a swap chain
        void createImageViews();

        // Function to add depth resources to the swap chain
        void createDepthResources();

        // Function to add render the pass tothe swap chain
        void createRenderPass();

        // Function to create the swap chain frame buffers
        void createFramebuffers();

        // Function to syncronize the swap chain image views with render finishes
        void createSyncObjects();

        // Function to choose a swap surface format based on set criterias
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& t_availableFormats);

        // Function to select the image synchornization with scree refresh
        VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& t_availablePresentModes);

        // Function to set the image extend baesd on the size of the given surface capabilities
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& t_capabilities);

        // Member variables
        VkFormat m_swapChainImageFormat;
        VkExtent2D m_swapChainExtent;

        std::vector<VkFramebuffer> m_swapChainFramebuffers;
        VkRenderPass m_renderPass;

        std::vector<VkImage> m_depthImages;
        std::vector<VkDeviceMemory> m_depthImageMemorys;
        std::vector<VkImageView> m_depthImageViews;
        std::vector<VkImage> m_swapChainImages;
        std::vector<VkImageView> m_swapChainImageViews;

        AeDevice& m_device;
        VkExtent2D m_windowExtent;

        VkSwapchainKHR m_swapChain;

        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;
        std::vector<VkFence> m_imagesInFlight;
        size_t m_currentFrame = 0;
    };

}  // namespace ae