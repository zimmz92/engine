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

        AeSwapChain(AeDevice& t_deviceRef, VkExtent2D t_windowExtent);
        ~AeSwapChain();

        // Do not allow this class to be copied (2 lines below)
        AeSwapChain(const AeSwapChain&) = delete;
        void operator=(const AeSwapChain&) = delete;

        VkFramebuffer getFrameBuffer(int t_index) { return m_swapChainFramebuffers[t_index]; }
        VkRenderPass getRenderPass() { return m_renderPass; }
        VkImageView getImageView(int t_index) { return m_swapChainImageViews[t_index]; }
        size_t imageCount() { return m_swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() { return m_swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() { return m_swapChainExtent; }
        uint32_t width() { return m_swapChainExtent.width; }
        uint32_t height() { return m_swapChainExtent.height; }

        float extentAspectRatio() {
            return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
        }
        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t* t_imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer* t_buffers, uint32_t* t_imageIndex);

    private:
        void createSwapChain();
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& t_availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& t_availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& t_capabilities);

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