/// \file ae_system.hpp
/// \brief The script defining the template system class.
/// The template system class is defined.
#pragma once

#include "ae_ecs.hpp"
#include "ae_system_base.hpp"

#include <cstdint>
#include <vector>
#include <array>


namespace ae_ecs {

    /// The base class for a system
    template <typename T>
    class AeSystem : public AeSystemBase {

    public:

        /// Function to create the system defining a specific system manager
        /// \param t_systemManager The system manager that will manage this system.
        explicit AeSystem(AeECS& t_ecs) : AeSystemBase(t_ecs.m_ecsSystemManager) {};

        /// Function to destroy the system
        ~AeSystem() {};

        /// Implements any setup required before executing the main functionality of the system.
        /// This is intentionally left empty for the actual system implementation to override.
        virtual void setupSystem() override{};

        /// Implements the main functionality of the system.
        /// This is intentionally left empty for the actual system implementation to override.
        virtual void executeSystem() override{};

        /// Implements any processing or cleanupSystem required after the execution of the main functionality of the system.
        /// This is intentionally left empty for the actual system implementation to override.
        virtual void cleanupSystem() override{};

    private:


    protected:

    };

}