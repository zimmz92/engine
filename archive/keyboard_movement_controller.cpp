#include "keyboard_movement_controller.hpp"

namespace ae {
	
	void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* t_window, float t_dt, AeGameObject& t_gameObject) {
		glm::vec3 rotate{ 0 };

		if (glfwGetKey(t_window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.0f;
		if (glfwGetKey(t_window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.0f;
		if (glfwGetKey(t_window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.0f;
		if (glfwGetKey(t_window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.0f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			t_gameObject.m_transform.rotation += m_lookSpeed * t_dt * glm::normalize(rotate);
		}

		// Limit pitch value between about +/- 85 degrees
		t_gameObject.m_transform.rotation.x = glm::clamp(t_gameObject.m_transform.rotation.x, -1.5f, 1.5f);
		t_gameObject.m_transform.rotation.y = glm::mod(t_gameObject.m_transform.rotation.y, glm::two_pi<float>());

		float yaw = t_gameObject.m_transform.rotation.y;
		const glm::vec3 forwardDir{sin(yaw), 0.0f, cos(yaw)};
		const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
		const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

		glm::vec3 moveDir{ 0.0f };
		if (glfwGetKey(t_window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(t_window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(t_window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(t_window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(t_window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(t_window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			t_gameObject.m_transform.translation += m_moveSpeed * t_dt * glm::normalize(moveDir);
		}


	}
} // namespace ae