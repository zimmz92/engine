/*! \file point_light_render_system.hpp
    \brief The script defining the system that renders the point lights.
    The render point light system is defined.
*/
#pragma once

#include "ae_ecs.hpp"

#include "game_components.hpp"

#include "ae_device.hpp"
#include "ae_pipeline.hpp"


namespace ae {

    /// Defines the simple render system push constant data.
    struct SimplePushConstantData {
        // Matrix corresponds to WorldPosition * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 modelMatrix{ 1.0f };

        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 normalMatrix{ 1.0f };
    };

    /// A child system of the RendererSystem which renders the entity models.
    class SimpleRenderSystem : public ae_ecs::AeSystem<SimpleRenderSystem> {
    public:
        /// Constructor of the SimpleRenderSystem
        /// \param t_game_components The game components available that this system may require.
        SimpleRenderSystem(GameComponentsStruct& t_game_components, AeDevice& t_aeDevice, VkRenderPass t_renderPass, VkDescriptorSetLayout t_globalSetLayout);

        /// Destructor of the SimpleRenderSystem
        ~SimpleRenderSystem();

        /// Setup the SimpleRenderSystem, this is handled by the RendererSystem.
        void setupSystem() override;

        /// Execute the SimpleRenderSystem, this is handled by the RendererSystem.
        void executeSystem(VkCommandBuffer& t_commandBuffer, VkDescriptorSet& t_globalDescriptorSet);

        /// DO NOT CALL! This is not used by this system.
        void executeSystem() override {
            throw std::runtime_error("This is the incorrect execute function for the PointLightRenderSystem and should "
                                     "not have been called. The PointLightRenderSystem execution should be being handled by "
                                     "it's parent, RenderSystem.");
        };

        /// Clean up the SimpleRenderSystem, this is handled by the RendererSystem.
        void cleanupSystem() override;

    private:

        // Components this system utilizes.
        /// The WorldPositionComponent this systems accesses to know where an entity is to render it.
        WorldPositionComponent& m_worldPositionComponent;
        /// The ModelComponent this system accesses to render the entity in the game world.
        ModelComponent& m_modelComponent;


        // Prerequisite systems for the SimpleRenderSystem.
        // This requires any world position updating system to run before this system runs.
        // This needs to be run before the point light render system to maintain transparency.


        /// Creates the pipeline layout for the SimpleRenderSystem.
        /// \param t_globalSetLayout The general descriptor set for the devices and general rendering setting that need
        /// to be accounted for when setting up the render pipeline for this system.
        void createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout);

        /// Creates the pipeline based on the render pass this pipeline should be associated with for the
        /// SimpleRenderSystem.
        /// \param t_renderPass The render pass this pipeline should be associated with.
        void createPipeline(VkRenderPass t_renderPass);


        /// The graphics device this render system will be using.
        AeDevice& m_aeDevice;

        /// The layout of this render system's pipeline.
        VkPipelineLayout m_pipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AePipeline> m_aePipeline;



        /// Calculates the push constant data for a specified entity.
        /// \param t_translation The translation data for the entity, this normally corresponds to world position but
        /// could be the world position plus an additional offset.
        /// \param t_rotation The rotation of the entity, typically the direction the entity is facing.
        /// \param t_scale The scaling for the entity's model.
        SimplePushConstantData calculatePushConstantData(glm::vec3 t_translation, glm::vec3 t_rotation, glm::vec3 t_scale);
    };
}

