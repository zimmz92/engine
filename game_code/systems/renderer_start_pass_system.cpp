/// \file renderer_start_pass_system.cpp
/// \brief The script implementing the renderer start pass system class.
/// The renderer start pass system class is implemented.

#include "renderer_start_pass_system.hpp"

namespace ae {
    // Constructor of the RendererStartPassSystem
    RendererStartPassSystem::RendererStartPassSystem(GameComponentsStruct& t_game_components,
                                                     UpdateUboSystem& t_updateUboSystem,
                                                     TimingSystem& t_timingSystem,
                                                     AeRenderer& t_renderer,
                                                     AeDevice& t_aeDevice) :
                                                     m_updateUboSystem{t_updateUboSystem},
                                                     m_timingSystem{t_timingSystem},
                                                     m_renderer{t_renderer},
                                                     m_aeDevice{t_aeDevice},
                                                     ae_ecs::AeSystem<RendererStartPassSystem>()  {

        // Register component dependencies
        // None currently.


        // Register system dependencies
        this->dependsOnSystem(m_updateUboSystem.getSystemId());
        this->dependsOnSystem(m_timingSystem.getSystemId());

        // Create the global pool
        m_globalPool = AeDescriptorPool::Builder(m_aeDevice)
                .setMaxSets(AeSwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, AeSwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();

        // Initialize the ubo buffers
        m_uboBuffers.reserve(AeSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < AeSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
            // Create a new buffer and push it to the back of the vector of uboBuffers.
            m_uboBuffers.push_back(std::make_unique<AeBuffer>(
                    m_aeDevice,
                    sizeof(GlobalUbo),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

            // Map this buffer to memory on the devices.
            m_uboBuffers.back()->map();
            //TODO: Check if the mapping was successful
        };

        // Get the layout of the device and specify some general rendering options for all render systems.
        auto globalSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                .build();

        // Reserve space for and then initialize the global descriptors for each frame.
        m_globalDescriptorSets.reserve(AeSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < AeSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
            // Get the buffer information from the uboBuffers.
            auto bufferInfo = m_uboBuffers[i]->descriptorInfo();

            // Build the descriptor set for the current frame.
            AeDescriptorWriter(*globalSetLayout, *m_globalPool)
                    .writeBuffer(0, &bufferInfo)
                    .build(m_globalDescriptorSets[i]);
        }

        // Enable the system
        this->enableSystem();

    };



    // Destructor class of the RendererStartPassSystem
    RendererStartPassSystem::~RendererStartPassSystem(){};



    // There is no setup required for this system to execute.
    void RendererStartPassSystem::setupSystem(){};



    //
    void RendererStartPassSystem::executeSystem(){

        // Unless the renderer is not ready to begin a new frame and returns a nullptr start rendering a new frame.
        if ((m_commandBuffer = m_renderer.beginFrame())) {

            // Get the frame index for the frame the render pass is starting for.
            m_frameIndex = m_renderer.getFrameIndex();

            // Get the global descriptor set for this frame.
            m_globalDescriptorSet = m_globalDescriptorSets[m_frameIndex];

            // Write the ubo data for the shaders for this frame
            m_uboBuffers[m_frameIndex]->writeToBuffer(m_updateUboSystem.getUbo());
            m_uboBuffers[m_frameIndex]->flush();

            // Start the render pass
            m_renderer.beginSwapChainRenderPass(m_commandBuffer);
        };
    };



    // There is no clean up required after this system executes.
    void RendererStartPassSystem::cleanupSystem(){};

}