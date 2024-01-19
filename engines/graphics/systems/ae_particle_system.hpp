/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_graphics_constants.hpp"
#include "ae_device.hpp"
#include "ae_pipeline.hpp"

// libraries

// std


namespace ae {

    class AeParticleSystem {
    public:
        AeParticleSystem(AeDevice& t_aeDevice,
                         VkRenderPass t_renderPass,
                         VkDescriptorSetLayout t_globalSetLayout);

        ~AeParticleSystem();

    private:

        /// Creates the pipeline layout for the point light render system.
        /// \param t_globalSetLayout The general descriptor set for the devices and general rendering setting that need
        /// to be accounted for when setting up the render pipeline for this system.
        void createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout);

        /// Creates the pipeline based on the render pass this pipeline should be associated with.
        /// \param t_renderPass The render pass this pipeline should be associated with.
        void createPipeline(VkRenderPass t_renderPass);

        /// The graphics device this render system will be using.
        AeDevice& m_aeDevice;

        /// The layout of this render system's pipeline.
        VkPipelineLayout m_pipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AeGraphicsPipeline> m_aePipeline;

    protected:

    };

} // namespace ae