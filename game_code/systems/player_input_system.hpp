/*! \file player_input_system.hpp
    \brief The script defining the player input system.

    The player input system is defined.

*/
#pragma once

#include "ae_ecs.hpp"

#include "game_components.hpp"
#include "timing_system.hpp"

#include "ae_window.hpp"

namespace ae {

    /// A system that retrieves the player's inputs and moves/rotates the currently controlled entity.
    class PlayerInputSystem : public ae_ecs::AeSystem<PlayerInputSystem> {
    public:
        /// Constructor of the PlayerInputSystem
        /// \param t_game_components The game components available that this system may require.
        /// \param t_timingSystem The TimingSystem the PlayerInputSystem will depend on executing first
        /// and require information from.
        /// \param t_window A pointer to the GLFW window that will be polling the player's inputs.
        PlayerInputSystem(GameComponentsStruct& t_game_components, TimingSystem& t_timingSystem, GLFWwindow* t_window);

        /// Destructor of the PlayerInputSystem
        ~PlayerInputSystem();

        /// Setup the PlayerInputSystem, this is handled by the ECS.
        void setupSystem() override;

        /// Execute the PlayerInputSystem, this is handled by the ECS.
        void executeSystem() override;

        /// Clean up the PlayerInputSystem, this is handled by the ECS.
        void cleanupSystem() override;

        /// Define structure that sets the key mapping of the keyboard keys to player controlled entity movement.
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

        /// Structure that sets the key mapping of the keyboard keys to player controlled entity movement.
        KeyMappings keys{};

        /// Is the movement speed of the player controlled entity when the entity movement keys are pressed.
        float m_moveSpeed{ 3.0f };

        /// Is the look speed (rotation) of the player controlled entity when the look movement keys are pressed.
        float m_lookSpeed{ 1.5f };

    private:

        /// The GLFW
        GLFWwindow* m_window;


        // Components this system utilizes.
        /// The WorldPositionComponent this systems accesses to update the position of player controlled entities.
        WorldPositionComponent& m_worldPositionComponent;
        /// The component this systems interacts with.
        ModelComponent& m_modelComponent;
        ///T he world position component this systems interacts with.
        PlayerControlledComponent& m_playerControlledComponent;


        // Prerequisite systems for the PlayerInputSystem.
        /// Points to the TimingSystem this system requires to operate prior to it's own operation.
        TimingSystem& m_timingSystem;


        /// Update the passed components based on the
        void moveInPlaneYXZ(WorldPositionComponentStruct& t_worldPosition, ModelComponentStruct& t_modelData);

    };
}


