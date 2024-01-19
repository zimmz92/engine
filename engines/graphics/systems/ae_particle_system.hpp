/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_graphics_constants.hpp"
#include "ae_device.hpp"
#include "ae_compute_pipeline.hpp"
#include "ae_buffer.hpp"

// libraries

// std


namespace ae {

    class AeParticleSystem {
    public:


        AeParticleSystem(AeDevice& t_aeDevice,
                         std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts,
                         std::vector<std::unique_ptr<AeBuffer>>& t_particleBuffers);

        ~AeParticleSystem();

        AeComputePipeline* getPipeline(){return m_aePipeline.get();};

        VkPipelineLayout& getPipelineLayout(){return m_pipelineLayout;};

        void bindPipeline(VkCommandBuffer& t_commandBuffer);

    private:

        /// Creates the pipeline layout for the point light render system.
        /// \param t_globalSetLayout The general descriptor set for the devices and general rendering setting that need
        /// to be accounted for when setting up the render pipeline for this system.
        void createPipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts);

        /// Creates the pipeline based on the render pass this pipeline should be associated with.
        void createPipeline();

        /// The graphics device this render system will be using.
        AeDevice& m_aeDevice;

        /// The layout of this render system's pipeline.
        VkPipelineLayout m_pipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AeComputePipeline> m_aePipeline;

        /// The object buffers for each frame
        std::vector<std::unique_ptr<AeBuffer>> m_computeBuffers;


    protected:

    };

} // namespace ae