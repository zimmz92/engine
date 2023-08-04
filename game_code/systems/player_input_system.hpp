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
    class PlayerInputSystemClass : public ae_ecs::AeSystem<PlayerInputSystemClass> {
    public:
        /// Constructor of the PlayerInputSystemClass
        /// \param t_game_components The game components available that this system may require.
        /// \param t_timingSystem The TimingSystemClass the PlayerInputSystemClass will depend on executing first
        /// and require information from.
        /// \param t_window A pointer to the GLFW window that will be polling the player's inputs.
        PlayerInputSystemClass(GameComponentsStruct& t_game_components, TimingSystemClass& t_timingSystem, GLFWwindow* t_window);

        /// Destructor of the PlayerInputSystemClass
        ~PlayerInputSystemClass();

        /// Setup the PlayerInputSystemClass, this is handled by the ECS.
        void setupSystem() override;

        /// Execute the PlayerInputSystemClass, this is handled by the ECS.
        void executeSystem() override;

        /// Clean up the PlayerInputSystemClass, this is handled by the ECS.
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
        WorldPositionComponentClass& m_worldPositionComponent;
        /// The component this systems interacts with.
        ModelComponentClass& m_modelComponent;
        ///T he world position component this systems interacts with.
        PlayerControlledComponentClass& m_playerControlledComponent;


        // Prerequisite systems for the PlayerInputSystemClass.
        /// Points to the TimingSystemClass this system requires to operate prior to it's own operation.
        TimingSystemClass& m_timingSystem;


        /// Update the passed components based on the
        void moveInPlaneYXZ(WorldPositionComponentStruct& t_worldPosition, ModelComponentStruct& t_modelData);

    };
}


