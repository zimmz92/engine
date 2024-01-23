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

    static const std::size_t MAX_PARTICLES = 256;

    struct Particle {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec4 color;


        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

            attributeDescriptions[0].binding = 1;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Particle, position);

            attributeDescriptions[1].binding = 1;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Particle, color);

            return attributeDescriptions;
        }
    };

    class AeParticleSystem {
    public:

        AeParticleSystem(AeDevice& t_aeDevice,
                         std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts,
                         std::vector<std::unique_ptr<AeBuffer>>& t_particleBuffers);

        ~AeParticleSystem();

        VkPipelineLayout& getComputePipelineLayout(){return m_computePipelineLayout;};

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
        VkPipelineLayout m_computePipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AeComputePipeline> m_aeComputePipeline;

        /// The object buffers for each frame
        std::vector<std::unique_ptr<AeBuffer>> m_computeBuffers;


    protected:

    };

} // namespace ae