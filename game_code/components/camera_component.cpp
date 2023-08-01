/// \file camera_component.cpp
/// \brief The script implementing the camera component class.
/// The camera component class is implemented.
#include "camera_component.hpp"

// std
#include <cassert>
#include <limits>

namespace ae {
    void cameraComponentClass::setOrthographicProjection(ecs_id t_entityId, float t_left, float t_right, float t_top,
                                                         float t_bottom, float t_near, float t_far) {
        auto entityData = this->getData(t_entityId);
        entityData->m_projectionMatrix = glm::mat4{ 1.0f };
        entityData->m_projectionMatrix[0][0] = 2.f / (t_right - t_left);
        entityData->m_projectionMatrix[1][1] = 2.f / (t_bottom - t_top);
        entityData->m_projectionMatrix[2][2] = 1.f / (t_far - t_near);
        entityData->m_projectionMatrix[3][0] = -(t_right + t_left) / (t_right - t_left);
        entityData->m_projectionMatrix[3][1] = -(t_bottom + t_top) / (t_bottom - t_top);
        entityData->m_projectionMatrix[3][2] = -t_near / (t_far - t_near);
    }

    void cameraComponentClass::setPerspectiveProjection(ecs_id t_entityId, float t_fovy, float t_aspect, float t_near, float t_far) {
        assert(glm::abs(t_aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
        auto entityData = this->getData(t_entityId);
        const float tanHalfFovy = tan(t_fovy / 2.f);
        entityData->m_projectionMatrix = glm::mat4{ 0.0f };
        entityData->m_projectionMatrix[0][0] = 1.f / (t_aspect * tanHalfFovy);
        entityData->m_projectionMatrix[1][1] = 1.f / (tanHalfFovy);
        entityData->m_projectionMatrix[2][2] = t_far / (t_far - t_near);
        entityData->m_projectionMatrix[2][3] = 1.f;
        entityData->m_projectionMatrix[3][2] = -(t_far * t_near) / (t_far - t_near);
    }

    void cameraComponentClass::setViewDirection(ecs_id t_entityId, glm::vec3 t_position, glm::vec3 t_direction, glm::vec3 t_up) {
        // TODO: Assertion that the direction provided is non-zero
        const glm::vec3 w{ glm::normalize(t_direction) };
        const glm::vec3 u{ glm::normalize(glm::cross(w, t_up)) };
        const glm::vec3 v{ glm::cross(w, u) };

        auto entityData = this->getData(t_entityId);
        entityData->m_viewMatrix = glm::mat4{ 1.f };
        entityData-> m_viewMatrix[0][0] = u.x;
        entityData->m_viewMatrix[1][0] = u.y;
        entityData->m_viewMatrix[2][0] = u.z;
        entityData->m_viewMatrix[0][1] = v.x;
        entityData->m_viewMatrix[1][1] = v.y;
        entityData->m_viewMatrix[2][1] = v.z;
        entityData->m_viewMatrix[0][2] = w.x;
        entityData->m_viewMatrix[1][2] = w.y;
        entityData->m_viewMatrix[2][2] = w.z;
        entityData->m_viewMatrix[3][0] = -glm::dot(u, t_position);
        entityData->m_viewMatrix[3][1] = -glm::dot(v, t_position);
        entityData->m_viewMatrix[3][2] = -glm::dot(w, t_position);

        entityData->m_inverseViewMatrix = glm::mat4{ 1.f };
        entityData->m_inverseViewMatrix[0][0] = u.x;
        entityData->m_inverseViewMatrix[0][1] = u.y;
        entityData->m_inverseViewMatrix[0][2] = u.z;
        entityData->m_inverseViewMatrix[1][0] = v.x;
        entityData->m_inverseViewMatrix[1][1] = v.y;
        entityData->m_inverseViewMatrix[1][2] = v.z;
        entityData->m_inverseViewMatrix[2][0] = w.x;
        entityData->m_inverseViewMatrix[2][1] = w.y;
        entityData->m_inverseViewMatrix[2][2] = w.z;
        entityData->m_inverseViewMatrix[3][0] = t_position.x;
        entityData->m_inverseViewMatrix[3][1] = t_position.y;
        entityData->m_inverseViewMatrix[3][2] = t_position.z;
    }

    void cameraComponentClass::setViewTarget(ecs_id t_entityId, glm::vec3 t_position, glm::vec3 t_target, glm::vec3 t_up) {
        // TODO: Assertion that the direction provided is non-zero
        setViewDirection(t_entityId, t_position, t_target - t_position, t_up);
    }

    void cameraComponentClass::setViewYXZ(ecs_id t_entityId, glm::vec3 t_position, glm::vec3 t_rotation) {
        // TODO: Assertion that the direction provided is non-zero
        const float c3 = glm::cos(t_rotation.z);
        const float s3 = glm::sin(t_rotation.z);
        const float c2 = glm::cos(t_rotation.x);
        const float s2 = glm::sin(t_rotation.x);
        const float c1 = glm::cos(t_rotation.y);
        const float s1 = glm::sin(t_rotation.y);
        const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
        const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
        const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
        auto entityData = this->getData(t_entityId);
        entityData->m_viewMatrix = glm::mat4{ 1.f };
        entityData->m_viewMatrix[0][0] = u.x;
        entityData->m_viewMatrix[1][0] = u.y;
        entityData->m_viewMatrix[2][0] = u.z;
        entityData->m_viewMatrix[0][1] = v.x;
        entityData->m_viewMatrix[1][1] = v.y;
        entityData->m_viewMatrix[2][1] = v.z;
        entityData->m_viewMatrix[0][2] = w.x;
        entityData->m_viewMatrix[1][2] = w.y;
        entityData->m_viewMatrix[2][2] = w.z;
        entityData->m_viewMatrix[3][0] = -glm::dot(u, t_position);
        entityData->m_viewMatrix[3][1] = -glm::dot(v, t_position);
        entityData->m_viewMatrix[3][2] = -glm::dot(w, t_position);

        entityData->m_inverseViewMatrix = glm::mat4{ 1.f };
        entityData->m_inverseViewMatrix[0][0] = u.x;
        entityData->m_inverseViewMatrix[0][1] = u.y;
        entityData->m_inverseViewMatrix[0][2] = u.z;
        entityData->m_inverseViewMatrix[1][0] = v.x;
        entityData->m_inverseViewMatrix[1][1] = v.y;
        entityData->m_inverseViewMatrix[1][2] = v.z;
        entityData->m_inverseViewMatrix[2][0] = w.x;
        entityData->m_inverseViewMatrix[2][1] = w.y;
        entityData->m_inverseViewMatrix[2][2] = w.z;
        entityData->m_inverseViewMatrix[3][0] = t_position.x;
        entityData->m_inverseViewMatrix[3][1] = t_position.y;
        entityData->m_inverseViewMatrix[3][2] = t_position.z;
    }

}; // ae namespace