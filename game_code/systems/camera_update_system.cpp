/// \file camera_update_system.cpp
/// \brief The script implementing the camera update system class.
/// The camera update system class is implemented.

#include "camera_update_system.hpp"

#include "world_position_component.hpp"
#include "model_component.hpp"
#include "camera_component.hpp"

namespace ae {
    // Constructor of the CameraUpdateSystemClass
    CameraUpdateSystemClass::CameraUpdateSystemClass(AeRenderer* t_renderer, PlayerInputSystem* t_playerInputSystem)  : ae_ecs::AeSystem<CameraUpdateSystemClass>()  {
        m_renderer = t_renderer;
        m_playerInputSystem = t_playerInputSystem;

        // Register component dependencies
        worldPositionComponent.requiredBySystem(this->getSystemId());
        modelComponent.requiredBySystem(this->getSystemId());
        cameraComponent.requiredBySystem(this->getSystemId());

        // Register system dependencies
        this->dependsOnSystem(t_playerInputSystem->getSystemId());

        // Enable the system
        this->enableSystem();

    };



    // Destructor class of the CameraUpdateSystemClass
    CameraUpdateSystemClass::~CameraUpdateSystemClass(){};



    // There is no setup required for this system to execute.
    void CameraUpdateSystemClass::setupSystem(){};



    // Updates the camera's view properties based upon its position in the world, the properties of the camera, and the
    // desired projection.
    void CameraUpdateSystemClass::executeSystem(){
        std::vector<ecs_id> validEntityIds = m_systemManager.getValidEntities(this->getSystemId());

        for (ecs_id entityId : validEntityIds){
            cameraComponentStructure* entityCameraData = cameraComponent.getDataPointer(entityId);
            worldPositionComponentStruct* entityWorldPosition = worldPositionComponent.getDataPointer(entityId);
            modelComponentStruct* entityModel = modelComponent.getDataPointer(entityId);

            // Set the view based upon the updated position of the camera, and potentially it's target.
            if(entityCameraData->cameraLockedOnDirection){
                setViewDirection(entityCameraData,
                              entityModel,
                              entityWorldPosition,
                              entityCameraData->cameraLockDirection);
            }else if(entityCameraData->cameraLockedOnEntity){
                worldPositionComponentStruct* targetEntityWorldPosition = worldPositionComponent.getDataPointer(entityCameraData->lockOnEntityId);
                setViewTarget(entityCameraData,
                              entityModel,
                              entityWorldPosition,
                              {targetEntityWorldPosition->rho,targetEntityWorldPosition->theta,targetEntityWorldPosition->phi});
            }else if(entityCameraData->cameraLockedOnWorldPosition){
                setViewTarget(entityCameraData,
                              entityModel,
                              entityWorldPosition,
                              {entityCameraData->cameraLockWorldPosition.rho,entityCameraData->cameraLockWorldPosition.theta,entityCameraData->cameraLockWorldPosition.phi});
            }else {
                setViewYXZ(entityCameraData,entityModel,entityWorldPosition);
            }

            // Update the perspective in case the window has changed
            if(entityCameraData->usePerspectiveProjection) {
                setPerspectiveProjection(entityCameraData,m_renderer->getAspectRatio());
            }else {
                // Standard Cube TODO: Add this to camera component.
                setOrthographicProjection(entityCameraData,-1,1,-1,1,-1,1);
            };
        }
    };



    // There is no clean up required after this system executes.
    void CameraUpdateSystemClass::cleanupSystem(){};



