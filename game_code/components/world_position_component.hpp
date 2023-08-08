/*! \file world_position_component.hpp
    \brief The script defining the world position component.
    The world position component is defined and the instance for the game is declared.
*/
#pragma once

// libs
#include <glm/gtc/matrix_transform.hpp>
#include "ae_ecs_include.hpp"

namespace ae {

    /// The world position of the entity in spherical coordinates.
    struct WorldPositionComponentStruct {
        std::float_t rho = 0.0f;
        std::float_t theta = 0.0f;
        std::float_t phi = 0.0f;
    };

    /// A component that specifies the world position of an entity in spherical coordinates. Using vec3 for storing the
    /// data to improve performance of the system(s) that use this component.
    class WorldPositionComponent : public ae_ecs::AeComponent<WorldPositionComponentStruct> {
    public:
        /// The WorldPositionComponent constructor uses the AeComponent constructor with no additions.
        WorldPositionComponent(ae_ecs::AeECS& t_ecs) : AeComponent(t_ecs) {};

        /// The destructor of the WorldPositionComponent. The WorldPositionComponent destructor
        /// uses the AeComponent constructor with no additions.
        ~WorldPositionComponent() {};

        /// Gets the world position structure and returns it as a glm::vec3.
        /// \param t_entityId The entity ID to get the world position for.
        /// \return The world position as a glm::vec3 where x = rho, y = theta, and z = phi.
        glm::vec3 getWorldPositionVec3(ecs_id t_entityId) {
            // Get a references to the world position of the entity.
            auto worldPosition = this->getDataReference(t_entityId);

            // Convert the world position from the world position struct to a glm::vec3.
            glm::vec3 position = {worldPosition.rho,
                                  worldPosition.theta,
                                  worldPosition.phi};

            // Return the glm::vec3
            return position;
        };

        /// Sets the world position of an entity from a glm::vec3.
        /// \param t_entityId The entity ID for which the world position is to be set.
        /// \param t_vec3WorldPosition The new world position of the entity as a glm::vec3 where x = rho, y = theta, and
        /// z = phi.
        void setWorldPositionVec3(ecs_id t_entityId, glm::vec3 t_vec3WorldPosition) {
            // Get a reference to the world position of the entity.
            WorldPositionComponentStruct& worldPosition = this->getDataReference(t_entityId);

            // Set the world position of the entity based on the given glm::vec3.
            worldPosition.rho = t_vec3WorldPosition.x;
            worldPosition.theta = t_vec3WorldPosition.y;
            worldPosition.phi = t_vec3WorldPosition.z;
        };

    private:

    protected:

    };
}