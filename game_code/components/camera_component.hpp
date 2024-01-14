/*! \file camera_component.hpp
    \brief The script defining the camera component.
    The camera component is defined. This component defines relevant properties to objects that are cameras or have
    cameras attached to them, such as player characters.
*/
#pragma once

#include "ae_ecs_include.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "world_position_component.hpp"

namespace ae {

    /// This structure defines the data stored for each entity using the player component.
    struct CameraComponentStruct {

        /// The matrix describing the mapping of a pinhole camera from 3D world space to the 2D screen.
        glm::mat4 m_projectionMatrix{ 1.0f };

        /// The camera transform matrix
        glm::mat4 m_viewMatrix{ 1.0f };

        /// Allows specular lighting and contains the position of the camera. Enables transform from camera space to
        /// world space if needed as well.
        glm::mat4 m_inverseViewMatrix{ 1.0f };

        /// The field of view of the camera from the center of the fov to the top of the near plane in the +y direction.
        float m_fovY = glm::radians(50.0f);

        /// The world distance from the camera to the near plane the camera displays.
        float m_nearDistance = 0.1f;

        /// The world distance from the camera to the far plane the camera displays.
        float m_farDistance = 100.0f;

        /// Sets the camera to use a perspective projection. If false will default to orthographic projection.
        bool usePerspectiveProjection = true;

        /// States if the camera is locked onto a set of world coordinates.
        bool cameraLockedOnWorldPosition = false;

        /// The coordinates of the target the camera is locked onto, this is the default behavior.
        WorldPositionComponentStruct cameraLockWorldPosition {0.0f, 0.0f, 0.0f};

        /// Specifies that the camera is locked onto an entity.
        bool cameraLockedOnEntity = false;

        /// The entity ID of the entity the camera is locked onto. Only works if cameraLockedOnEntity is set true.
        ecs_id lockOnEntityId = 0;

        /// Specifies that the camera is locked looking in a specific direction
        bool cameraLockedOnDirection = false;

        /// This is a xyz vector representing the direction that the camera is looking
        glm::vec3 cameraLockDirection = {0.0f, 0.0f, 0.0f};

        /// This indicates if this camera is the main camera of the application
        bool isMainCamera = false;

        /// TODO: Implement camera offset from model center.
    };


    /// The CameraComponent is derived from the AeComponent template class using the CameraComponentStruct.
    class CameraComponent : public ae_ecs::AeComponent<CameraComponentStruct> {
    public:
        /// The CameraComponent constructor uses the AeComponent constructor with no additions.
        CameraComponent(ae_ecs::AeECS& t_ecs) : AeComponent(t_ecs,1,componentStorageMethod_unorderedMap) {};

        /// The destructor of the CameraComponent class. The CameraComponent destructor
        /// uses the AeComponent constructor with no additions.
        ~CameraComponent() {};

    private:

    protected:

    };
}