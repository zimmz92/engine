/// \file point_light_render_system.cpp
/// \brief The script implementing the system that renders the point lights.
/// The cycle point light system is implemented.

#include "ae_simple_render_system.hpp"

// Standard Libraries
#include <map>

namespace ae {

    // Constructor implementation
    SimpleRenderSystem::SimpleRenderSystem(ae_ecs::AeECS& t_ecs, GameComponents &t_game_components, AeDevice &t_aeDevice,
                                           VkRenderPass t_renderPass, VkDescriptorSetLayout t_globalSetLayout)
            : m_worldPositionComponent{t_game_components.worldPositionComponent},
              m_modelComponent{t_game_components.modelComponent},
              m_aeDevice{t_aeDevice},
              ae_ecs::AeSystem<SimpleRenderSystem>(t_ecs) {

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
    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    };


    // Set up the system prior to execution. Currently not used.
    void SimpleRenderSystem::setupSystem() {};


    // Renders the point lights.
    void SimpleRenderSystem::executeSystem(VkCommandBuffer &t_commandBuffer, VkDescriptorSet t_globalDescriptorSet) {

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


        // Get the entities that use the components this system depends on.
        std::vector<ecs_id> validEntityIds = m_systemManager.getValidEntities(this->getSystemId());


        // Loop through the entities if they have models render them.
        for (ecs_id entityId: validEntityIds) {
            // Get the world position and model of the entity
            glm::vec3 entityWorldPosition = m_worldPositionComponent.getWorldPositionVec3(entityId);
            ModelComponentStruct& entityModelData = m_modelComponent.getDataReference(entityId);

            // Make sure the entity actually has a model to render.
            if (entityModelData.m_model == nullptr) continue;

            // Get the simple render system push constants for the current entity.
            SimplePushConstantData push{calculatePushConstantData(entityWorldPosition,
                                                                  entityModelData.rotation,
                                                                  entityModelData.scale)};

            vkCmdPushConstants(t_commandBuffer, m_pipelineLayout,
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                               sizeof(SimplePushConstantData), &push);

            entityModelData.m_model->bind(t_commandBuffer);
            entityModelData.m_model->draw(t_commandBuffer);
        };
    };


    // Clean up the system after execution. Currently not used.
    void SimpleRenderSystem::cleanupSystem() {};


    // Creates the pipeline layout for the point light render system.
    void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout) {

        // Define the push constant information for this pipeline.
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        // Prepare the descriptor set layouts based on the global set layout for the device.
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{t_globalSetLayout};

        // Define the specific layout of the point light renderer.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Send a small amount of data to shader program

        // Attempt to create the pipeline layout, if it cannot error out.
        if (vkCreatePipelineLayout(m_aeDevice.device(),
                                   &pipelineLayoutInfo,
                                   nullptr,
                                   &m_pipelineLayout)
                                   != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the simple render system's pipeline layout!");
        }

    };


    // Creates the pipeline for the point light render system.
    void SimpleRenderSystem::createPipeline(VkRenderPass t_renderPass) {

        // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
        assert(m_pipelineLayout != nullptr &&
               "Cannot create the simple render system's pipeline before pipeline layout!");

        // Define the pipeline to be created.
        PipelineConfigInfo pipelineConfig{};
        AePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = t_renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;

        // Create the point light render system pipeline.
        m_aePipeline = std::make_unique<AePipeline>(
                m_aeDevice,
                "engines/graphics/shaders/shader.vert.spv",
                "engines/graphics/shaders/shader.frag.spv",
                pipelineConfig);
    };

    // Calculates the push constant data for the specified entity
    SimplePushConstantData SimpleRenderSystem::calculatePushConstantData(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale) {

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
        glm::mat4 rotationMatrix = {
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

        return {rotationMatrix, normalMatrix};
    };
}