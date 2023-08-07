/*! \file renderer_start_pass_system.hpp
    \brief The script defining the renderer start pass system.
    The renderer start pass system is defined.
*/
#pragma once

#include "ae_ecs.hpp"

#include "game_components.hpp"
#include "update_ubo_system.hpp"
#include "timing_system.hpp"

#include "ae_renderer.hpp"
#include "ae_device.hpp"
#include "ae_buffer.hpp"
#include "ae_swap_chain.hpp"
#include "ae_descriptors.hpp"

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
        RendererStartPassSystem(GameComponentsStruct& t_game_components,
                                UpdateUboSystem& t_updateUboSystem,
                                TimingSystem& t_timingSystem,
                                AeRenderer& t_renderer,
                                AeDevice& t_aeDevice);

        /// Destructor of the RendererStartPassSystem
        ~RendererStartPassSystem();

        /// Setup the RendererStartPassSystem, this is handled by the ECS.
        void setupSystem() override;

        /// Execute the RendererStartPassSystem, this is handled by the ECS.
        void executeSystem() override;

        /// Clean up the RendererStartPassSystem, this is handled by the ECS.
        void cleanupSystem() override;

    private:

        // Components this system utilizes.
        // None currently.

        // Prerequisite systems for the PlayerInputSystem.
        /// The UpdateUboSystem this system requires to operate prior to it's own operation.
        UpdateUboSystem& m_updateUboSystem;
        /// The TimingSystem this system requires to operate prior to it's own operation.
        TimingSystem& m_timingSystem;



        /// Reference to the game renderer
        AeRenderer& m_renderer; // TODO: Make this an entity of it's own?

        /// Reference to the game graphics device
        AeDevice& m_aeDevice;

        /// The global poo;
        std::unique_ptr<AeDescriptorPool> m_globalPool;

        /// The global descriptor sets used for each of the allowed frames in flight.
        std::vector<VkDescriptorSet> m_globalDescriptorSets;

        /// The UBO buffer objects for each frame in flight
        std::vector<std::unique_ptr<AeBuffer>> m_uboBuffers;

        /// The frame index for the current render pass.
        int m_frameIndex;

        /// Command buffer for the current render pass
        VkCommandBuffer_T* m_commandBuffer;

        /// The global descriptor set for the current frame
        VkDescriptorSet m_globalDescriptorSet;

    };
}