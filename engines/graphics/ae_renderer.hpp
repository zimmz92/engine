#pragma once

#include "ae_device.hpp"
#include "ae_swap_chain.hpp"
#include "ae_window.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace ae {
    class AeRenderer {
    public:

        AeRenderer(AeWindow& t_window, AeDevice& t_device);
        ~AeRenderer();

        // Do not allow this class to be copied (2 lines below)
        AeRenderer(const AeRenderer&) = delete;
        AeRenderer& operator=(const AeRenderer&) = delete;

        VkRenderPass getSwapChainRenderPass() const { return m_aeSwapChain->getRenderPass(); }
        const bool isFrameInProgress() { return m_isFrameStarted; };

        const VkCommandBuffer getCurrentCommandBuffer() {
            assert(isFrameInProgress() && "Cannot get command buffer when frame not in progress");
            return m_commandBuffers[m_currentImageIndex];
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer t_commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer t_commandBuffer);


    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();


        AeWindow& m_aeWindow;
        AeDevice& m_aeDevice;
        std::unique_ptr<AeSwapChain> m_aeSwapChain;
        std::vector<VkCommandBuffer> m_commandBuffers;

        uint32_t m_currentImageIndex;
        bool m_isFrameStarted = false;
    };
}  // namespace ae