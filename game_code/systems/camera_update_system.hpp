/*! \file camera_update_system.hpp
    \brief The script defining the camera update system.

    The camera update system is defined.

*/
#pragma once

#include "ae_ecs.hpp"

#include "game_components.hpp"
#include "player_input_system.hpp"

#include "ae_renderer.hpp"

namespace ae {

    /// Updates cameras view/projection matrices based on the camera's settings and updated position/orientation data.
    class CameraUpdateSystemClass : public ae_ecs::AeSystem<CameraUpdateSystemClass> {
    public:
        /// Constructor of the CameraUpdateSystemClass
        /// \param t_game_components The game components available that this system may require.
        /// \param t_playerInputSystem The PlayerInputSystemClass the CameraUpdateSystemClass will depend on executing
        /// first and require information from.
        /// \param t_renderer The renderer that the cameras being processed are associated with to obtain the aspect
        /// ratio.
        CameraUpdateSystemClass(GameComponentsStruct& t_game_components,
                                PlayerInputSystemClass& t_playerInputSystem,
                                AeRenderer& t_renderer);

        /// Destructor of the CameraUpdateSystemClass
        ~CameraUpdateSystemClass();

        /// Setup the CameraUpdateSystemClass, this is handled by the ECS.
        void setupSystem() override;

        /// Execute the CameraUpdateSystemClass, this is handled by the ECS.
        void executeSystem() override;

        /// Clean up the CameraUpdateSystemClass, this is handled by the ECS.
        void cleanupSystem() override;

    private:

        // Components this system utilizes.
        /// The WorldPositionComponent this systems accesses to obtain the world position of the camera entities.
        WorldPositionComponentClass& m_worldPositionComponent;
        /// The ModelComponent this systems accesses to obtain the rotation of the camera entities.
        ModelComponentClass& m_modelComponent;
        /// The CameraComponent this systems accesses to read their camera attributes to update the view/perspective
        /// matrices.
        CameraComponentClass& m_cameraComponent;

        // Prerequisite systems for the CameraUpdateSystemClass.
        /// Points to the PlayerInputSystemClass this system requires to operate prior to it's own operation.
        PlayerInputSystemClass& m_playerInputSystem;

        /// The renderer that this system will pull aspect ratio information from.
        AeRenderer& m_renderer;

        void setOrthographicProjection(CameraComponentStructure* t_entityCameraComponentData, float t_left, float t_right, float t_top, float t_bottom, float t_near, float t_far);
        void setPerspectiveProjection(CameraComponentStructure* t_entityCameraComponentData, float t_aspect);

        /// Sets the direction in which the camera will be looking.
        void setViewDirection(CameraComponentStructure* t_entityCameraData, ModelComponentStruct* t_entityModelData, WorldPositionComponentStruct* t_entityWorldPosition, glm::vec3 t_direction);

        /// Camera locked onto a particular location, or object.
        void setViewTarget(CameraComponentStructure* t_entityCameraData, ModelComponentStruct* t_entityModelData, WorldPositionComponentStruct* t_entityWorldPosition, glm::vec3 t_target);

        /// Euler angles to specify position of the camera and what it is viewing.
        void setViewYXZ(CameraComponentStructure* t_entityCameraData, ModelComponentStruct* t_entityModelData, WorldPositionComponentStruct* t_entityWorldPostition);

    };
}