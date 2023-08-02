/*! \file player_input_system.hpp
    \brief The script defining the player input system.

    The player input system is defined and the instance for the game is declared.

*/
#pragma once

#include "ae_ecs.hpp"

#include "game_components.hpp"
#include "player_input_system.hpp"

#include "ae_renderer.hpp"

namespace ae {
    class CameraUpdateSystemClass : public ae_ecs::AeSystem<CameraUpdateSystemClass> {
    public:
        CameraUpdateSystemClass(GameComponents* t_game_components,
                                PlayerInputSystem* t_playerInputSystem,
                                AeRenderer* t_renderer);
        ~CameraUpdateSystemClass();

        void setupSystem() override;
        void executeSystem() override;
        void cleanupSystem() override;

    private:

        /// The renderer that this system will pull it's information from.
        /// TODO: Remove this pointer somehow!
        AeRenderer* m_renderer;

        /// Pointer to the game components that this game is actually using.
        GameComponents* m_game_components;

        void setOrthographicProjection(cameraComponentStructure* t_entityCameraComponentData,float t_left, float t_right, float t_top, float t_bottom, float t_near, float t_far);
        void setPerspectiveProjection(cameraComponentStructure* t_entityCameraComponentData, float t_aspect);

        /// Sets the direction in which the camera will be looking.
        void setViewDirection(cameraComponentStructure* t_entityCameraData, modelComponentStruct* t_entityModelData, worldPositionComponentStruct* t_entityWorldPosition, glm::vec3 t_direction);

        /// Camera locked onto a particular location, or object.
        void setViewTarget(cameraComponentStructure* t_entityCameraData, modelComponentStruct* t_entityModelData, worldPositionComponentStruct* t_entityWorldPosition, glm::vec3 t_target);

        /// Euler angles to specify position of the camera and what it is viewing.
        void setViewYXZ(cameraComponentStructure* t_entityCameraData, modelComponentStruct* t_entityModelData, worldPositionComponentStruct* t_entityWorldPostition);

    };
}