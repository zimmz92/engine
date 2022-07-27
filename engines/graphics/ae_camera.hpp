#pragma once

// libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace ae {

	class AeCamera {
	public:

		void setOrthographicProjection(float t_left, float t_right, float t_top, float t_bottom, float t_near, float t_far);
		void setPerspectiveProjection(float t_fovy, float t_aspect, float t_near, float t_far);

		void setViewDirection(glm::vec3 t_position, glm::vec3 t_direction, glm::vec3 t_up = { 0.0f, -1.0f, 0.0f });
		void setViewTarget(glm::vec3 t_position, glm::vec3 t_target, glm::vec3 t_up = { 0.0f, -1.0f, 0.0f });
		void setViewYXZ(glm::vec3 t_position, glm::vec3 t_rotation);

		const glm::mat4 getProjection() const { return m_projectionMatrix; }
		const glm::mat4 getView() const { return m_viewMatrix; }
		const glm::mat4 getInverseView() const { return m_inverseViewMatrix;  }

	private:
		glm::mat4 m_projectionMatrix{ 1.0f };
		glm::mat4 m_viewMatrix{ 1.0f };
		glm::mat4 m_inverseViewMatrix{ 1.0f };
	};
}; // namespace ae