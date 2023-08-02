/*! \file player_controlled_component.hpp
    \brief The script defining the player controlled component.

    The player controlled component is defined and the instance for the game is declared. This component defines
    relevant properties to objects that may be controlled by a player such as a camera, a player character, or a
    vehicle.

*/
#pragma once

#include "ae_ecs.hpp"

namespace ae {

    /// This structure defines the data stored for each entity using the player component.
    struct playerControlledComponentStructure {

        /// A flag to indicate if the player is currently controlling the entity with this component.
        bool isCurrentlyControlled = false;

    };


    /// The player controlled component class is derived from the AeComponent template class using the player controlled
    /// component structure.
    class playerControlledComponentClass : public ae_ecs::AeComponent<playerControlledComponentStructure> {
    public:
        /// The playerControlledComponentClass constructor uses the AeComponent constructor with no additions.
        playerControlledComponentClass() : AeComponent() {};

        /// The destructor of the playerControlledComponentClass class. The playerControlledComponentClass destructor
        /// uses the AeComponent constructor with no additions.
        ~playerControlledComponentClass() {};

    private:

    protected:

    };
}