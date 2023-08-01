/*! \file player_input_system.hpp
    \brief The script defining the player input system.

    The player input system is defined and the instance for the game is declared.

*/
#pragma once

#include "ae_ecs.hpp"
#include "ae_window.hpp"
#include "world_position_component.hpp"
#include "model_component.hpp"

namespace ae {
    class PlayerInputSystem : public ae_ecs::AeSystem<PlayerInputSystem> {
    public:
        PlayerInputSystem(GLFWwindow* t_window);
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

    };

    //inline PlayerInputSystem playerInputSystem;
}


