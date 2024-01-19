/*! \file point_light_render_system.hpp
    \brief The script defining the system that renders the point lights.
    The render point light system is defined.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "ae_graphics_constants.hpp"

#include "game_components.hpp"

#include "ae_device.hpp"
#include "ae_pipeline.hpp"


namespace ae {

    /// A child system of the RendererSystem which renders point lights.
    class PointLightRenderSystem : public ae_ecs::AeSystem<PointLightRenderSystem> {
    public:
        /// Constructor of the PointLightRenderSystem
        /// \param t_game_components The game components available that this system may require.
        PointLightRenderSystem(ae_ecs::AeECS& t_ecs, GameComponents& t_game_components, AeDevice& t_aeDevice, VkRenderPass t_renderPass, VkDescriptorSetLayout t_globalSetLayout);

        /// Destructor of the PointLightRenderSystem
        ~PointLightRenderSystem();

        /// Setup the PointLightRenderSystem, this is handled by the RendererSystem.
        void setupSystem() override;

        /// Execute the PointLightRenderSystem, this is handled by the RendererSystem.
        /// \param t_commandBuffer The command buffer for the current frame being rendered.
        /// \param t_globalDescriptorSet The global descriptor set for the current frame being rendered.
        void executeSystem(VkCommandBuffer& t_commandBuffer, VkDescriptorSet t_globalDescriptorSet);

        /// DO NOT CALL! This is not used by this system.
        void executeSystem() override {
            throw std::runtime_error("This is the incorrect execute function for the PointLightRenderSystem and should "
                                     "not have been called. The PointLightRenderSystem execution should be being handled by "
                                     "it's parent, RenderSystem.");
        };

        /// Clean up the PointLightRenderSystem, this is handled by the RendererSystem.
        void cleanupSystem() override;

    private:

        // Components this system utilizes.
        /// The WorldPositionComponent this systems accesses to know where a point light to render it.
        WorldPositionComponent& m_worldPositionComponent;
        /// The PointLightComponent this systems accesses to obtain properties of the point light for rendering.
        PointLightComponent& m_pointLightComponent;
        /// The CameraComponent this system accesses to find the main camera for calculating point light distance.
        CameraComponent& m_cameraComponent;


        // Prerequisite systems for the PointLightRenderSystem. These are handled by the RendererSystem
        // This requires the CameraUpdateSystem and point light updating systems to run before this system executes.


        /// Creates the pipeline layout for the point light render system.
        /// \param t_globalSetLayout The general descriptor set for the devices and general rendering setting that need
        /// to be accounted for when setting up the render pipeline for this system.
        void createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout);

        /// Creates the pipeline based on the render pass this pipeline should be associated with.
        /// \param t_renderPass The render pass this pipeline should be associated with.
        void createPipeline(VkRenderPass t_renderPass);

        /// The graphics device this render system will be using.
        AeDevice& m_aeDevice;

        /// The layout of this render system's pipeline.
        VkPipelineLayout m_pipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AeGraphicsPipeline> m_aePipeline;

    };
}


