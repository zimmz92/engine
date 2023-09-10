/// \file ae_ui_render_system.hpp
/// \brief The script defining the system that renders user interfaces.
/// The user interface system is defined.
#pragma once

#include "ae_ecs_include.hpp"

#include "game_components.hpp"

#include "ae_device.hpp"
#include "ae_pipeline.hpp"


namespace ae {

    /// Defines the simple render system push constant data.
    struct UiPushConstantData {
        /// The scaling and rotation of the 2D object.
        glm::mat2 transform{ 1.0f };

        /// The offset of the 2D object.
        glm::vec2 translation{ 1.0f };
    };

    /// A child system of the RendererSystem which renders the entity models.
    class UiRenderSystem : public ae_ecs::AeSystem<UiRenderSystem> {
    public:
        /// Constructor of the SimpleRenderSystem
        /// \param t_ecs The entity component system this render system will run on.
        /// \param t_game_components The game components available that this system may require.
        /// \param t_aeDevice The device this render system is to be implemented on.
        /// \param t_renderPass The current render pass that will be used to execute this system.
        /// \param t_globalSetLayout The layout of the set of information that is common to all render systems.
        /// \param t_textureSetLayout The descriptor set layout for the textures and materials used by a model.
        UiRenderSystem(ae_ecs::AeECS& t_ecs, GameComponents& t_game_components, AeDevice& t_aeDevice, VkRenderPass t_renderPass, VkDescriptorSetLayout t_globalSetLayout, VkDescriptorSetLayout t_textureSetLayout);

        /// Destructor of the SimpleRenderSystem
        ~UiRenderSystem();

        /// Setup the SimpleRenderSystem, this is handled by the RendererSystem.
        void setupSystem() override;

        /// Execute the SimpleRenderSystem, this is handled by the RendererSystem.
        void executeSystem(VkCommandBuffer& t_commandBuffer, VkDescriptorSet t_globalDescriptorSet);

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
        /// The ModelComponent this system accesses to render the entity in the game world.
        Model2dComponent& m_model2DComponent;


        // Prerequisite systems for the SimpleRenderSystem.
        // This requires any world position updating system to run before this system runs.
        // This needs to be run after the point light render system to maintain transparency/render it in front.


        /// Creates the pipeline layout for the SimpleRenderSystem.
        /// \param t_globalSetLayout The general descriptor set for the devices and general rendering setting that need
        /// \param t_textureSetLayout The descriptor set layout for the textures and materials used by a model.
        /// to be accounted for when setting up the render pipeline for this system.
        void createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout, VkDescriptorSetLayout t_textureSetLayout);

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
        /// \param t_translation The translation data for the entity.
        /// \param t_rotation The rotation of the entity.
        /// \param t_scale The scaling for the entity.
        UiPushConstantData calculatePushConstantData(glm::vec2 t_translation, float t_rotation, glm::vec2 t_scale);
    };
}


