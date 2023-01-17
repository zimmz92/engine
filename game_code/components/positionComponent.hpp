#pragma once

#include "ae_ecs.hpp"

namespace ae {
    struct worldPositionComponentStructure {
        std::int64_t r; // Radius
        std::int64_t theta; // Rotation around "z" axis
        std::int64_t phi; // Rotation from "z" axis
    };

    class worldPositionComponentClass : public ae_ecs::AeComponent<worldPositionComponentStructure> {
    public:
        worldPositionComponentClass(ae_ecs::AeComponentManager& t_componentManager) : AeComponent(t_componentManager) {};
        ~worldPositionComponentClass() {};

    private:

    protected:

    };

    worldPositionComponentClass worldPositionComponent(ae_ecs::ecsComponentManager);
}