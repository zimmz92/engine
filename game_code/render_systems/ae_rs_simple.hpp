#pragma once

#include "ae_device.hpp"
#include "ae_game_object.hpp"
#include "ae_pipeline.hpp"


#include <memory>
#include <vector>

namespace ae {
    class AeRsSimple {
    public:

        AeRsSimple(AeDevice &t_device, VkRenderPass t_renderPass);
        ~AeRsSimple();

        // Do not allow this class to be copied (2 lines below)
        AeRsSimple(const AeRsSimple&) = delete;
        AeRsSimple& operator=(const AeRsSimple&) = delete;

        void renderGameObjects(VkCommandBuffer t_commandBuffer, std::vector<AeGameObject> &t_gameObjects);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass t_renderPass);


        AeDevice &m_aeDevice;
        std::unique_ptr<AePipeline> m_aePipeline; // TODO: Smart Pointer research, learn Cpp
        VkPipelineLayout m_pipelineLayout;
    };
}  // namespace ae