/// \file ##.cpp
/// The ## class is implemented.
#include "ae_collision_system.hpp"

// dependencies

// libraries

// std
#include <stdexcept>
#include <random>


namespace ae {

    AeCollisionSystem::AeCollisionSystem(ae_ecs::AeECS& t_ecs,
                                         GameComponents& t_game_components,
                                         AeDevice& t_aeDevice,
                                         VkRenderPass t_renderPass,
                                         std::vector<VkDescriptorSetLayout> t_computeDescriptorSetLayouts,
                                         std::vector<VkDescriptorSetLayout> t_aabbDescriptorSetLayouts)
            : m_worldPositionComponent{t_game_components.worldPositionComponent},
              m_modelComponent{t_game_components.modelComponent},
              m_aeDevice{t_aeDevice},
              ae_ecs::AeSystem<AeCollisionSystem>(t_ecs) {

        // Register component dependencies
        m_worldPositionComponent.requiredBySystem(this->getSystemId());
        m_modelComponent.requiredBySystem(this->getSystemId());

        // Register system dependencies
        // This is a child system and dependencies, as well as execution, will be handled by the parent system,
        // RendererSystem.
        this->isChildSystem = true;

        // Creates the compute pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
        createComputePipelineLayout(t_computeDescriptorSetLayouts);

        // Creates a compute pipeline for this render system and sets the m_aePipeline member variable.
        createComputePipeline();

        // Creates the pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
        createAabbPipelineLayout(t_aabbDescriptorSetLayouts);

        // Creates a graphics pipeline for this render system and sets the m_aePipeline member variable.
        createAabbPipeline(t_renderPass);

        // Enable the system so it will run.
        this->enableSystem();
    };



    AeCollisionSystem::~AeCollisionSystem(){

        vkDestroyPipelineLayout(m_aeDevice.device(), m_computePipelineLayout, nullptr);
        vkDestroyPipelineLayout(m_aeDevice.device(),m_pipelineLayout, nullptr);

    };



