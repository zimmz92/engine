/*! \file point_light_component.hpp
    \brief The script defining the point light component.
    The point light component is defined. This component provides point light properties to game entities.
*/
#pragma once

#include "ae_ecs.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace ae {

    /// This structure defines the data stored for each entity using the point light component.
    struct PointLightComponentStruct {
        glm::vec3 m_color = glm::vec3(1.0f);
        float lightIntensity = 10.0f;
        float radius = 0.1f;
    };


    /// The PointLightComponent class is derived from the AeComponent template class using the
    /// PointLightComponentStruct structure.
    class PointLightComponent : public ae_ecs::AeComponent<PointLightComponentStruct> {
    public:
        /// The PointLightComponent constructor uses the AeComponent constructor with no additions.
        PointLightComponent(ae_ecs::AeECS& t_ecs) : AeComponent(t_ecs) {};

        /// The destructor of the PointLightComponent class. The PointLightComponent destructor
        /// uses the AeComponent constructor with no additions.
        ~PointLightComponent() {};

    private:

    protected:

    };
}