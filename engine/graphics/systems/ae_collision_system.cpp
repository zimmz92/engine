/// \file ##.cpp
/// The ## class is implemented.
#include "ae_collision_system.hpp"

// dependencies

// libraries

// std
#include <stdexcept>
#include <random>


namespace ae {

    AeCollisionSystem::AeCollisionSystem(AeDevice& t_aeDevice,
                                       std::vector<VkDescriptorSetLayout> t_computeDescriptorSetLayouts,
                                       std::vector<std::unique_ptr<AeBuffer>>& t_particleBuffers,
                                       VkRenderPass t_renderPass) : m_aeDevice{t_aeDevice}{

        // Creates the compute pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
        createComputePipelineLayout(t_computeDescriptorSetLayouts);

        // Creates a compute pipeline for this render system and sets the m_aePipeline member variable.
        createComputePipeline();

        // Creates the pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
        createPipelineLayout();

        // Creates a graphics pipeline for this render system and sets the m_aePipeline member variable.
        createPipeline(t_renderPass);
    };



    AeCollisionSystem::~AeCollisionSystem(){

        vkDestroyPipelineLayout(m_aeDevice.device(), m_computePipelineLayout, nullptr);
        vkDestroyPipelineLayout(m_aeDevice.device(),m_pipelineLayout, nullptr);

    };



    // Creates the pipeline layout for the collision compute system.
    void AeCollisionSystem::createComputePipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts) {

        // Define the specific layout of the collision compute.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(t_descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = t_descriptorSetLayouts.data();

        // Attempt to create the pipeline layout, if it cannot error out.
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_computePipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the default compute pipeline layout!");
        }

    };



    // Creates the pipeline for the collision compute system.
    void AeCollisionSystem::createComputePipeline() {

        // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
        assert(m_computePipelineLayout != nullptr && "Cannot create point light render system's pipeline before pipeline layout!");

        // Create the collision compute system pipeline.
        m_aeComputePipeline = std::make_unique<AeComputePipeline>(m_aeDevice,
                                                                  m_computePipelineLayout,
                                                                  "engines/graphics/shaders/particles.comp.spv");
    };



    void AeCollisionSystem::bindComputePipeline(VkCommandBuffer& t_commandBuffer){
        m_aeComputePipeline->bind(t_commandBuffer);
    };

    void AeCollisionSystem::recordComputeCommandBuffer(VkCommandBuffer& t_commandBuffer, std::vector<VkDescriptorSet>& t_descriptorSets){

        bindComputePipeline(t_commandBuffer);

        vkCmdBindDescriptorSets(t_commandBuffer,
                                VK_PIPELINE_BIND_POINT_COMPUTE,
                                m_computePipelineLayout,
                                0,
                                static_cast<uint32_t>(t_descriptorSets.size()),
                                t_descriptorSets.data(),
                                0,
                                nullptr);

        vkCmdDispatch(t_commandBuffer, NUM_PARTICLES / 256, 1, 1);

        if (vkEndCommandBuffer(t_commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record compute command buffer!");
        }
    };

    void AeCollisionSystem::drawAABBs(VkCommandBuffer &t_commandBuffer,
                                         VkBuffer& t_computeBuffer){

        // Bind the graphics pipeline setup to draw the particles in the compute buffer.
        m_aePipeline->bind(t_commandBuffer);

        // Bind the compute buffer with the updated particle positions to the pipeline.
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(t_commandBuffer, 0, 1, &t_computeBuffer, offsets);

        // Draw the particles.
        vkCmdDraw(t_commandBuffer, NUM_PARTICLES, 1, 0, 0);

    }

    void AeCollisionSystem::drawOBBs(VkCommandBuffer &t_commandBuffer,
                                          VkBuffer& t_computeBuffer){

        // Bind the graphics pipeline setup to draw the particles in the compute buffer.
        m_aePipeline->bind(t_commandBuffer);

        // Bind the compute buffer with the updated particle positions to the pipeline.
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(t_commandBuffer, 0, 1, &t_computeBuffer, offsets);

        // Draw the particles.
        vkCmdDraw(t_commandBuffer, NUM_PARTICLES, 1, 0, 0);

    }



    // Creates the pipeline layout for the point light render system.
    void AeCollisionSystem::createPipelineLayout() {

        // Define the specific layout of the point light renderer.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        // Attempt to create the pipeline layout, if it cannot error out.
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the point light render system's pipeline layout!");
        }

    };



    // Creates the pipeline for the point light render system.
    void AeCollisionSystem::createPipeline(VkRenderPass t_renderPass) {

        // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
        assert(m_pipelineLayout != nullptr && "Cannot create point light render system's pipeline before pipeline layout!");

        // Define the pipeline to be created.
        GraphicsPipelineConfigInfo pipelineConfig{};
        AeGraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig);
        AeGraphicsPipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.bindingDescriptions = Particle::getBindingDescriptions();
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.attributeDescriptions = Particle::getAttributeDescriptions();
        pipelineConfig.renderPass = t_renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_POINT;

        GraphicsShaderFilesPaths shaderPaths{};
        shaderPaths.vertFilepath = "engines/graphics/shaders/particles.vert.spv";
        shaderPaths.fragFilepath = "engines/graphics/shaders/particles.frag.spv";

        // Create the point light render system pipeline.
        m_aePipeline = std::make_unique<AeGraphicsPipeline>(
                m_aeDevice,
                shaderPaths,
                pipelineConfig);
    };



} //namespace ae