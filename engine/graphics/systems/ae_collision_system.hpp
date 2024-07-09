/// \file ##.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_ecs_include.hpp"
#include "ae_engine_constants.hpp"
#include "ae_device.hpp"
#include "ae_compute_pipeline.hpp"
#include "ae_graphics_pipeline.hpp"
#include "ae_buffer.hpp"
#include "ae_renderer.hpp"
#include "game_components.hpp"

// libraries

// std


namespace ae {

    class AeCollisionSystem : public ae_ecs::AeSystem<AeCollisionSystem> {
    public:

        AeCollisionSystem(ae_ecs::AeECS& t_ecs,
                          GameComponents& t_game_components,
                          AeDevice& t_aeDevice,
                          VkRenderPass t_renderPass,
                          std::vector<VkDescriptorSetLayout> t_computeDescriptorSetLayouts,
                          std::vector<VkDescriptorSetLayout> t_aabbDescriptorSetLayouts);

        ~AeCollisionSystem();

        VkPipelineLayout& getComputePipelineLayout(){return m_computePipelineLayout;};

        void bindComputePipeline(VkCommandBuffer& t_commandBuffer);

        void drawAABBs(VkCommandBuffer &t_commandBuffer,
                       std::vector<VkDescriptorSet>& t_descriptorSets);

        void drawOBBs(VkCommandBuffer &t_commandBuffer,
                      std::vector<VkDescriptorSet>& t_descriptorSets);

        void recordComputeCommandBuffer(VkCommandBuffer& t_commandBuffer, std::vector<VkDescriptorSet>& t_descriptorSets);

        /// Setup the PointLightRenderSystem, this is handled by the RendererSystem.
        void setupSystem() override {
            throw std::runtime_error("This is the incorrect execute function for the AeCollisionSystem and should "
                                     "not have been called. The AeCollisionSystem execution should be being handled by "
                                     "it's parent, RenderSystem.");
        };

        /// DO NOT CALL! This is not used by this system.
        void executeSystem() override {
            throw std::runtime_error("This is the incorrect execute function for the AeCollisionSystem and should "
                                     "not have been called. The AeCollisionSystem execution should be being handled by "
                                     "it's parent, RenderSystem.");
        };

        /// Clean up the PointLightRenderSystem, this is handled by the RendererSystem.
        void cleanupSystem() override {
            throw std::runtime_error("This is the incorrect execute function for the AeCollisionSystem and should "
                                     "not have been called. The AeCollisionSystem execution should be being handled by "
                                     "it's parent, RenderSystem.");
        };

    private:

        // Components this system utilizes.
        /// The WorldPositionComponent this systems accesses to know where a point light to render it.
        WorldPositionComponent& m_worldPositionComponent;
        /// The ModelComponent this system accesses to render the entity in the game world.
        ModelComponent& m_modelComponent;

        /// Creates the pipeline layout for the point light render system.
        /// \param t_globalSetLayout The general descriptor set for the devices and general rendering setting that need
        /// to be accounted for when setting up the render pipeline for this system.
        void createComputePipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts);

        /// Creates the pipeline based on the render pass this pipeline should be associated with.
        void createComputePipeline();


        void createAabbPipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts);

        void createAabbPipeline(VkRenderPass t_renderPass);

        void createObbPipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts);

        void createObbPipeline(VkRenderPass t_renderPass);

        /// The graphics device this render system will be using.
        AeDevice& m_aeDevice;

        /// The layout of this render system's pipeline.
        VkPipelineLayout m_aabbPipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AeGraphicsPipeline> m_aabbPipeline;

        /// The layout of this render system's pipeline.
        VkPipelineLayout m_obbPipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AeGraphicsPipeline> m_obbPipeline;

        /// The layout of this render system's pipeline.
        VkPipelineLayout m_computePipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AeComputePipeline> m_aeComputePipeline;

    protected:

    };

} // namespace ae