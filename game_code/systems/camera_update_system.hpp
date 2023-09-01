/*! \file camera_update_system.hpp
    \brief The script defining the camera update system.
    The camera update system is defined.
*/
#pragma once

#include "ae_ecs_include.hpp"

#include "game_components.hpp"
#include "player_input_system.hpp"

#include "ae_renderer.hpp"

namespace ae {

    /// Updates cameras view/projection matrices based on the camera's settings and updated position/orientation data.
    class CameraUpdateSystem : public ae_ecs::AeSystem<CameraUpdateSystem> {
    public:
        /// Constructor of the CameraUpdateSystem
        /// \param t_game_components The game components available that this system may require.
        /// \param t_playerInputSystem The PlayerInputSystem the CameraUpdateSystem will depend on executing
        /// first and require information from.
        /// \param t_renderer The renderer that the cameras being processed are associated with to obtain the aspect
        /// ratio.
        CameraUpdateSystem(ae_ecs::AeECS& t_ecs,
                           GameComponents& t_game_components,
                           PlayerInputSystem& t_playerInputSystem,
                           AeRenderer& t_renderer);

        /// Destructor of the CameraUpdateSystem
        ~CameraUpdateSystem();

        /// Setup the CameraUpdateSystem, this is handled by the ECS.
        void setupSystem() override;

        /// Execute the CameraUpdateSystem, this is handled by the ECS.
        void executeSystem() override;

        /// Clean up the CameraUpdateSystem, this is handled by the ECS.
        void cleanupSystem() override;

    private:

        // Components this system utilizes.
        /// The WorldPositionComponent this systems accesses to obtain the world position of the camera entities.
        WorldPositionComponent& m_worldPositionComponent;
        /// The ModelComponent this systems accesses to obtain the rotation of the camera entities.
        ModelComponent& m_modelComponent;
        /// The CameraComponent this systems accesses to read their camera attributes to update the view/perspective
        /// matrices.
        CameraComponent& m_cameraComponent;

        // Prerequisite systems for the CameraUpdateSystem.
        /// Points to the PlayerInputSystem this system requires to operate prior to it's own operation.
        PlayerInputSystem& m_playerInputSystem;

        /// The renderer that this system will pull aspect ratio information from.
        AeRenderer& m_renderer;

        /// Calculate and set the camera's projection based on an orthographic view volume.
        /// \param t_entityCameraComponentData The CameraComponentStruct data for the specific entity in which the
        /// projection matrix is to be calculated and set for.
        /// \param t_left The point of the orthographic view volume furthest in the -X direction.
        /// \param t_right The point of the orthographic view volume furthest in the +X direction.
        /// \param t_top The point of the orthographic view volume furthest in the -y direction (from Vulkan's perspective).
        /// \param t_bottom The point of the orthographic view volume furthest in the +y direction (from Vulkan's perspective).
        /// \param t_near The point of the orthographic view volume furthest in the -z direction (from Vulkan's perspective).
        /// \param t_far The point of the orthographic view volume furthest in the +z direction (from Vulkan's perspective).
        static void setOrthographicProjection(CameraComponentStruct& t_entityCameraComponentData,
                                              float t_left, float t_right,
                                              float t_top, float t_bottom,
                                              float t_near, float t_far);

        /// Calculate and set the camera's projection based on a frustum (looking through a rectangular window) view
        /// volume for perspective.
        /// \param t_entityCameraComponentData The CameraComponentStruct data for the specific entity in which the
        /// projection matrix is to be calculated and set for.
        /// \param t_aspect The aspect ratio of the window the perspective projection matrix is being calculated for.
        static void setPerspectiveProjection(CameraComponentStruct& t_entityCameraComponentData, float t_aspect);

        /// Calculate the camera's view matrices using the position of the camera and a specified vector representing
        /// which direction the camera should face..
        /// \param t_entityCameraData The CameraComponentStruct data for the specific entity.
        /// \param t_entityModelData The ModelComponentStruct data for the specific entity.
        /// \param t_entityWorldPosition The WorldPositionComponentStruct data for the specific entity.
        /// \param t_direction The vector that specifies the direction the camera should point.
        static void setViewDirection(CameraComponentStruct& t_entityCameraComponentData,
                                     ModelComponentStruct& t_entityModelData,
                                     WorldPositionComponentStruct& t_entityWorldPosition,
                                     glm::vec3 t_direction);

        /// Calculate the camera's view matrices using the position of the camera and a specified world position to lock
        /// the camera onto.
        /// \param t_entityCameraData The CameraComponentStruct data for the specific entity.
        /// \param t_entityModelData The ModelComponentStruct data for the specific entity.
        /// \param t_entityWorldPosition The WorldPositionComponentStruct data for the specific entity.
        /// \param t_target The world position, coordinates, that the camera should be locked onto.
        static void setViewTarget(CameraComponentStruct& t_entityCameraComponentData,
                                  ModelComponentStruct& t_entityModelData,
                                  WorldPositionComponentStruct& t_entityWorldPosition,
                                  glm::vec3 t_target);

        /// Calculate the camera's view matrices using the position and rotation of the camera by the Euler angles
        /// method.
        /// \param t_entityCameraData The CameraComponentStruct data for the specific entity.
        /// \param t_entityModelData The ModelComponentStruct data for the specific entity.
        /// \param t_entityWorldPosition The WorldPositionComponentStruct data for the specific entity.
        static void setViewYXZ(CameraComponentStruct& t_entityCameraComponentData,
                               ModelComponentStruct& t_entityModelData,
                               WorldPositionComponentStruct& t_entityWorldPosition);

    };
}