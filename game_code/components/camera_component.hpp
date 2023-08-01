/*! \file camera_component.hpp
    \brief The script defining the camera component.

    The camera component is defined and the instance for the game is declared. This component defines
    relevant properties to objects that are cameras or have cameras attached to them, such as player characters.

*/
#pragma once

#include "ae_ecs.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace ae {

    /// This structure defines the data stored for each entity using the player component.
    struct cameraComponentStructure {

        /// The matrix describing the mapping of a pinhole camera from 3D world space to the 2D screen.
        glm::mat4 m_projectionMatrix{ 1.0f };

        ///
        glm::mat4 m_viewMatrix{ 1.0f };

        ///
        glm::mat4 m_inverseViewMatrix{ 1.0f };

        /// TODO: Implement camera offset from model center.
    };


    /// The player controlled component class is derived from the AeComponent template class using the player controlled
    /// component structure.
    class cameraComponentClass : public ae_ecs::AeComponent<cameraComponentStructure> {
    public:
        /// The playerControlledComponentClass constructor uses the AeComponent constructor with no additions.
        cameraComponentClass() : AeComponent() {};

        /// The destructor of the playerControlledComponentClass class. The playerControlledComponentClass destructor
        /// uses the AeComponent constructor with no additions.
        ~cameraComponentClass() {};


        void setOrthographicProjection(ecs_id t_entityId, float t_left, float t_right, float t_top, float t_bottom, float t_near, float t_far);
        void setPerspectiveProjection(ecs_id t_entityId, float t_fovy, float t_aspect, float t_near, float t_far);

        void setViewDirection(ecs_id t_entityId, glm::vec3 t_position, glm::vec3 t_direction, glm::vec3 t_up = { 0.0f, -1.0f, 0.0f });

        /// Camera locked onto a particular location, or object.
        void setViewTarget(ecs_id t_entityId, glm::vec3 t_position, glm::vec3 t_target, glm::vec3 t_up = { 0.0f, -1.0f, 0.0f });

        /// Euler angles to specify position of the camera
        void setViewYXZ(ecs_id t_entityId, glm::vec3 t_position, glm::vec3 t_rotation);

    private:

    protected:

    };

    /// The instantiation of the player controlled component for use by the game.
    inline cameraComponentClass cameraComponent;
}