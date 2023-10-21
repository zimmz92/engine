/// \file ae_material3d.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_ecs_include.hpp"
#include "ae_image.hpp"
#include "ae_pipeline.hpp"
#include "game_components.hpp"

// libraries

//std
#include <map>


namespace ae {

    template <int requiredTextures>
    class AeMaterial3D : public ae_ecs::AeSystem<AeMaterial3D<requiredTextures>>{

    public:

        struct Model3DMaterial{
            AeMaterial3D& material3D;
            std::shared_ptr<AeImage> textures[requiredTextures];
        };

        /// Constructor of the SimpleRenderSystem
        /// \param t_game_components The game components available that this system may require.
        AeMaterial3D(ae_ecs::AeECS& t_ecs,
                     GameComponents& t_game_components,
                     AeDevice& t_aeDevice,
                     VkRenderPass t_renderPass,
                     VkDescriptorSetLayout t_globalSetLayout,
                     VkDescriptorSetLayout t_textureSetLayout,
                     VkDescriptorSetLayout t_objectSetLayout);

        /// Destructor of the SimpleRenderSystem
        ~AeMaterial3D();

        /// Setup the SimpleRenderSystem, this is handled by the RendererSystem.
        void setupSystem() override;

        /// Execute the SimpleRenderSystem, this is handled by the RendererSystem.
        void executeSystem(VkCommandBuffer& t_commandBuffer,
                           VkDescriptorSet t_globalDescriptorSet,
                           VkDescriptorSet t_textureDescriptorSet,
                           VkDescriptorSet t_objectDescriptorSet,
                           uint64_t t_frameIndex);

        /// Clean up the SimpleRenderSystem, this is handled by the RendererSystem.
        void cleanupSystem() override;

        void registerEntity(ecs_id t_entityId, AeModel& t_model3D, std::shared_ptr<AeImage> textures[requiredTextures]);

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
        void createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout,
                                  VkDescriptorSetLayout t_textureSetLayout,
                                  VkDescriptorSetLayout t_objectSetLayout);

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

        /// A vector to store the unique models, and the number of times they need to be drawn, for each frame.
        std::map<AeModel,std::map<ecs_id,uint64_t>> draws;

        /// DO NOT CALL! This is not used by this system.
        void executeSystem() override {
            throw std::runtime_error("This is the incorrect execute function for a Model3DMaterial and should "
                                     "not have been called. The Model3DMaterial execution should be being handled by "
                                     "it's parent, RenderSystem.");
        };

    protected:

    };

} // namespace ae