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

#include "ae_simple_render_system.hpp"
#include "ae_point_light_render_system.hpp"
#include "ae_ui_render_system.hpp"

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

        /// Command buffer for the current render pass
        VkCommandBuffer m_commandBuffer;

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

        /// Create a default image for use with the render system.
        std::shared_ptr<AeImage> m_defaultImage;

        //==============================================================================================================
        // 3D Object Buffer
        //==============================================================================================================
        /// The object descriptor sets used for storing the model matrices and texture indexes.
        std::vector<VkDescriptorSet> m_objectDescriptorSets;

        /// The object buffers for each frame
        std::vector<std::unique_ptr<AeBuffer>> m_objectBuffers;

        //==============================================================================================================
        // 2D Object Buffer
        //==============================================================================================================
        /// The object descriptor sets used for storing the model matrices and texture indexes.
        std::vector<VkDescriptorSet> m_object2DDescriptorSets;

        /// The object buffers for each frame
        std::vector<std::unique_ptr<AeBuffer>> m_object2DBuffers;

        //==============================================================================================================
        // Child render systems
        //==============================================================================================================
        GameMaterials* m_gameMaterials;

        /// Pointer to the point light render system
        PointLightRenderSystem* m_pointLightRenderSystem;

        /// Pointer to the simple render system
        SimpleRenderSystem* m_simpleRenderSystem;

        /// Pointer to the UI render system.
        UiRenderSystem* m_uiRenderSystem;

    };
}