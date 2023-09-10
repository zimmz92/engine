/// \file renderer_start_pass_system.cpp
/// \brief The script implementing the renderer start pass system class.
/// The renderer start pass system class is implemented.

#include "ae_renderer_system.hpp"

namespace ae {
    // Constructor of the RendererStartPassSystem
    RendererStartPassSystem::RendererStartPassSystem(ae_ecs::AeECS& t_ecs,
                                                     GameComponents& t_game_components,
                                                     UpdateUboSystem& t_updateUboSystem,
                                                     TimingSystem& t_timingSystem,
                                                     AeRenderer& t_renderer,
                                                     AeDevice& t_aeDevice) :
                                                     m_updateUboSystem{t_updateUboSystem},
                                                     m_timingSystem{t_timingSystem},
                                                     m_renderer{t_renderer},
                                                     m_aeDevice{t_aeDevice},
                                                     ae_ecs::AeSystem<RendererStartPassSystem>(t_ecs)  {

        // Register component dependencies
        // None currently.


        // Register system dependencies
        this->dependsOnSystem(m_updateUboSystem.getSystemId());
        this->dependsOnSystem(m_timingSystem.getSystemId());

        // Create the global pool
        m_globalPool = AeDescriptorPool::Builder(m_aeDevice)
                .setMaxSets(AeSwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, AeSwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,AeSwapChain::MAX_FRAMES_IN_FLIGHT)
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

            // Attempt to create the pipeline layout, if it cannot error out.
            if ( m_uboBuffers.back()->map() != VK_SUCCESS) {
                throw std::runtime_error("Failed to map the ubo buffer to memory!");
            };
        };

        // Get the layout of the device and specify some general rendering options for all render systems.
        auto globalSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                .build();

        // Define the descriptor set for the texture.
        auto textureSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();

        // Reserve space for and then initialize the global descriptors for each frame.
        m_globalDescriptorSets.reserve(AeSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < AeSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
            // Get the buffer information from the uboBuffers.
            auto bufferInfo = m_uboBuffers[i]->descriptorInfo();

            // Initialize the descriptor set for the current frame.
            AeDescriptorWriter(*globalSetLayout, *m_globalPool)
                    .writeBuffer(0, &bufferInfo)
                    .build(m_globalDescriptorSets[i]);
        }


        // Setup child render systems
        m_simpleRenderSystem = new SimpleRenderSystem(t_ecs,
                                                      t_game_components,
                                                      m_aeDevice,
                                                      m_renderer.getSwapChainRenderPass(),
                                                      globalSetLayout->getDescriptorSetLayout());
        m_pointLightRenderSystem = new PointLightRenderSystem(t_ecs,
                                                              t_game_components,
                                                              m_aeDevice,
                                                              m_renderer.getSwapChainRenderPass(),
                                                              globalSetLayout->getDescriptorSetLayout());

        m_uiRenderSystem = new UiRenderSystem(t_ecs,
                                              t_game_components,
                                              m_aeDevice,
                                              m_renderer.getSwapChainRenderPass(),
                                              globalSetLayout->getDescriptorSetLayout(),
                                              textureSetLayout->getDescriptorSetLayout());


        // Enable the system
        this->enableSystem();

    };



    // Destructor class of the RendererStartPassSystem
    RendererStartPassSystem::~RendererStartPassSystem(){
        delete m_simpleRenderSystem;
        m_simpleRenderSystem = nullptr;

        delete m_pointLightRenderSystem;
        m_pointLightRenderSystem = nullptr;

        delete m_uiRenderSystem;
        m_uiRenderSystem = nullptr;

    };



    // There is no setup required for this system to execute.
    void RendererStartPassSystem::setupSystem(){};



    //
    void RendererStartPassSystem::executeSystem(){

        // Unless the renderer is not ready to begin a new frame and returns a nullptr start rendering a new frame.
        if ((m_commandBuffer = m_renderer.beginFrame())) {

            // Get the frame index for the frame the render pass is starting for.
            m_frameIndex = m_renderer.getFrameIndex();

            // Write the ubo data for the shaders for this frame.
            m_uboBuffers[m_frameIndex]->writeToBuffer(m_updateUboSystem.getUbo());
            m_uboBuffers[m_frameIndex]->flush();

            // Start the render pass.
            m_renderer.beginSwapChainRenderPass(m_commandBuffer);

            // Call subservient render systems. Order matters here to maintain object transparencies.
            m_simpleRenderSystem->executeSystem(m_commandBuffer,m_globalDescriptorSets[m_frameIndex]);
            m_pointLightRenderSystem->executeSystem(m_commandBuffer,m_globalDescriptorSets[m_frameIndex]);
            m_uiRenderSystem->executeSystem(m_commandBuffer,m_globalDescriptorSets[m_frameIndex]);

            // End the render pass and the frame.
            m_renderer.endSwapChainRenderPass(m_commandBuffer);
            m_renderer.endFrame();
        };
    };



    // There is no clean up required after this system executes.
    void RendererStartPassSystem::cleanupSystem(){};

}