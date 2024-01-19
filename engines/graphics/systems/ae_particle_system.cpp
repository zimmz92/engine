/// \file ##.cpp
/// The ## class is implemented.
#include "ae_particle_system.hpp"

// dependencies

// libraries

// std
#include <stdexcept>
#include <random>


namespace ae {

    AeParticleSystem::AeParticleSystem(AeDevice& t_aeDevice,
                                       std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts,
                                       std::vector<std::unique_ptr<AeBuffer>>& t_particleBuffers) : m_aeDevice{t_aeDevice}{

        // Creates the pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
        createPipelineLayout(t_descriptorSetLayouts);

        // Creates a graphics pipeline for this render system and sets the m_aePipeline member variable.
        createPipeline();

        // Initialize particles
        std::default_random_engine rndEngine((unsigned)time(nullptr));
        std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

        // Initial particle positions on a circle
        std::vector<Particle> particles(MAX_PARTICLES);
        for (auto& particle : particles) {
            float r = 0.25f * sqrt(rndDist(rndEngine));
            float theta = rndDist(rndEngine) * 2 * 3.14159265358979323846;
            float x = r * cos(theta) * 600 / 800;
            float y = r * sin(theta);
            particle.position = glm::vec2(x, y);
            particle.velocity = glm::normalize(glm::vec2(x,y)) * 0.00025f;
            particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
        }

        VkDeviceSize bufferSize = sizeof(Particle) * MAX_PARTICLES;
        AeBuffer stagingBuffer = AeBuffer(m_aeDevice,
                                          sizeof(Particle),
                                          MAX_PARTICLES,
                                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


        if (stagingBuffer.map() != VK_SUCCESS) {
            throw std::runtime_error("Failed to map the particle system staging buffer memory!");
        };

        stagingBuffer.writeToBuffer(particles.data());

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Copy data from the staging buffer (host) to the shader storage buffer (GPU)
            m_aeDevice.copyBuffer(stagingBuffer.getBuffer(), t_particleBuffers[i]->getBuffer(), bufferSize);
        };

    };



    AeParticleSystem::~AeParticleSystem(){

        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);

    };



    // Creates the pipeline layout for the point light render system.
    void AeParticleSystem::createPipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts) {

        // Define the specific layout of the point light renderer.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(t_descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = t_descriptorSetLayouts.data();

        // Attempt to create the pipeline layout, if it cannot error out.
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the default compute pipeline layout!");
        }

    };



    // Creates the pipeline for the point light render system.
    void AeParticleSystem::createPipeline() {

        // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
        assert(m_pipelineLayout != nullptr && "Cannot create point light render system's pipeline before pipeline layout!");

        // Create the point light render system pipeline.
        m_aePipeline = std::make_unique<AeComputePipeline>( m_aeDevice,
                                                            m_pipelineLayout,
                                                            "engines/graphics/shaders/particles.comp.spv");
    };




    void AeParticleSystem::bindPipeline(VkCommandBuffer& t_commandBuffer){
        m_aePipeline->bind(t_commandBuffer);
    };



} //namespace ae