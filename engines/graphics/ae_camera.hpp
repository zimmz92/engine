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

		const glm::mat4 getProjection() const { return m_projectionMatrix; };

	private:
		glm::mat4 m_projectionMatrix{ 1.0f };


	};
}; // namespace ae