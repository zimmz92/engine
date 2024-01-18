/// \file ae_ui_render_system.cpp
/// \brief The script implementing the system that renders the user interface.
/// The user interface render system is implemented.

#include "ae_ui_render_system.hpp"

// Standard Libraries
#include <map>

namespace ae {

    // Constructor implementation
    UiRenderSystem::UiRenderSystem(ae_ecs::AeECS& t_ecs,
                                   GameComponents &t_game_components,
                                   AeDevice &t_aeDevice,
                                   VkRenderPass t_renderPass,
                                   VkDescriptorSetLayout t_globalSetLayout,
                                   VkDescriptorSetLayout t_textureSetLayout,
                                   VkDescriptorSetLayout t_object2dSetLayout)
            : m_model2DComponent{t_game_components.model2DComponent},
              m_aeDevice{t_aeDevice},
              ae_ecs::AeSystem<UiRenderSystem>(t_ecs) {

        // Register component dependencies
        m_model2DComponent.requiredBySystem(this->getSystemId());


        // Register system dependencies
        // This is a child system and dependencies, as well as execution, will be handled by the parent system,
        // RendererSystem.
        this->isChildSystem = true;

        // Creates the pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
        createPipelineLayout(t_globalSetLayout, t_textureSetLayout, t_object2dSetLayout);

        // Creates a graphics pipeline for this render system and sets the m_aePipeline member variable.
        createPipeline(t_renderPass);

        // Enable the system so it will run.
        this->enableSystem();
    };


    // Destructor implementation
    UiRenderSystem::~UiRenderSystem() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    };


    // Set up the system prior to execution. Currently not used.
    void UiRenderSystem::setupSystem() {};


    // Renders the models.
    void UiRenderSystem::executeSystem(VkCommandBuffer &t_commandBuffer,
                                       VkDescriptorSet t_globalDescriptorSet,
                                       VkDescriptorSet t_textureDescriptorSet,
                                       VkDescriptorSet t_object2dSet,
                                       uint64_t t_frameIndex) {

        // Tell the pipeline what the current command buffer being worked on is.
        m_aePipeline->bind(t_commandBuffer);

        VkDescriptorSet descriptorSetsToBind[] = {t_globalDescriptorSet,
                                                  t_textureDescriptorSet,
                                                  t_object2dSet};

        // Bind the descriptor sets to the command buffer.
        vkCmdBindDescriptorSets(
                t_commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_pipelineLayout,
                0,
                3,
                descriptorSetsToBind,
                0,
                nullptr);


        // Get the entities that use the components this system depends on.
        std::vector<ecs_id> validEntityIds = m_systemManager.getEnabledSystemsEntities(this->getSystemId());


        // Loop through the entities if they have models render them.
        int j=0;
        for (ecs_id entityId: validEntityIds) {

            // Make sure the entity actually has a model to render.
            const Model2dComponentStruct& entityModelData = m_model2DComponent.getReadOnlyDataReference(entityId);
            if (entityModelData.m_2d_model == nullptr) continue;

            // Bind the model buffer(s) to the command buffer.
            entityModelData.m_2d_model->bind(t_commandBuffer);

            // Draw the model.
            entityModelData.m_2d_model->draw(t_commandBuffer,j);
            j++;
        };
    };


    // Clean up the system after execution. Currently not used.
    void UiRenderSystem::cleanupSystem() {
        m_systemManager.clearSystemEntityUpdateSignatures(m_systemId);
    };


    // Creates the pipeline layout for the point light render system.
    void UiRenderSystem::createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout,
                                              VkDescriptorSetLayout t_textureSetLayout,
                                              VkDescriptorSetLayout t_object2dSetLayout) {

        // Prepare the descriptor set layouts based on the global set layout for the device.
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{t_globalSetLayout,
                                                                t_textureSetLayout,
                                                               t_object2dSetLayout};

        // Define the specific layout of the point light renderer.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

        // Attempt to create the pipeline layout, if it cannot error out.
        if (vkCreatePipelineLayout(m_aeDevice.device(),
                                   &pipelineLayoutInfo,
                                   nullptr,
                                   &m_pipelineLayout)
                                   != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the ui render system's pipeline layout!");
        }

    };


    // Creates the pipeline for the point light render system.
    void UiRenderSystem::createPipeline(VkRenderPass t_renderPass) {

        // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
        assert(m_pipelineLayout != nullptr &&
               "Cannot create the ui render system's pipeline before pipeline layout!");

        // Define the pipeline to be created.
        PipelineConfigInfo pipelineConfig{};
        AePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = t_renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        // Specify that this pipeline will be dealing with the 2D vertexes.
        pipelineConfig.bindingDescriptions = Ae2DModel::Vertex2D::getBindingDescriptions();
        pipelineConfig.attributeDescriptions = Ae2DModel::Vertex2D::getAttributeDescriptions();

        shaderFilesPaths shaderPaths{};
        shaderPaths.vertFilepath = "engines/graphics/shaders/ui_shader.vert.spv";
        shaderPaths.fragFilepath = "engines/graphics/shaders/ui_shader.frag.spv";


        // Create the point light render system pipeline.
        m_aePipeline = std::make_unique<AePipeline>(
                m_aeDevice,
                shaderPaths,
                pipelineConfig);
    };

    // Calculates the push constant data for the specified entity
    UiPushConstantData UiRenderSystem::calculatePushConstantData(glm::vec2 t_translation, float t_rotation, glm::vec2 t_scale) {
        const float s = glm::sin(t_rotation);
        const float c = glm::cos(t_rotation);
        glm::mat2 rotMatrix{{c, s}, {-s, c}};

        glm::mat2 scaleMat{{t_scale.x, .0f}, {.0f, t_scale.y}};
        return {rotMatrix * scaleMat,t_translation};
    };
}