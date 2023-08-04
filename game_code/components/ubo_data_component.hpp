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
    struct UboDataFlagsComponentStruct {
        bool hasUboCameraData = false;
        bool hasUboPointLightData = false;
    };


    /// The player controlled component class is derived from the AeComponent template class using the player controlled
    /// component structure.
    class UboDataFlagsComponentClass : public ae_ecs::AeComponent<UboDataFlagsComponentStruct> {
    public:
        /// The PlayerControlledComponentClass constructor uses the AeComponent constructor with no additions.
        UboDataFlagsComponentClass() : AeComponent() {};

        /// The destructor of the PlayerControlledComponentClass class. The PlayerControlledComponentClass destructor
        /// uses the AeComponent constructor with no additions.
        ~UboDataFlagsComponentClass() {};

    private:

    protected:

    };
}