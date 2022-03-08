#include "ae_camera.hpp"

// std
#include <cassert>
#include <limits>

namespace ae {
    void AeCamera::setOrthographicProjection(float t_left, float t_right, float t_top, float t_bottom, float t_near, float t_far) {
        m_projectionMatrix = glm::mat4{ 1.0f };
        m_projectionMatrix[0][0] = 2.f / (t_right - t_left);
        m_projectionMatrix[1][1] = 2.f / (t_bottom - t_top);
        m_projectionMatrix[2][2] = 1.f / (t_far - t_near);
        m_projectionMatrix[3][0] = -(t_right + t_left) / (t_right - t_left);
        m_projectionMatrix[3][1] = -(t_bottom + t_top) / (t_bottom - t_top);
        m_projectionMatrix[3][2] = -t_near / (t_far - t_near);
    }

    void AeCamera::setPerspectiveProjection(float t_fovy, float t_aspect, float t_near, float t_far) {
        assert(glm::abs(t_aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
        const float tanHalfFovy = tan(t_fovy / 2.f);
        m_projectionMatrix = glm::mat4{ 0.0f };
        m_projectionMatrix[0][0] = 1.f / (t_aspect * tanHalfFovy);
        m_projectionMatrix[1][1] = 1.f / (tanHalfFovy);
        m_projectionMatrix[2][2] = t_far / (t_far - t_near);
        m_projectionMatrix[2][3] = 1.f;
        m_projectionMatrix[3][2] = -(t_far * t_near) / (t_far - t_near);
    }
}; // ae namespace