/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_engine_constants.hpp"
#include "ae_device.hpp"
#include "ae_compute_pipeline.hpp"
#include "ae_graphics_pipeline.hpp"
#include "ae_buffer.hpp"
#include "ae_renderer.hpp"

// libraries

// std


namespace ae {

    class AeCollisionSystem {
    public:

        AeCollisionSystem(AeDevice& t_aeDevice,
                         std::vector<VkDescriptorSetLayout> t_computeDescriptorSetLayouts,
                         std::vector<std::unique_ptr<AeBuffer>>& t_particleBuffers,
                         VkRenderPass t_renderPass);

        ~AeCollisionSystem();

        VkPipelineLayout& getComputePipelineLayout(){return m_computePipelineLayout;};

        void bindComputePipeline(VkCommandBuffer& t_commandBuffer);

        void drawAABBs(VkCommandBuffer &t_commandBuffer,
                           VkBuffer& t_computeBuffer);

        void drawOBBs(VkCommandBuffer &t_commandBuffer,
                      VkBuffer& t_computeBuffer);

        void recordComputeCommandBuffer(VkCommandBuffer& t_commandBuffer, std::vector<VkDescriptorSet>& t_descriptorSets);

    private:

        /// Creates the pipeline layout for the point light render system.
        /// \param t_globalSetLayout The general descriptor set for the devices and general rendering setting that need
        /// to be accounted for when setting up the render pipeline for this system.
        void createComputePipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts);

        /// Creates the pipeline based on the render pass this pipeline should be associated with.
        void createComputePipeline();


        void createPipelineLayout();

        void createPipeline(VkRenderPass t_renderPass);

        /// The graphics device this render system will be using.
        AeDevice& m_aeDevice;

        /// The layout of this render system's pipeline.
        VkPipelineLayout m_pipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AeGraphicsPipeline> m_aePipeline;

        /// The layout of this render system's pipeline.
        VkPipelineLayout m_computePipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AeComputePipeline> m_aeComputePipeline;

    protected:

    };

} // namespace ae