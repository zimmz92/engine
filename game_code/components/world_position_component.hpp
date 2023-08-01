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

    private:

    protected:

    };

    inline worldPositionComponentClass worldPositionComponent;
}