    void CameraUpdateSystemClass::setOrthographicProjection(cameraComponentStructure* t_entityCameraData, float  t_left, float t_right, float t_top, float t_bottom, float t_near, float t_far) {
        t_entityCameraData->m_projectionMatrix = glm::mat4{ 1.0f };
        t_entityCameraData->m_projectionMatrix[0][0] = 2.f / (t_right - t_left);
        t_entityCameraData->m_projectionMatrix[1][1] = 2.f / (t_bottom - t_top);
        t_entityCameraData->m_projectionMatrix[2][2] = 1.f / (t_far - t_near);
        t_entityCameraData->m_projectionMatrix[3][0] = -(t_right + t_left) / (t_right - t_left);
        t_entityCameraData->m_projectionMatrix[3][1] = -(t_bottom + t_top) / (t_bottom - t_top);
        t_entityCameraData->m_projectionMatrix[3][2] = -t_near / (t_far - t_near);
    }

    void CameraUpdateSystemClass::setPerspectiveProjection(cameraComponentStructure* t_entityCameraData, float t_aspect) {
        assert(glm::abs(t_aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
        const float tanHalfFovy = tan(t_entityCameraData->m_fovY / 2.f);
        t_entityCameraData->m_projectionMatrix = glm::mat4{ 0.0f };
        t_entityCameraData->m_projectionMatrix[0][0] = 1.f / (t_aspect * tanHalfFovy);
        t_entityCameraData->m_projectionMatrix[1][1] = 1.f / (tanHalfFovy);
        t_entityCameraData->m_projectionMatrix[2][2] = t_entityCameraData->m_farDistance / (t_entityCameraData->m_farDistance - t_entityCameraData->m_nearDistance);
        t_entityCameraData->m_projectionMatrix[2][3] = 1.f;
        t_entityCameraData->m_projectionMatrix[3][2] = -(t_entityCameraData->m_farDistance * t_entityCameraData->m_nearDistance) / (t_entityCameraData->m_farDistance - t_entityCameraData->m_nearDistance);
    }

    void CameraUpdateSystemClass::setViewDirection(cameraComponentStructure* t_entityCameraData, modelComponentStruct* t_entityModelData, worldPositionComponentStruct* t_entityWorldPosition, glm::vec3 t_direction) {
        glm::vec3 position = {t_entityWorldPosition->rho, t_entityWorldPosition->theta, t_entityWorldPosition->phi};

        // TODO: Assertion that the direction provided is non-zero
        const glm::vec3 w{ glm::normalize(t_direction) };
        const glm::vec3 u{ glm::normalize(glm::cross(w, t_entityModelData->rotation)) };
        const glm::vec3 v{ glm::cross(w, u) };

        t_entityCameraData->m_viewMatrix = glm::mat4{ 1.f };
        t_entityCameraData->m_viewMatrix[0][0] = u.x;
        t_entityCameraData->m_viewMatrix[1][0] = u.y;
        t_entityCameraData->m_viewMatrix[2][0] = u.z;
        t_entityCameraData->m_viewMatrix[0][1] = v.x;
        t_entityCameraData->m_viewMatrix[1][1] = v.y;
        t_entityCameraData->m_viewMatrix[2][1] = v.z;
        t_entityCameraData->m_viewMatrix[0][2] = w.x;
        t_entityCameraData->m_viewMatrix[1][2] = w.y;
        t_entityCameraData->m_viewMatrix[2][2] = w.z;
        t_entityCameraData->m_viewMatrix[3][0] = -glm::dot(u, position);
        t_entityCameraData->m_viewMatrix[3][1] = -glm::dot(v, position);
        t_entityCameraData->m_viewMatrix[3][2] = -glm::dot(w, position);

        t_entityCameraData->m_inverseViewMatrix = glm::mat4{ 1.f };
        t_entityCameraData->m_inverseViewMatrix[0][0] = u.x;
        t_entityCameraData->m_inverseViewMatrix[0][1] = u.y;
        t_entityCameraData->m_inverseViewMatrix[0][2] = u.z;
        t_entityCameraData->m_inverseViewMatrix[1][0] = v.x;
        t_entityCameraData->m_inverseViewMatrix[1][1] = v.y;
        t_entityCameraData->m_inverseViewMatrix[1][2] = v.z;
        t_entityCameraData->m_inverseViewMatrix[2][0] = w.x;
        t_entityCameraData->m_inverseViewMatrix[2][1] = w.y;
        t_entityCameraData->m_inverseViewMatrix[2][2] = w.z;
        t_entityCameraData->m_inverseViewMatrix[3][0] = position.x;
        t_entityCameraData->m_inverseViewMatrix[3][1] = position.y;
        t_entityCameraData->m_inverseViewMatrix[3][2] = position.z;
    }

    void CameraUpdateSystemClass::setViewTarget(cameraComponentStructure* t_entityCameraData, modelComponentStruct* t_entityModelData, worldPositionComponentStruct* t_entityWorldPosition, glm::vec3 t_target) {
        glm::vec3 position = {t_entityWorldPosition->rho, t_entityWorldPosition->theta, t_entityWorldPosition->phi};
        setViewDirection(t_entityCameraData, t_entityModelData, t_entityWorldPosition, t_target - position);
    }

    void CameraUpdateSystemClass::setViewYXZ(cameraComponentStructure* t_entityCameraData, modelComponentStruct* t_entityModelData, worldPositionComponentStruct* t_entityWorldPosition) {
        glm::vec3 position = {t_entityWorldPosition->rho, t_entityWorldPosition->theta, t_entityWorldPosition->phi};

        const float c3 = glm::cos(t_entityModelData->rotation.z);
        const float s3 = glm::sin(t_entityModelData->rotation.z);
        const float c2 = glm::cos(t_entityModelData->rotation.x);
        const float s2 = glm::sin(t_entityModelData->rotation.x);
        const float c1 = glm::cos(t_entityModelData->rotation.y);
        const float s1 = glm::sin(t_entityModelData->rotation.y);
        const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
        const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
        const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
        t_entityCameraData->m_viewMatrix = glm::mat4{ 1.f };
        t_entityCameraData->m_viewMatrix[0][0] = u.x;
        t_entityCameraData->m_viewMatrix[1][0] = u.y;
        t_entityCameraData->m_viewMatrix[2][0] = u.z;
        t_entityCameraData->m_viewMatrix[0][1] = v.x;
        t_entityCameraData->m_viewMatrix[1][1] = v.y;
        t_entityCameraData->m_viewMatrix[2][1] = v.z;
        t_entityCameraData->m_viewMatrix[0][2] = w.x;
        t_entityCameraData->m_viewMatrix[1][2] = w.y;
        t_entityCameraData->m_viewMatrix[2][2] = w.z;
        t_entityCameraData->m_viewMatrix[3][0] = -glm::dot(u, position);
        t_entityCameraData->m_viewMatrix[3][1] = -glm::dot(v, position);
        t_entityCameraData->m_viewMatrix[3][2] = -glm::dot(w, position);

        t_entityCameraData->m_inverseViewMatrix = glm::mat4{ 1.f };
        t_entityCameraData->m_inverseViewMatrix[0][0] = u.x;
        t_entityCameraData->m_inverseViewMatrix[0][1] = u.y;
        t_entityCameraData->m_inverseViewMatrix[0][2] = u.z;
        t_entityCameraData->m_inverseViewMatrix[1][0] = v.x;
        t_entityCameraData->m_inverseViewMatrix[1][1] = v.y;
        t_entityCameraData->m_inverseViewMatrix[1][2] = v.z;
        t_entityCameraData->m_inverseViewMatrix[2][0] = w.x;
        t_entityCameraData->m_inverseViewMatrix[2][1] = w.y;
        t_entityCameraData->m_inverseViewMatrix[2][2] = w.z;
        t_entityCameraData->m_inverseViewMatrix[3][0] = position.x;
        t_entityCameraData->m_inverseViewMatrix[3][1] = position.y;
        t_entityCameraData->m_inverseViewMatrix[3][2] = position.z;
    }
}