#include "ae_swap_chain.hpp"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace ae {

    // Function that creates a swap chain object.
    AeSwapChain::AeSwapChain(AeDevice& t_deviceRef, VkExtent2D t_extent)
        : m_device{ t_deviceRef }, m_windowExtent{ t_extent } {

        // Create the swap chain
        init();
    }

    // Function that creates a swap chain object reusing as much of the specified swap chain as possible.
    AeSwapChain::AeSwapChain(AeDevice& t_deviceRef, VkExtent2D t_extent, std::shared_ptr<AeSwapChain> t_previous)
        : m_device{ t_deviceRef }, m_windowExtent{ t_extent }, m_oldSwapChain{t_previous} {

        // Create the swap chain referencing the specified swap chain.
        init();

        // Clean up the old swap chain and release it from memory if nothing else is using it.
        m_oldSwapChain = nullptr;
    }

    // Function that creates a swap chain object
    void AeSwapChain::init() {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createSyncObjects();
    }



    // Function that destroys a swap chain object
    AeSwapChain::~AeSwapChain() {

        // Destroy all the image views.
        for (auto imageView : m_swapChainImageViews) {
            vkDestroyImageView(m_device.device(), imageView, nullptr);
        }
        m_swapChainImageViews.clear();

        // Destroy the swap chain itself.
        if (m_swapChain != nullptr) {
            vkDestroySwapchainKHR(m_device.device(), m_swapChain, nullptr);
            m_swapChain = nullptr;
        }

        // Destroy the depth images.
        for (int i = 0; i < m_depthImages.size(); i++) {
            vkDestroyImageView(m_device.device(), m_depthImageViews[i], nullptr);
            vkDestroyImage(m_device.device(), m_depthImages[i], nullptr);
            vkFreeMemory(m_device.device(), m_depthImageMemories[i], nullptr);
        }

        // Destroy the frame buffers.
        for (auto framebuffer : m_swapChainFramebuffers) {
            vkDestroyFramebuffer(m_device.device(), framebuffer, nullptr);
        }

        // Destroy the render pass.
        vkDestroyRenderPass(m_device.device(), m_renderPass, nullptr);

        // Destroy the synchronization objects
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_device.device(), m_renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_device.device(), m_imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(m_device.device(), m_computeFinishedSemaphores[i], nullptr);
            vkDestroyFence(m_device.device(), m_commandBufferInFlightFences[i], nullptr);
            vkDestroyFence(m_device.device(), m_computeInFlightFences[i], nullptr);
        }
    }

    // Function to acquire the next image from the swap chain.
    VkResult AeSwapChain::acquireNextImage(uint32_t* t_imageIndex) {

        // Ensure that the device frame is ready to begin rendering a new image.
        vkWaitForFences(
            m_device.device(),
            1,
            &m_commandBufferInFlightFences[m_currentFrame],
            VK_TRUE,
            std::numeric_limits<uint64_t>::max());

        // Acquire the image to be rendered to.
        VkResult result = vkAcquireNextImageKHR(
            m_device.device(),
            m_swapChain,
            std::numeric_limits<uint64_t>::max(),
            m_imageAvailableSemaphores[m_currentFrame],  // must be a not signaled semaphore
            VK_NULL_HANDLE,
            t_imageIndex);

        return result;
    }

    // Function to submit command buffers to the swap chain
    VkResult AeSwapChain::submitCommandBuffers(const VkCommandBuffer* t_graphicsBuffer, const VkCommandBuffer* t_computeBuffer, const uint32_t* t_imageIndex) {

        // Create blank info struct.
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


        // Ensure that the compute queue has finished before submitting another. Once they are done reset the fence.
        vkWaitForFences(m_device.device(), 1, &m_computeInFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(m_device.device(), 1, &m_computeInFlightFences[m_currentFrame]);


        // Create the info struct to submit the command buffer to the queue
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = t_computeBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_computeFinishedSemaphores[m_currentFrame];

        // Attempt to submit and execute the command buffer using the compute queue.
        if (vkQueueSubmit(m_device.computeQueue(), 1, &submitInfo, m_computeInFlightFences[m_currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit compute command buffer!");
        }



        // Ensure that the previously submitted command buffers for the image have finished before submitting new ones.
        if (m_imagesInFlightFences[*t_imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(m_device.device(), 1, &m_imagesInFlightFences[*t_imageIndex], VK_TRUE, UINT64_MAX);
        }

        // Set the fence tracking if the command buffers corresponding to the image have finished to the fence keeping
        // track of the frame command buffer status. This is useful when we have more images than frames.
        m_imagesInFlightFences[*t_imageIndex] = m_commandBufferInFlightFences[m_currentFrame];

        // Reset the fences tracking the execution status of the current frame.
        vkResetFences(m_device.device(), 1, &m_commandBufferInFlightFences[m_currentFrame]);

        // Create the info struct to submit the command buffer to the queue
        submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // Specify the image tracking semaphores.
        VkSemaphore waitSemaphores[] = { m_computeFinishedSemaphores[m_currentFrame], m_imageAvailableSemaphores[m_currentFrame] };
        //VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        //VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 2;
        //submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        // Specify the command buffers that will be submitted and executed.
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = t_graphicsBuffer;

        // Specify the render tracking semaphores.
        VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // Attempt to submit and execute the command buffer using the device queue.
        if (vkQueueSubmit(m_device.graphicsQueue(), 1, &submitInfo, m_commandBufferInFlightFences[m_currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }




        // Create the info struct to submit the rendered image to the presentation queue.
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        // Specify the semaphores tracking the rendering status.
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        // Specify the swap chain holding the queue of images to be presented.
        VkSwapchainKHR swapChains[] = { m_swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        // The index of the image in the swap chain to be presented when ready.
        presentInfo.pImageIndices = t_imageIndex;

        // Attempt to submit the image to the present queue.
        auto result = vkQueuePresentKHR(m_device.presentQueue(), &presentInfo);

        // Increment the frame tracker.
        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        // Return VK_SUCCESS if the image was submitted to the present queue.
        return result;
    }

    // Function to create a swap chain object
    void AeSwapChain::createSwapChain() {

        // Get the swap chain support details for the GPU.
        SwapChainSupportDetails swapChainSupport = m_device.getSwapChainSupport();

        // Choose an appropriate surface format, present mode, and extent for the swap chain based on what is supported
        // by the GPU.
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        // Keep the number of simultaneous images to a minimum at the moment.
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        // Ensure that the chosen image count does not exceed the GPU's capabilities.
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }


        // Create and populate the swap chain creation information structure.
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_device.surface();

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = m_device.findPhysicalQueueFamilies();
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

        // Optimize the swap chain if the graphics and present queue families are the same queue family.
        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;      // Optional
            createInfo.pQueueFamilyIndices = nullptr;  // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

        // Allow alpha coloring.
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        // If the old swap chain exists then ensure that the new swap chain reuses anything it can, this should improve
        // the creation performance. Depending on the system this can also help with full screen permissions.
        createInfo.oldSwapchain = m_oldSwapChain == nullptr ? VK_NULL_HANDLE : m_oldSwapChain->m_swapChain;

        if (vkCreateSwapchainKHR(m_device.device(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        // Only a minimum number of images in the swap chain was specified upon creation, however, the implementation is
        // allowed to create a swap chain with more. Query the final number of images with vkGetSwapchainImagesKHR, then
        // resize the image container and finally call it again to retrieve the image handles.
        vkGetSwapchainImagesKHR(m_device.device(), m_swapChain, &imageCount, nullptr);
        m_swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_device.device(), m_swapChain, &imageCount, m_swapChainImages.data());

        // With the swap chain fully created save the properties of the swap chain.
        m_swapChainImageFormat = surfaceFormat.format;
        m_swapChainExtent = extent;
    }

    // Create image views for a swap chain
    void AeSwapChain::createImageViews() {

        // Ensure there is an image view for each of the images the swap chain will be tracking.
        m_swapChainImageViews.resize(m_swapChainImages.size());

        // Create the image views.
        for (size_t i = 0; i < m_swapChainImages.size(); i++) {
            // Specify the properties of the image view to be created.
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_swapChainImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = m_swapChainImageFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            // Attempt to create the image view.
            if (vkCreateImageView(m_device.device(), &viewInfo, nullptr, &m_swapChainImageViews[i]) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    // Add a render the pass to the swap chain, the render pass is a template for a frame buffer.
    void AeSwapChain::createRenderPass() {

        // Specify the image depth information.
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // Specify the image color information.
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = getSwapChainImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // Define the sub-pass attributes.
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        // Define the dependency attributes.
        VkSubpassDependency dependency = {};
        dependency.dstSubpass = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

        // Define the render creation struct.
        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        // Attempt to create the render pass.
        if (vkCreateRenderPass(m_device.device(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    // Function to create the swap chain frame buffers
    void AeSwapChain::createFramebuffers() {

        // Create a frame buffer for each of the swap chain images.
        m_swapChainFramebuffers.resize(imageCount());
        for (size_t i = 0; i < imageCount(); i++) {

            // Get the images, and their views, that the frame buffer will be responsible for.
            std::array<VkImageView, 2> attachments = { m_swapChainImageViews[i], m_depthImageViews[i] };

            // Get the extent, resolution, of the image the buffer will be rendering for.
            VkExtent2D swapChainExtent = getSwapChainExtent();

            // Specify the frame buffer properties.
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            // Attempt to create the image's frame buffer.
            if (vkCreateFramebuffer(
                m_device.device(),
                &framebufferInfo,
                nullptr,
                &m_swapChainFramebuffers[i]) != VK_SUCCESS) {

                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    // Function to add depth resources to the swap chain
    void AeSwapChain::createDepthResources() {

        // Get the depth format of the swap chain.
        VkFormat depthFormat = findDepthFormat();
        m_swapChainDepthFormat = depthFormat;

        // Get the extent, resolution, of the swap chain.
        VkExtent2D swapChainExtent = getSwapChainExtent();

        // Size the image depth properties for the number of images managed by the swap chain.
        m_depthImages.resize(imageCount());
        m_depthImageMemories.resize(imageCount());
        m_depthImageViews.resize(imageCount());

        for (int i = 0; i < m_depthImages.size(); i++) {

            // Specify the depth image information
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = swapChainExtent.width;
            imageInfo.extent.height = swapChainExtent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = depthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            // Create the depth images.
            m_device.createImageWithInfo(
                    imageInfo,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    m_depthImages[i],
                    m_depthImageMemories[i]);

            // Specify the depth image view properties.
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_depthImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = depthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            // Attempt to create the depth image view.
            if (vkCreateImageView(m_device.device(), &viewInfo, nullptr, &m_depthImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    // Create the synchronization objects for the swap chain image views to identify when the render finishes.
    void AeSwapChain::createSyncObjects() {

        // Make synchronization objects for each of the frames that may be being rendered.
        m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

        // These will only serve as pointers to the m_commandBufferInFlightFences corresponding to an image. Therefor
        // do not need to be "created" or "destroyed"
        m_imagesInFlightFences.resize(imageCount(), VK_NULL_HANDLE);

        m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_commandBufferInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        m_computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        // Specify the semaphore properties. This is the same for all the frames currently.
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        // Specify the fence properties. This is the same for all the frames currently.
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        // Attempt to create the synchronization objects for each of the frames.
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(m_device.device(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateSemaphore(m_device.device(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateSemaphore(m_device.device(), &semaphoreInfo, nullptr, &m_computeFinishedSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateFence(m_device.device(), &fenceInfo, nullptr, &m_commandBufferInFlightFences[i]) != VK_SUCCESS ||
                vkCreateFence(m_device.device(), &fenceInfo, nullptr, &m_computeInFlightFences[i]) != VK_SUCCESS){
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    // Choose a swap surface format based on the desired criteria within this function.
    VkSurfaceFormatKHR AeSwapChain::chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& t_availableFormats) {

        // Check the available formats for one that meets the criteria define here.
        for (const auto& availableFormat : t_availableFormats) {

            // Choose a color format that applies the proper gamma correction, in this case VK_FORMAT_B8G8R8A8_SRGB.
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {

                return availableFormat;
            }
        }

        // If none of the available formats meets the desired criteria just return the first one.
        // TODO Should probably try to find the most compatible format and not just return whatever is first.
        return t_availableFormats[0];
    }

    // Select the methodology for how the image presentation will synchronization with the screen's refreshing.
    VkPresentModeKHR AeSwapChain::chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& t_availablePresentModes) {

        // Mailbox present mode. Synchronize images with screen refresh rate, newest image is always used.
        for (const auto& availablePresentMode : t_availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                std::cout << "Present mode: Mailbox" << std::endl;
                return availablePresentMode;
            }
        }

        // Immediate present mode. No synchronization with the screen's refresh. Images rendered as soon as they are
        // available. Will most likely result in screen tearing.
        // for (const auto &availablePresentMode : t_availablePresentModes) {
        //   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
        //     std::cout << "Present mode: Immediate" << std::endl;
        //     return availablePresentMode;
        //   }
        // }

        // Default to FIFO VSYNC. Images are synchronized to the screen's refresh but are always presented in the order
        // in which they are rendered even if they are "old".
        std::cout << "Present mode: V-Sync" << std::endl;
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    // Swap chain image extend, resolution, based on the size of the given surface capabilities.
    VkExtent2D AeSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& t_capabilities) {

        // Check if the provided capabilities already have a defined extent.
        if (t_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return t_capabilities.currentExtent;
        }
        else {
            // Set the extent to either the size of the screen or the maximum capabilities of the device.
            VkExtent2D actualExtent = m_windowExtent;
            actualExtent.width = std::max(
                t_capabilities.minImageExtent.width,
                std::min(t_capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(
                t_capabilities.minImageExtent.height,
                std::min(t_capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    // Gets a supported depth format for the swap chain.
    VkFormat AeSwapChain::findDepthFormat() {
        return m_device.findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

}  // namespace ae