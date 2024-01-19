/// \file point_light_render_system.cpp
/// \brief The script implementing the system that renders the point lights.
/// The point light render system is implemented.

#include "ae_point_light_render_system.hpp"

// Standard Libraries
#include <map>

namespace ae {

    // Defines the point light push constant data.
    struct PointLightPushConstants {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

    // Constructor implementation
    PointLightRenderSystem::PointLightRenderSystem(ae_ecs::AeECS& t_ecs,
                                                   GameComponents& t_game_components,
                                                   AeDevice& t_aeDevice,
                                                   VkRenderPass t_renderPass,
                                                   VkDescriptorSetLayout t_globalSetLayout)
    : m_worldPositionComponent{t_game_components.worldPositionComponent},
      m_pointLightComponent{t_game_components.pointLightComponent},
      m_cameraComponent{t_game_components.cameraComponent},
      m_aeDevice{t_aeDevice},
      ae_ecs::AeSystem<PointLightRenderSystem>(t_ecs) {

        // Register component dependencies
        m_worldPositionComponent.requiredBySystem(this->getSystemId());
        m_pointLightComponent.requiredBySystem(this->getSystemId());

        // Register system dependencies
        // This is a child system and dependencies, as well as execution, will be handled by the parent system,
        // RendererSystem.
        this->isChildSystem = true;

        // Creates the pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
        createPipelineLayout(t_globalSetLayout);

        // Creates a graphics pipeline for this render system and sets the m_aePipeline member variable.
        createPipeline(t_renderPass);

        // Enable the system so it will run.
        this->enableSystem();
    };



    // Destructor implementation
    PointLightRenderSystem::~PointLightRenderSystem(){
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    };



    // Set up the system prior to execution. Currently not used.
    void PointLightRenderSystem::setupSystem(){};



    // Renders the point lights.
    void PointLightRenderSystem::executeSystem(VkCommandBuffer& t_commandBuffer, VkDescriptorSet t_globalDescriptorSet){

        // Get the entities that use the components this system depends on.
        std::vector<ecs_id> validEntityIds = m_systemManager.getEnabledSystemsEntities(this->getSystemId());

        // Declaring a map to sort the point lights by their distance to the camera.
        std::map<float, ecs_id> sorted_point_lights;

        // The entity ID of the main camera that the point lights need to have their light contributions calculated for.
        ecs_id mainCameraEntityId;

        // Find the main camera from all the entities that are cameras.
        for(ecs_id entityId : m_cameraComponent.getMyEntities()){
            if(m_cameraComponent.getReadOnlyDataReference(entityId).isMainCamera){
                mainCameraEntityId = entityId;
                break;
            }
        }

        // Get the world position of the main camera.
        glm::vec3 cameraPosition = m_worldPositionComponent.getWorldPositionVec3(mainCameraEntityId);

        // Loop through the point light entities and sort them by their distance to the camera.
        for (ecs_id entityId : validEntityIds){
            // Calculate the distance from the camera to the point light.
            auto offset = cameraPosition - m_worldPositionComponent.getWorldPositionVec3(entityId);

            // Square the distance to resolve minor position differences.
            float disSquared = glm::dot(offset, offset);

            // Sort the point light entity.
            sorted_point_lights[disSquared] = entityId;
        };

        // Tell the pipeline what the current command buffer being worked on is.
        m_aePipeline->bind(t_commandBuffer);

        // Bind the descriptor sets to the command buffer.
        vkCmdBindDescriptorSets(
                t_commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_pipelineLayout,
                0,
                1,
                &t_globalDescriptorSet,
                0,
                nullptr);

        // Iterate through the sorted point lights from the cameras that are the closest to the camera to the lights
        // furthest away from the camera.
        for (auto it = sorted_point_lights.rbegin(); it != sorted_point_lights.rend(); ++it) {

            // Get the entity ID of the point light.
            ecs_id entityId = it->second;

            // Initialize the point light push constants.
            PointLightPushConstants push{};

            // Get the world position of the point light.
            push.position = glm::vec4(m_worldPositionComponent.getWorldPositionVec3(entityId), 1.0f);

            // Get the point light characteristics.
            auto entityPointLightData = m_pointLightComponent.getReadOnlyDataReference(entityId);
            push.color = glm::vec4(entityPointLightData.m_color, entityPointLightData.lightIntensity);
            push.radius = entityPointLightData.radius;

            // Push the point light information to the buffer.
            vkCmdPushConstants(
                    t_commandBuffer,
                    m_pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(PointLightPushConstants),
                    &push);

            // Draw the point light.
            vkCmdDraw(t_commandBuffer, 6, 1, 0, 0);
        };
    };



    // Clean up the system after execution. Currently not used.
    void PointLightRenderSystem::cleanupSystem(){
        m_systemManager.clearSystemEntityUpdateSignatures(m_systemId);
    };



    // Creates the pipeline layout for the point light render system.
    void PointLightRenderSystem::createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout) {

        // Define the push constant information for this pipeline.
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstants);

        // Prepare the descriptor set layouts based on the global set layout for the device.
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ t_globalSetLayout };

        // Define the specific layout of the point light renderer.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Send a small amount of data to shader program

        // Attempt to create the pipeline layout, if it cannot error out.
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the point light render system's pipeline layout!");
        }

    };



    // Creates the pipeline for the point light render system.
    void PointLightRenderSystem::createPipeline(VkRenderPass t_renderPass) {

        // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
        assert(m_pipelineLayout != nullptr && "Cannot create point light render system's pipeline before pipeline layout!");

        // Define the pipeline to be created.
        GraphicsPipelineConfigInfo pipelineConfig{};
        AeGraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig);
        AeGraphicsPipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.renderPass = t_renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;

        GraphicsShaderFilesPaths shaderPaths{};
        shaderPaths.vertFilepath = "engines/graphics/shaders/point_light.vert.spv";
        shaderPaths.fragFilepath = "engines/graphics/shaders/point_light.frag.spv";

        // Create the point light render system pipeline.
        m_aePipeline = std::make_unique<AeGraphicsPipeline>(
                m_aeDevice,
                shaderPaths,
                pipelineConfig);
    };
}
