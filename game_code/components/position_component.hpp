/*! \file position_component.hpp
    \brief The script defining the model component.

    The model component is defined and the instance for the game is declared.

*/
#pragma once

// libs
#include <glm/gtc/matrix_transform.hpp>
#include "ae_ecs.hpp"

namespace ae {

    /// The world position of the entity in spherical coordinates.
    struct worldPosition {
        std::float_t rho = 0.0f;
        std::float_t theta = 0.0f;
        std::float_t phi = 0.0f;
    };

    /// A component that specifies the world position of an entity in spherical coordinates. Using vec3 for storing the
    /// data to improve performance of the system(s) that use this component.
    class worldPositionComponentClass : public ae_ecs::AeComponent<glm::vec3> {
    public:
        worldPositionComponentClass() : AeComponent() {};
        ~worldPositionComponentClass() {};

        /// Removing the base class in this case to make setting the world position at entity initialization more
        /// readable by using a structure for the world position then converting it to a glm::vec3 internally.
        void requiredByEntity(ecs_id t_entityId, glm::vec3 t_entityComponentData) = delete;


        /// A function to indicate to the component manager that a specific entity uses a component and store the data
        /// of the entity within the component.
        /// \param t_entityId The ID of the entity using the component.
        /// \param t_entityComponentData The data of the specified entity to be stored within the component.
        void requiredByEntity(ecs_id t_entityId, worldPosition t_entityComponentData) {
            m_componentManager.setEntityComponentSignature(t_entityId, m_componentId);
            updateData(t_entityId, {t_entityComponentData.rho,t_entityComponentData.theta,t_entityComponentData.phi});
            // TODO: If the entity has been enabled alert system manager that this entity uses this component.
        };

    private:

    protected:

    };

    inline worldPositionComponentClass worldPositionComponent;
}