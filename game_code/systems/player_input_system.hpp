/*! \file player_input_system.hpp
    \brief The script defining the player input system.

    The player input system is defined and the instance for the game is declared.

*/
#pragma once

#include "ae_ecs.hpp"

#include "game_components.hpp"
#include "timing_system.hpp"

#include "ae_window.hpp"

namespace ae {
    class PlayerInputSystem : public ae_ecs::AeSystem<PlayerInputSystem> {
    public:
        PlayerInputSystem(GameComponents* t_game_components, TimingSystem* t_timingSystem, GLFWwindow* t_window);
        ~PlayerInputSystem();

        void setupSystem() override{};
        void executeSystem() override;
        void cleanupSystem() override{};

        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        void moveInPlaneXZ(worldPositionComponentStruct* t_worldPosition, modelComponentStruct* t_modelData);

        KeyMappings keys{};
        float m_moveSpeed{ 3.0f };
        float m_lookSpeed{ 1.5f };

    private:

        GLFWwindow* m_window;

        GameComponents* m_game_components;
        TimingSystem* m_timingSystem;

    };
}


