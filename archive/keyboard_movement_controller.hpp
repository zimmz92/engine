#pragma once

#include "ae_game_object.hpp"
#include "ae_window.hpp"

namespace ae {
	class KeyboardMovementController {
	public:
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

        void moveInPlaneXZ(GLFWwindow* t_window, float t_dt, AeGameObject& t_gameObject);

        KeyMappings keys{};
        float m_moveSpeed{ 3.0f };
        float m_lookSpeed{ 1.5f };


	};


} // namespace ae