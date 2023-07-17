/*! \file position_component.hpp
    \brief The script defining the model component.

    The model compoent is defined and the instance for the game is declared.

*/
#pragma once

// libs
#include <glm/gtc/matrix_transform.hpp>

namespace ae {
    //! A component that specifies the world position of an entity in spherical coordinates.
    /*!
    * The world position vector is specified in spherical coordinates where:
    * worldPosition[0] = rho, the distance from origin
    * worldPosition[1] = theta, the angle in xy plane with respect to the x-axis
    * worldPosition[2] = phi, the angle with respect to the z-axis
    */
    class worldPositionComponentClass : public ae_ecs::AeComponent<glm::vec3> {
    public:
        worldPositionComponentClass(ae_ecs::AeComponentManager& t_componentManager) : AeComponent(t_componentManager) {};
        ~worldPositionComponentClass() {};

    private:

    protected:

    };

    inline worldPositionComponentClass worldPositionComponent(ae_ecs::ecsComponentManager);
}