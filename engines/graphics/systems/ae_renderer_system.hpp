/*! \file renderer_start_pass_system.hpp
    \brief The script defining the renderer start pass system.
    The renderer start pass system is defined.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "ae_graphics_constants.hpp"

#include "game_components.hpp"
#include "update_ubo_system.hpp"
#include "timing_system.hpp"

#include "ae_renderer.hpp"
#include "ae_device.hpp"
#include "ae_buffer.hpp"
#include "ae_swap_chain.hpp"
#include "ae_descriptors.hpp"
#include "ae_image.hpp"
#include "ae_samplers.hpp"

#include "game_materials.hpp"
#include "ae_model_3d_buffer_system.hpp"

#include "ae_simple_render_system.hpp"
#include "ae_point_light_render_system.hpp"
#include "ae_ui_render_system.hpp"
#include "ae_particle_system.hpp"
#include "ae_aabb_render_system.hpp"

#include "pre_allocated_stack.hpp"

namespace ae {

    /// If the swap chain does not need to be recreated this system will start the render pass, compile the frame
    /// information, push the ubo information to the shaders, and initiates the swap chain render pass.
    class RendererStartPassSystem : public ae_ecs::AeSystem<RendererStartPassSystem> {
    public:

        /// Constructor of the RendererStartPassSystem
        /// \param t_game_components The game components available that this system may require.
        /// \param t_updateUboSystem The UpdateUboSystem the RendererStartPassSystem depends on executing to ensure up
        /// to date ubo information is being pushed to the shaders.
        /// \param t_timingSystem The TimingSystem the RendererStartPassSystem will depend on executing first
        /// and require information from.
        /// \param t_renderer The Ae_Renderer that this system will interact with to initiate the render pass.
        /// \param t_globalDescriptorSets The global descriptor sets for this class to utilize.
        /// \param t_uboBuffers The ubo buffers this class utilizes.
        RendererStartPassSystem(ae_ecs::AeECS& t_ecs,
                                GameComponents& t_game_components,
                                UpdateUboSystem& t_updateUboSystem,
                                TimingSystem& t_timingSystem,
                                AeRenderer& t_renderer,
                                AeDevice& t_aeDevice,
                                AeSamplers& t_aeSamplers);

        /// Destructor of the RendererStartPassSystem
        ~RendererStartPassSystem();

        /// Setup the RendererStartPassSystem, this is handled by the ECS.
        void setupSystem() override;

        /// Execute the RendererStartPassSystem, this is handled by the ECS.
        void executeSystem() override;

        /// Clean up the RendererStartPassSystem, this is handled by the ECS.
        void cleanupSystem() override;

        /// Returns a reference to the materials available while using this renderer
        GameMaterials& getGameMaterials(){return *m_gameMaterials;}

    private:

        void updateParticleDescriptorSets();

        // Components this system utilizes.
        // None currently.

        // Prerequisite systems for the PlayerInputSystem.
        /// The UpdateUboSystem this system requires to operate prior to it's own operation.
        UpdateUboSystem& m_updateUboSystem;
        /// The TimingSystem this system requires to operate prior to it's own operation.
        TimingSystem& m_timingSystem;

        /// Updates the texture descriptor set for the current frame being rendered.
        void updateDescriptorSets();

        /// Reference to the game renderer
        AeRenderer& m_renderer; // TODO: Make this an entity of it's own?

        /// Reference to the game graphics device
        AeDevice& m_aeDevice;

        /// The frame index for the current render pass.
        int m_frameIndex;

        /// Graphics command buffer for the current render pass
        VkCommandBuffer m_graphicsCommandBuffer;

        /// Compute command buffer for the current render pass
        VkCommandBuffer m_computeCommandBuffer;

        /// The samplers the render system utilizes.
        AeSamplers& m_aeSamplers;

        /// Component Systems that might be required
        /// TODO Improve the texture evaluation system so that this is not needed.
        GameComponents& m_gameComponents;

        //==============================================================================================================
        // Descriptor Pool
        //==============================================================================================================
        /// The global pool;
        std::unique_ptr<AeDescriptorPool> m_globalPool;

        /// The global descriptor sets used for each of the allowed frames in flight.
        std::vector<VkDescriptorSet> m_globalDescriptorSets;


        //==============================================================================================================
        // UBO
        //==============================================================================================================
        /// The UBO buffer objects for each frame in flight
        std::vector<std::unique_ptr<AeBuffer>> m_uboBuffers;

        //==============================================================================================================
        // Textures Image Buffer
        //==============================================================================================================
        /// The texture descriptor sets used for each of the allowed frames in flight.
        std::vector<VkDescriptorSet> m_textureDescriptorSets;

        /// The texture descriptor writers.
        AeDescriptorWriter* m_textureDescriptorWriter;

        /// The default image for use with the render system.
        std::shared_ptr<AeImage> m_defaultImage;

        /// Stores images for each frame that are used during rendering.
        VkDescriptorImageInfo m_imageBufferData[MAX_TEXTURES];

        /// A stack to track the available texture buffer data indexes.
        PreAllocatedStack<uint64_t,MAX_TEXTURES> m_imageBufferDataIndexStack{};

        /// Stores the image buffer and entity relation for a specific image.
        std::map<std::shared_ptr<AeImage>,ImageBufferInfo> m_imageBufferEntityMaterialMap{};

        //==============================================================================================================
        // Compute Stage
        //==============================================================================================================
        /// The compute descriptor set.
        std::vector<VkDescriptorSet> m_computesDescriptorSets;

        /// The object buffers for each frame
        std::vector<std::unique_ptr<AeBuffer>> m_computeBuffers;

        /// The texture descriptor writers.
        AeDescriptorWriter* m_computeDescriptorWriter;

        //==============================================================================================================
        // Particle System
        //==============================================================================================================
        AeParticleSystem* m_particleSystem;
        std::vector<std::vector<VkDescriptorSet>> m_particleFrameDescriptorSets;


        //==============================================================================================================
        // 3D Object Buffer
        //==============================================================================================================
        /// The object descriptor sets used for storing the model matrices and texture indexes.
        std::vector<VkDescriptorSet> m_object3DDescriptorSets;

        /// The object buffers for each frame
        std::vector<std::unique_ptr<AeBuffer>> m_object3DBuffers;


        /// The object descriptor sets used for storing the model matrices and texture indexes.
        std::vector<VkDescriptorSet> m_object3DDescriptorSetsIndirect;

        /// The object buffers for each frame
        std::vector<std::unique_ptr<AeBuffer>> m_object3DBuffersIndirect;

        /// Stores all the entity specific data for 3D models required for rendering a specific frame.
        std::vector<Entity3DSSBOData> m_object3DBufferData = std::vector<Entity3DSSBOData>(MAX_OBJECTS, Entity3DSSBOData());

        /// Maps entities to their model matrix/texture data in the 3D SSBO.
        std::map<ecs_id, uint32_t> m_object3DBufferEntityMap= std::map<ecs_id, uint32_t>{};

        /// A stack to track the available data positions in the SSBO.
        PreAllocatedStack<uint64_t,MAX_OBJECTS> m_object3DBufferDataIndexStack{};


        //==============================================================================================================
        // 2D Object Buffer
        //==============================================================================================================
        /// The object descriptor sets used for storing the model matrices and texture indexes.
        std::vector<VkDescriptorSet> m_object2DDescriptorSets;

        /// The object buffers for each frame
        std::vector<std::unique_ptr<AeBuffer>> m_object2DBuffers;


        //==============================================================================================================
        // Draw Indirect buffer
        //==============================================================================================================
        /// The draw indirect buffer descriptor sets used for draw indirect commands.
        std::vector<VkDescriptorSet> m_drawIndirectDescriptorSets;

        /// The draw indirect command buffer objects for each frame in flight
        std::vector<std::unique_ptr<AeBuffer>> m_drawIndirectBuffers;

        //==============================================================================================================
        // Descriptor Sets
        //==============================================================================================================
        std::vector<std::vector<VkDescriptorSet>> m_frameDescriptorSets;

        std::vector<std::vector<VkDescriptorSet>> m_frameDescriptorSetsOLD;

        //==============================================================================================================
        // Child render systems
        //==============================================================================================================
        /// The materials available to objects to define how they appear when rendered.
        GameMaterials* m_gameMaterials;

        /// Stores the material component IDs for quick reference.
        std::vector<ecs_id> m_materialComponentIds;

        /// A system that compiles the model matrix data for entities that can be rendered.
        AeModel3DBufferSystem* m_model3DBufferSystem;

        /// Pointer to the point light render system
        PointLightRenderSystem* m_pointLightRenderSystem;

        /// Pointer to the simple render system
        SimpleRenderSystem* m_simpleRenderSystem;

        /// Pointer to the UI render system.
        UiRenderSystem* m_uiRenderSystem;

        /// AABB Render System for debug
        AabbRenderSystem* m_aabbRenderSystem;

    };
}