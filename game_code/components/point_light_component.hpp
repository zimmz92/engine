/*! \file player_controlled_component.hpp
    \brief The script defining the player controlled component.

    The player controlled component is defined and the instance for the game is declared. This component defines
    relevant properties to objects that may be controlled by a player such as a camera, a player character, or a
    vehicle.

*/
#pragma once

#include "ae_ecs.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace ae {

    /// This structure defines the data stored for each entity using the player component.
    struct PointLightComponentStructure {
        glm::vec3 m_color = glm::vec3(1.0f);
        float lightIntensity = 10.0f;
        float radius = 0.1f;
    };


    /// The player controlled component class is derived from the AeComponent template class using the player controlled
    /// component structure.
    class PointLightComponentClass : public ae_ecs::AeComponent<PointLightComponentStructure> {
    public:
        /// The playerControlledComponentClass constructor uses the AeComponent constructor with no additions.
        PointLightComponentClass() : AeComponent() {};

        /// The destructor of the playerControlledComponentClass class. The playerControlledComponentClass destructor
        /// uses the AeComponent constructor with no additions.
        ~PointLightComponentClass() {};

    private:

    protected:

    };
}