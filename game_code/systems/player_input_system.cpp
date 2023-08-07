/// \file player_input_system.cpp
/// \brief The script implementing the player input system class.
/// The player input system class is implemented.

#include "player_input_system.hpp"

namespace ae {

    // Constructor implementation
    PlayerInputSystem::PlayerInputSystem(GameComponentsStruct& t_game_components, TimingSystem& t_timingSystem, GLFWwindow* t_window) :
    m_window{t_window},
    m_worldPositionComponent{t_game_components.worldPositionComponent},
    m_modelComponent{t_game_components.modelComponent},
    m_playerControlledComponent{t_game_components.playerControlledComponent},
    m_timingSystem{t_timingSystem},
    ae_ecs::AeSystem<PlayerInputSystem>() {

        // Register component dependencies
        m_worldPositionComponent.requiredBySystem(this->getSystemId());
        m_modelComponent.requiredBySystem(this->getSystemId());
        m_playerControlledComponent.requiredBySystem(this->getSystemId());

        // Register system dependencies
        this->dependsOnSystem(m_timingSystem.getSystemId());

        // Enable the system so it will run.
        this->enableSystem();
    };



    // Destructor implementation
    PlayerInputSystem::~PlayerInputSystem(){};



    // Set up the system prior to execution. Currently not used.
    void PlayerInputSystem::setupSystem() {};



    // Check for player inputs and apply them to controlled entities.
    void PlayerInputSystem::executeSystem(){

        // Get the entities that use the components this system depends on.
        std::vector<ecs_id> validEntityIds = m_systemManager.getValidEntities(this->getSystemId());

        // TODO: Need to call a function here that calculates the required change in the controlled entities position
        //  before looping through all the entities.

        // Loop through the valid entities and update their world position based on the player's inputs.
        for (ecs_id entityId : validEntityIds){

            // Only move the entity if it is currently being controlled by the player.
            if(m_playerControlledComponent.getDataReference(entityId).isCurrentlyControlled){

                // Move the entity based on the player's input.
                moveInPlaneYXZ(m_worldPositionComponent.getDataReference(entityId),
                               m_modelComponent.getDataReference(entityId));
            };
        };
    };



    // Clean up the system after execution. Currently not used.
    void PlayerInputSystem::cleanupSystem() {};



    // Helper function that translates the users inputs to desired entity position and rotation updates.
    void PlayerInputSystem::moveInPlaneYXZ(WorldPositionComponentStruct& t_worldPosition, ModelComponentStruct& t_modelData){

        // Initialize the rotation transform matrix.
        glm::vec3 rotate{ 0 };

        // Get the key presses that represent the players desire to rotate the model.
        if (glfwGetKey(m_window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.0f;
        if (glfwGetKey(m_window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.0f;
        if (glfwGetKey(m_window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.0f;
        if (glfwGetKey(m_window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.0f;

        // Apply the rotation transform matrix to the model accounting for the amount of time that has past since the
        // last update.
        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
            t_modelData.rotation += m_lookSpeed * m_timingSystem.getDt() * glm::normalize(rotate);
        }

        // Limit pitch value between about +/- 85 degrees.
        t_modelData.rotation.x = glm::clamp(t_modelData.rotation.x, -1.5f, 1.5f);
        t_modelData.rotation.y = glm::mod(t_modelData.rotation.y, glm::two_pi<float>());

        // Account for the fact that the model has rotated when updating the model movement.
        float yaw = t_modelData.rotation.y;
        const glm::vec3 forwardDir{sin(yaw), 0.0f, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
        const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

        // Initialize the movement vector matrix.
        glm::vec3 moveDir{ 0.0f };

        // Get the key presses that represent the players desire to move the model.
        if (glfwGetKey(m_window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(m_window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(m_window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(m_window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(m_window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(m_window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

        // As long as the requested vector of movement is greater than the limits that a float can capture move the entity.
        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {

            // Calculate the actual movement of the entity to be applied using the movement vector
            glm::vec3 movement = m_moveSpeed * m_timingSystem.getDt() * glm::normalize(moveDir);

            // Move the entity.
            t_worldPosition.rho = t_worldPosition.rho + movement.x;
            t_worldPosition.theta = t_worldPosition.theta + movement.y;
            t_worldPosition.phi = t_worldPosition.phi + movement.z;

        };
    };
}
