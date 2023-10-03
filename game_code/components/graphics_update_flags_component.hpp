/*! \file graphics_update_flags_component.hpp
    \brief The script defining the graphics update flags component.
    The graphics update flags component is defined and allows entities to indicate what has changed from a graphics
    perspective so that the render system can update their data on the GPU accordingly.
*/
#pragma once

#include "ae_ecs_include.hpp"

namespace ae {

    /// This structure defines the data stored for each entity using the graphics update flags component.
    struct GraphicsUpdateFlagsComponentStruct {
        bool updatedPosition = true;
        bool updatedModel = true;
        bool updatedMaterial = true;
    };


    /// The graphics update flags class is derived from the AeComponent template class using the graphics update flags
    /// component structure.
    class GraphicsUpdateFlagsComponent : public ae_ecs::AeComponent<GraphicsUpdateFlagsComponentStruct> {
    public:
        /// The GraphicsUpdateFlagsComponent constructor uses the AeComponent constructor with no additions.
        GraphicsUpdateFlagsComponent(ae_ecs::AeECS& t_ecs) : AeComponent(t_ecs) {};

        /// The destructor of the GraphicsUpdateFlagsComponent. The GraphicsUpdateFlagsComponent destructor
        /// uses the AeComponent constructor with no additions.
        ~GraphicsUpdateFlagsComponent() {};

    private:

    protected:

    };
}