/*! \file renderer_start_system.hpp
    \brief The script defining the renderer start system.

    The player input system is defined and the instance for the game is declared.

*/
#pragma once

#include "ae_ecs.hpp"

#include "game_components.hpp"
#include "camera_update_system.hpp"
#include "timing_system.hpp"

#include "ae_renderer.hpp"

namespace ae {
    class RendererStartPassSystem : public ae_ecs::AeSystem<RendererStartPassSystem> {
    public:
        RendererStartPassSystem(GameComponents* t_game_components,
                                   CameraUpdateSystemClass* t_cameraUpdateSystem,
                                   TimingSystem* t_timingSystem,
                                   AeRenderer* t_renderer);
        ~RendererStartPassSystem();

        void setupSystem() override;
        void executeSystem() override;
        void cleanupSystem() override;

    private:

        /// Pointer to the game components that this game is actually using.
        GameComponents* m_game_components;

        // Pointer to the game renderer
        // TODO: Make this an entity of it's own.
        AeRenderer* m_renderer;

        // Pointers to systems that are required by this system
        CameraUpdateSystemClass* m_cameraUpdateSystem;
        TimingSystem* m_timingSystem;

    };
}