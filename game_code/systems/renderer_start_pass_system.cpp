/// \file camera_update_system.cpp
/// \brief The script implementing the camera update system class.
/// The camera update system class is implemented.

#include "renderer_start_pass_system.hpp"

#include "ae_renderer.hpp"
#include "ae_frame_info.hpp"

namespace ae {
    // Constructor of the StartRenderPassSystemClass
    RendererStartPassSystem::RendererStartPassSystem(GameComponents* t_game_components,
                                                           CameraUpdateSystemClass* t_cameraUpdateSystem,
                                                           TimingSystem* t_timingSystem,
                                                           AeRenderer* t_renderer)
                                                     : ae_ecs::AeSystem<RendererStartPassSystem>()  {
        m_renderer = t_renderer;
        m_game_components = t_game_components;
        m_cameraUpdateSystem = t_cameraUpdateSystem;
        m_timingSystem = t_timingSystem;

        // Register component dependencies
        m_game_components->worldPositionComponent.requiredBySystem(this->getSystemId());
        m_game_components->modelComponent.requiredBySystem(this->getSystemId());
        m_game_components->cameraComponent.requiredBySystem(this->getSystemId());

        // Register system dependencies
        this->dependsOnSystem(t_cameraUpdateSystem->getSystemId());
        this->dependsOnSystem(t_timingSystem->getSystemId());

        // Enable the system
        this->enableSystem();

    };



    // Destructor class of the CameraUpdateSystemClass
    RendererStartPassSystem::~StartRenderPassSystemClass(){};



    // There is no setup required for this system to execute.
    void RendererStartPassSystem::setupSystem(){};



    //
    void RendererStartPassSystem::executeSystem(){
        if (auto commandBuffer = m_renderer->beginFrame()) {
            int frameIndex = m_renderer->getFrameIndex();
            FrameInfo frameInfo{
                    frameIndex,
                    m_timingSystem->getDt(),
                    commandBuffer,
                    cameraECS.getEntityId(),
                    globalDescriptorSets[frameIndex],
                    m_gameObjects
            };

            // update
            GlobalUbo ubo{};
            ubo.projection = cameraECS.m_cameraData->m_projectionMatrix;
            ubo.view = cameraECS.m_cameraData->m_viewMatrix;
            ubo.inverseView = cameraECS.m_cameraData->m_inverseViewMatrix;
            pointLightSystem.update(frameInfo, ubo);
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();

            // render
            m_renderer->beginSwapChainRenderPass(commandBuffer);
        }
    };



    // There is no clean up required after this system executes.
    void RendererStartPassSystem::cleanupSystem(){};

}