/// \file ae_obb_render_system.cpp
/// \brief Creates and executes the pipeline which renders an object's OBB.
/// This system creates a pipeline which specifically renders each object's oriented bounding box (OBB).

#include "ae_obb_render_system.hpp"

// Standard Libraries
#include <map>

namespace ae {

    // Constructor implementation
    ObbRenderSystem::ObbRenderSystem(ae_ecs::AeECS& t_ecs,
                                                   GameComponents& t_game_components,
                                                   AeDevice& t_aeDevice,
                                                   VkRenderPass t_renderPass,
                                                   VkDescriptorSetLayout t_globalSetLayout)
    : m_worldPositionComponent{t_game_components.worldPositionComponent},
      m_modelComponent{t_game_components.modelComponent},
      m_aeDevice{t_aeDevice},
      ae_ecs::AeSystem<ObbRenderSystem>(t_ecs) {

        // Register component dependencies
        m_worldPositionComponent.requiredBySystem(this->getSystemId());
        m_modelComponent.requiredBySystem(this->getSystemId());

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
    ObbRenderSystem::~ObbRenderSystem(){
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    };



    // Set up the system prior to execution. Currently not used.
    void ObbRenderSystem::setupSystem(){};



    // Renders the point lights.
    void ObbRenderSystem::executeSystem(VkCommandBuffer& t_commandBuffer, VkDescriptorSet t_globalDescriptorSet){

        // Get the entities that use the components this system depends on.
        std::vector<ecs_id> validEntityIds = m_systemManager.getEnabledSystemsEntities(this->getSystemId());

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
        for (auto entityId : validEntityIds) {
            auto entityModelData = m_modelComponent.getReadOnlyDataReference(entityId);
            if(entityModelData.m_model != nullptr) {
                auto entityWorldPosition = m_worldPositionComponent.getWorldPositionVec3(entityId);


                // Initialize the point light push constants.
                PushConstantData push = calculatePushConstantData(entityWorldPosition, entityModelData.rotation,
                                                                  entityModelData.scale);

                // Push the point light information to the buffer.
                vkCmdPushConstants(
                        t_commandBuffer,
                        m_pipelineLayout,
                        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                        0,
                        sizeof(PushConstantData),
                        &push);

                entityModelData.m_model->bindAabb(t_commandBuffer);

                entityModelData.m_model->drawAabb(t_commandBuffer);
            }

        };
    };



    // Clean up the system after execution. Currently not used.
    void ObbRenderSystem::cleanupSystem(){
        m_systemManager.clearSystemEntityUpdateSignatures(m_systemId);
    };



    // Creates the pipeline layout for the point light render system.
    void ObbRenderSystem::createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout) {

        // Define the push constant information for this pipeline.
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);

        // Prepare the descriptor set layouts based on the global set layout for the device.
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ t_globalSetLayout };

        // Define the specific layout of the AABB renderer.
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
    void ObbRenderSystem::createPipeline(VkRenderPass t_renderPass) {

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
        shaderPaths.vertFilepath = "engines/graphics/shaders/obb.vert.spv";
        shaderPaths.fragFilepath = "engines/graphics/shaders/obb.frag.spv";

        // Create the point light render system pipeline.
        m_aePipeline = std::make_unique<AeGraphicsPipeline>(
                m_aeDevice,
                shaderPaths,
                pipelineConfig);
    };

    // Calculates the push constant data for the specified entity
    ObbRenderSystem::PushConstantData ObbRenderSystem::calculatePushConstantData(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale) {

        // Calculate the components of the Tait-bryan angles matrix.
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 invScale = 1.0f / scale;

        // Matrix corresponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 modelMatrix = {
                {
                        scale.x * (c1 * c3 + s1 * s2 * s3),
                        scale.x * (c2 * s3),
                        scale.x * (c1 * s2 * s3 - c3 * s1),
                        0.0f,
                },
                {
                        scale.y * (c3 * s1 * s2 - c1 * s3),
                        scale.y * (c2 * c3),
                        scale.y * (c1 * c3 * s2 + s1 * s3),
                        0.0f,
                },
                {
                        scale.z * (c2 * s1),
                        scale.z * (-s2),
                        scale.z * (c1 * c2),
                        0.0f,
                },
                {
                        translation.x,
                        translation.y,
                        translation.z,
                        1.0f
                }};

        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        // Normal Matrix is calculated to facilitate non-uniform model scaling scale.x != scale.y =! scale.z
        // TODO benchmark if this is faster than just calculating the normal matrix in the shader when there are many objects.
        glm::mat3 normalMatrix = {
                {
                        invScale.x * (c1 * c3 + s1 * s2 * s3),
                        invScale.x * (c2 * s3),
                        invScale.x * (c1 * s2 * s3 - c3 * s1),
                },
                {
                        invScale.y * (c3 * s1 * s2 - c1 * s3),
                        invScale.y * (c2 * c3),
                        invScale.y * (c1 * c3 * s2 + s1 * s3),
                },
                {
                        invScale.z * (c2 * s1),
                        invScale.z * (-s2),
                        invScale.z * (c1 * c2),
                }};

        return {modelMatrix, normalMatrix};
    };
}