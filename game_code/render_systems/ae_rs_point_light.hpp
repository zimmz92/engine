#pragma once

#include "ae_device.hpp"
#include "ae_game_object.hpp"
#include "ae_pipeline.hpp"
#include "ae_camera.hpp"
#include "ae_frame_info.hpp"

#include <memory>
#include <vector>

namespace ae {
    class AeRsPointLight {
    public:

        AeRsPointLight(AeDevice &t_device, VkRenderPass t_renderPass, VkDescriptorSetLayout t_globalSetLayout);
        ~AeRsPointLight();

        // Do not allow this class to be copied (2 lines below)
        AeRsPointLight(const AeRsPointLight&) = delete;
        AeRsPointLight& operator=(const AeRsPointLight&) = delete;

        void update(FrameInfo& t_frameInfo, GlobalUbo& t_ubo);
        void render(FrameInfo &t_frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout);
        void createPipeline(VkRenderPass t_renderPass);


        AeDevice &m_aeDevice;
        std::unique_ptr<AePipeline> m_aePipeline; // TODO: Smart Pointer research, learn Cpp
        VkPipelineLayout m_pipelineLayout;
    };
}  // namespace ae