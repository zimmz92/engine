/// \file player_input_system.cpp
/// \brief The script implementing the player input system class.
/// The player input system class is implemented.

#include "player_input_system.hpp"
#include "timing_system.hpp"
#include "player_controlled_component.hpp"

namespace ae {
    PlayerInputSystem::PlayerInputSystem(GLFWwindow* t_window) : ae_ecs::AeSystem<PlayerInputSystem>()  {
        m_window = t_window;
        // Register component dependencies
        worldPositionComponent.requiredBySystem(this->getSystemId());
        modelComponent.requiredBySystem(this->getSystemId());
        playerControlledComponent.requiredBySystem(this->getSystemId());

        // Register system dependencies
        this->dependsOnSystem(timingSystem.getSystemId());
        this->enableSystem();
    };

    PlayerInputSystem::~PlayerInputSystem(){};

    void PlayerInputSystem::executeSystem(){
        std::vector<ecs_id> validEntityIds = m_systemManager.getValidEntities(this->getSystemId());

        for (ecs_id entityId : validEntityIds){
            if(playerControlledComponent.getDataPointer(entityId)->isCurrentlyControlled){
                moveInPlaneXZ( worldPositionComponent.getDataPointer(entityId), modelComponent.getDataPointer(entityId));
            };
        };
    };

    void PlayerInputSystem::moveInPlaneXZ(worldPositionComponentStruct* t_worldPosition, modelComponentStruct* t_modelData){
        glm::vec3 rotate{ 0 };

        if (glfwGetKey(m_window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.0f;
        if (glfwGetKey(m_window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.0f;
        if (glfwGetKey(m_window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.0f;
        if (glfwGetKey(m_window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.0f;

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
            t_modelData->rotation += m_lookSpeed * timingSystem.getDt() * glm::normalize(rotate);
        }

        // Limit pitch value between about +/- 85 degrees
        t_modelData->rotation.x = glm::clamp(t_modelData->rotation.x, -1.5f, 1.5f);
        t_modelData->rotation.y = glm::mod(t_modelData->rotation.y, glm::two_pi<float>());

        float yaw = t_modelData->rotation.y;
        const glm::vec3 forwardDir{sin(yaw), 0.0f, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
        const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

        glm::vec3 moveDir{ 0.0f };
        if (glfwGetKey(m_window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(m_window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(m_window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(m_window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(m_window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(m_window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            glm::vec3 movement = m_moveSpeed * timingSystem.getDt() * glm::normalize(moveDir);
            t_worldPosition->rho = t_worldPosition->rho + movement.x;
            t_worldPosition->theta = t_worldPosition->theta + movement.y;
            t_worldPosition->phi = t_worldPosition->phi + movement.z;

        };
    };
}
