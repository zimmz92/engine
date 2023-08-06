/*! \file ubo_data_flags_component.hpp
    \brief The script defining the ubo data flags component.
    The ubo data flags component is defined and allows entities to indicate to the ubo system that they have data that
    needs to be put into the ubo.
*/
#pragma once

#include "ae_ecs.hpp"

namespace ae {

    /// This structure defines the data stored for each entity using the ubo data flags component.
    struct UboDataFlagsComponentStruct {
        bool hasUboCameraData = false;
        bool hasUboPointLightData = false;
    };


    /// The ubo data flags component class is derived from the AeComponent template class using the ubo data component
    /// structure.
    class UboDataFlagsComponentClass : public ae_ecs::AeComponent<UboDataFlagsComponentStruct> {
    public:
        /// The UboDataFlags constructor uses the AeComponent constructor with no additions.
        UboDataFlagsComponentClass() : AeComponent() {};

        /// The destructor of the UboDataFlagsClass. The UboDataFlagsClass destructor
        /// uses the AeComponent constructor with no additions.
        ~UboDataFlagsComponentClass() {};

    private:

    protected:

    };
}