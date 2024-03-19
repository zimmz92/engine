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

    static const std::size_t MAX_PARTICLES = 1024;

    struct Particle {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec4 color;


        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

            attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32_SFLOAT , offsetof(Particle, position)});
            attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT , offsetof(Particle, color)});

            return attributeDescriptions;
        }

        // Define the vertex binding descriptions used to bind the vertex buffer type to the pipeline.
        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {

            std::vector<VkVertexInputBindingDescription> bindingDescriptions;

            bindingDescriptions.push_back({0,sizeof(Particle),VK_VERTEX_INPUT_RATE_VERTEX});

            return bindingDescriptions;
        }
    };

    class AeParticleSystem {
    public:

        AeParticleSystem(AeDevice& t_aeDevice,
                         std::vector<VkDescriptorSetLayout> t_computeDescriptorSetLayouts,
                         std::vector<std::unique_ptr<AeBuffer>>& t_particleBuffers,
                         VkRenderPass t_renderPass);

        ~AeParticleSystem();

        VkPipelineLayout& getComputePipelineLayout(){return m_computePipelineLayout;};

        void bindComputePipeline(VkCommandBuffer& t_commandBuffer);

        void drawParticles(VkCommandBuffer &t_commandBuffer,
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