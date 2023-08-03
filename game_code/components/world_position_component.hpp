/*! \file world_position_component.hpp
    \brief The script defining the world position component.

    The world position component is defined and the instance for the game is declared.

*/
#pragma once

// libs
#include <glm/gtc/matrix_transform.hpp>
#include "ae_ecs.hpp"

namespace ae {

    /// The world position of the entity in spherical coordinates.
    struct worldPositionComponentStruct {
        std::float_t rho = 0.0f;
        std::float_t theta = 0.0f;
        std::float_t phi = 0.0f;
    };

    /// A component that specifies the world position of an entity in spherical coordinates. Using vec3 for storing the
    /// data to improve performance of the system(s) that use this component.
    class worldPositionComponentClass : public ae_ecs::AeComponent<worldPositionComponentStruct> {
    public:
        worldPositionComponentClass() : AeComponent() {};
        ~worldPositionComponentClass() {};

        glm::vec3 getWorldPositionVec3(ecs_id t_entityId) {
            auto worldPosition = this->getDataPointer(t_entityId);
            glm::vec3 position = {worldPosition->rho,
                                  worldPosition->theta,
                                  worldPosition->phi};
            return position;
        };

        void setWorldPositionVec3(ecs_id t_entityId, glm::vec3 t_vec3WorldPosition) {
            auto worldPosition = this->getDataPointer(t_entityId);
            worldPosition->rho = t_vec3WorldPosition.x;
            worldPosition->theta = t_vec3WorldPosition.y;
            worldPosition->phi = t_vec3WorldPosition.z;
        };

    private:

    protected:

    };
}