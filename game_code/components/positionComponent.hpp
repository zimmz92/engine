
#pragma once

#include "ae_ecs.hpp"

namespace ae {
    struct worldPositionComponentStructure {
        std::int64_t r; // Radius
        std::int64_t theta; // Rotation around "z" axis
        std::int64_t phi; // Rotation from "z" axis
    };

    class worldPositionComponentClass : public AeComponent<worldPositionComponentStructure> {
    public:
        worldPositionComponentClass(AeComponentManager& t_componentManager) : AeComponent(t_componentManager) {};
        ~worldPositionComponentClass() {};

    private:

    protected:

    };

    worldPositionComponentClass worldPositionComponent(ecsComponentManager);
}