/// \file ui_component.hpp
/// \brief Stores the information for a user interface game object.
/// Specifies the information for a user interface game object.
#pragma once

#include "ae_ecs_include.hpp"
#include "ae_model.hpp"

namespace ae {

    /// This structure defines the model data stored for each entity using the model component.
    struct Model2dComponentStruct {

        /// A pointer to the model used by a entity.
        std::shared_ptr<AeModel> m_2d_model{};

        /// Defines the offset of the 2D object from the viewport origin.
        glm::vec2 translation{};

        /// Defines the scaling factors to be applied to the model being used by the entity.
        glm::vec2 scale{ 1.0f, 1.0f};

        /// The rotation of the 2D object in radians.
        float rotation;

        // TODO Placeholder for texture
    };


    /// The model component class is derived from the AeComponent template class using the model component structure.
    class Model2dComponent : public ae_ecs::AeComponent<Model2dComponentStruct> {
    public:
        /// The ModelComponent constructor uses the AeComponent constructor with no additions.
        Model2dComponent(ae_ecs::AeECS& t_ecs) : AeComponent(t_ecs) {};

        /// The destructor of the modelComponent class. The ModelComponent destructor uses the AeComponent
        /// constructor with no additions.
        ~Model2dComponent() {};

    private:

    protected:

    };
}