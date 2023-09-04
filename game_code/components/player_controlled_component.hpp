/*! \file player_controlled_component.hpp
    \brief The script defining the player controlled component.
    The player controlled component is defined and the instance for the game is declared. This component defines
    relevant properties to objects that may be controlled by a player such as a camera, a player character, or a
    vehicle.
*/
#pragma once

#include "ae_ecs_include.hpp"

namespace ae {

    /// This structure defines the data stored for each entity using the player component.
    struct PlayerControlledComponentStruct {

        /// A flag to indicate if the player is currently controlling the entity with this component.
        bool isCurrentlyControlled = false;

    };


    /// The player controlled component class is derived from the AeComponent template class using the player controlled
    /// component structure.
    class PlayerControlledComponent : public ae_ecs::AeComponent<PlayerControlledComponentStruct> {
    public:
        /// The PlayerControlledComponent constructor uses the AeComponent constructor with no additions.
        PlayerControlledComponent(ae_ecs::AeECS& t_ecs) : AeComponent(t_ecs) {};

        /// The destructor of the PlayerControlledComponent class. The PlayerControlledComponent destructor
        /// uses the AeComponent constructor with no additions.
        ~PlayerControlledComponent() {};

    private:

    protected:

    };
}