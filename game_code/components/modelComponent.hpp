#pragma once

#include "ae_ecs.hpp"
#include "ae_model.hpp"

namespace ae {
    struct modelComponentStructure {
        // Pointer to the entities model file
        std::shared_ptr<AeModel> m_model{};

        // The orientation and size that the model displays as
        glm::vec3 translation{}; // (position offset)
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
        glm::vec3 rotation;

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4();
        glm::mat3 normalMatrix();

        // TODO Placeholder for texture
    };

    class positionComponentClass : public AeComponent<positionComponentStructure> {
    public:
        positionComponentClass(AeComponentManager& t_componentManager) : AeComponent(t_componentManager) {};
        ~positionComponentClass() {};

    private:

    protected:

    };

    positionComponentClass positionComponent(ecsComponentManager);
}