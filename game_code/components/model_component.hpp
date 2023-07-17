/*! \file model_component.hpp
    \brief The script defining the model component.

    The model component is defined and the instance for the game is declared.

*/
#pragma once

#include "ae_model.hpp"

namespace ae {

    //! The model component structure.
    /*!
    * This structure defines the model data stored for each entity using the model component.
    */
    struct modelComponentStructure {

        //! The model pointer.
        /*!
        * A pointer to the model used by a entity.
        */
        std::shared_ptr<AeModel> m_model{};

        //! The scale of the model.
        /*!
        * Defines the scaling factors to be applied to the model being used by the entity.
        */
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

        //! The rotation of the model.
        /*!
        * Defines the rotation of the model used by a entity in radians. 
        * Rotations correspond to Tait-bryan angles of Y(1) - varphi, X(2) - theta, Z(3) - psi.
        */
        glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };


        // TODO Placeholder for texture
    };


    //!  The model component class.
    /*!
    * The model component class is derived from the AeComponent template class using the model component structure.
    */
    class modelComponentClass : public ae_ecs::AeComponent<modelComponentStructure> {
    public:
        //!  The constructor of the modelComponent class.
        /*!
        * The modelComponentClass constructor uses the AeComponent constructor with no additions.
        */
        modelComponentClass(ae_ecs::AeComponentManager& t_componentManager) : AeComponent(t_componentManager) {};

        //!  The destructor of the modelComponent class.
        /*!
        * The modelComponentClass destructor uses the AeComponent constructor with no additions.
        */
        ~modelComponentClass() {};

    private:

    protected:

    };

    //!  The instantiation of the model component for use by the game.
    /*!
    * The instantiation of the model component for use by the game.
    */
    inline modelComponentClass modelComponent(ae_ecs::ecsComponentManager);
}