#pragma once

#include "ae_device.hpp"
#include "ae_game_object.hpp"
#include "ae_graphics_pipeline.hpp"
#include "ae_frame_info.hpp"

#include <memory>
#include <vector>

namespace ae {
    class AeRsSimple {
    public:

        AeRsSimple(AeDevice &t_device, VkRenderPass t_renderPass, VkDescriptorSetLayout t_globalSetLayout);
        ~AeRsSimple();

        // Do not allow this class to be copied (2 lines below)
        AeRsSimple(const AeRsSimple&) = delete;
        AeRsSimple& operator=(const AeRsSimple&) = delete;

        void renderGameObjects(FrameInfo &t_frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout);
        void createPipeline(VkRenderPass t_renderPass);


        AeDevice &m_aeDevice;
        std::unique_ptr<AePipeline> m_aePipeline; // TODO: Smart Pointer research, learn Cpp
        VkPipelineLayout m_pipelineLayout;
    };
}  // namespace ae