    // Creates the pipeline layout for the collision compute system.
    void AeCollisionSystem::createComputePipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts) {

        // Define the push constant information for this pipeline.
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(uint32_t);

        // Define the specific layout of the collision compute.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(t_descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = t_descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        // Attempt to create the pipeline layout, if it cannot error out.
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_computePipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the default compute pipeline layout!");
        }

    };



    // Creates the pipeline for the collision compute system.
    void AeCollisionSystem::createComputePipeline() {

        // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
        assert(m_computePipelineLayout != nullptr && "Cannot create collision compute pipeline before pipeline layout!");

        // Create the collision compute system pipeline.
        m_aeComputePipeline = std::make_unique<AeComputePipeline>(m_aeDevice,
                                                                  m_computePipelineLayout,
                                                                  "engines/graphics/shaders/collision.comp.spv");
    };



    void AeCollisionSystem::bindComputePipeline(VkCommandBuffer& t_commandBuffer){
        m_aeComputePipeline->bind(t_commandBuffer);
    };

    void AeCollisionSystem::recordComputeCommandBuffer(VkCommandBuffer& t_commandBuffer, std::vector<VkDescriptorSet>& t_descriptorSets){

        // Get the entities that use the components this system depends on.
        std::vector<ecs_id> validEntityIds = m_systemManager.getEnabledSystemsEntities(this->getSystemId());
        uint32_t numModels = validEntityIds.size();

        bindComputePipeline(t_commandBuffer);

        vkCmdBindDescriptorSets(t_commandBuffer,
                                VK_PIPELINE_BIND_POINT_COMPUTE,
                                m_computePipelineLayout,
                                0,
                                static_cast<uint32_t>(t_descriptorSets.size()),
                                t_descriptorSets.data(),
                                0,
                                nullptr);

        // Push the point light information to the buffer.
        vkCmdPushConstants(
                t_commandBuffer,
                m_computePipelineLayout,
                VK_SHADER_STAGE_COMPUTE_BIT,
                0,
                sizeof(uint32_t),
                &numModels);

        uint32_t numWorkGroups = (uint32_t)std::ceil((float)numModels/256.0);

        vkCmdDispatch(t_commandBuffer, numWorkGroups, 1, 1);

//        if (vkEndCommandBuffer(t_commandBuffer) != VK_SUCCESS) {
//            throw std::runtime_error("failed to record compute command buffer!");
//        }
    };

    void AeCollisionSystem::drawAABBs(VkCommandBuffer &t_commandBuffer,
                                      std::vector<VkDescriptorSet>& t_descriptorSets){
        // Tell the pipeline what the current command buffer being worked on is.
        m_aePipeline->bind(t_commandBuffer);

        // Bind the descriptor sets to the command buffer.
        vkCmdBindDescriptorSets(t_commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_pipelineLayout,
                                0,
                                static_cast<uint32_t>(t_descriptorSets.size()),
                                t_descriptorSets.data(),
                                0,
                                nullptr);


        std::vector<ecs_id> validEntityIds = m_systemManager.getEnabledSystemsEntities(this->getSystemId());
        uint32_t numModels = validEntityIds.size();

        // Bind the graphics pipeline setup to draw the particles in the compute buffer.
        m_aePipeline->bind(t_commandBuffer);

        // Draw the particles.
        // TODO: Set number of draws appropriately based on number of entities.
        vkCmdDraw(t_commandBuffer, 24, numModels, 0, 0);

    }

    void AeCollisionSystem::drawOBBs(VkCommandBuffer &t_commandBuffer,
                                          VkBuffer& t_computeBuffer){

        // Bind the graphics pipeline setup to draw the particles in the compute buffer.
        m_aePipeline->bind(t_commandBuffer);

        // Bind the compute buffer with the updated particle positions to the pipeline.
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(t_commandBuffer, 0, 1, &t_computeBuffer, offsets);

        // Draw the particles.
        vkCmdDraw(t_commandBuffer, 1, 1, 0, 0);

    }



    // Creates the pipeline layout for the point light render system.
    void AeCollisionSystem::createAabbPipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts) {

        // Define the specific layout of the collision compute.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(t_descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = t_descriptorSetLayouts.data();

        // Attempt to create the pipeline layout, if it cannot error out.
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the point light render system's pipeline layout!");
        }

    };



    // Creates the pipeline for the point light render system.
    void AeCollisionSystem::createAabbPipeline(VkRenderPass t_renderPass) {

        // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
        assert(m_pipelineLayout != nullptr && "Cannot create point light render system's pipeline before pipeline layout!");

        // Define the pipeline to be created.
        GraphicsPipelineConfigInfo pipelineConfig{};
        AeGraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig);
        AeGraphicsPipeline::enableAlphaBlending(pipelineConfig);

        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.bindingDescriptions = Ae3DModel::ObbVertex::getBindingDescriptions();
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.attributeDescriptions = Ae3DModel::ObbVertex::getAttributeDescriptions();

        pipelineConfig.renderPass = t_renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_POINT;

        GraphicsShaderFilesPaths shaderPaths{};
        shaderPaths.vertFilepath = "engines/graphics/shaders/collision.vert.spv";
        shaderPaths.fragFilepath = "engines/graphics/shaders/collision.frag.spv";

        // Create the point light render system pipeline.
        m_aePipeline = std::make_unique<AeGraphicsPipeline>(
                m_aeDevice,
                shaderPaths,
                pipelineConfig);
    };

    // Creates the pipeline layout for the point light render system.
    void AeCollisionSystem::createObbPipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts) {

        // Define the specific layout of the collision compute.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(t_descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = t_descriptorSetLayouts.data();

        // Attempt to create the pipeline layout, if it cannot error out.
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the point light render system's pipeline layout!");
        }

    };



    // Creates the pipeline for the point light render system.
    void AeCollisionSystem::createObbPipeline(VkRenderPass t_renderPass) {

        // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
        assert(m_pipelineLayout != nullptr && "Cannot create point light render system's pipeline before pipeline layout!");

        // Define the pipeline to be created.
        GraphicsPipelineConfigInfo pipelineConfig{};
        AeGraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig);
        AeGraphicsPipeline::enableAlphaBlending(pipelineConfig);

        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.bindingDescriptions = Ae3DModel::ObbVertex::getBindingDescriptions();
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.attributeDescriptions = Ae3DModel::ObbVertex::getAttributeDescriptions();

        pipelineConfig.renderPass = t_renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
        pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

        GraphicsShaderFilesPaths shaderPaths{};
        shaderPaths.vertFilepath = "engines/graphics/shaders/collision.vert.spv";
        shaderPaths.fragFilepath = "engines/graphics/shaders/collision.frag.spv";

        // Create the point light render system pipeline.
        m_aePipeline = std::make_unique<AeGraphicsPipeline>(
                m_aeDevice,
                shaderPaths,
                pipelineConfig);
    };

} //namespace ae