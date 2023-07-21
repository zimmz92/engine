/// \file ae_system_base.hpp
/// \brief The script defining the base system class.
/// The base system class is defined. This is required since the system manager needs to directly interact with the
/// system classes to call their update classes.
#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_system_manager.hpp"

#include <cstdint>
#include <vector>
#include <array>


namespace ae_ecs {

    class AeSystemManager;

    /// The base class for a system
    class AeSystemBase {
        friend class AeSystemManager;

    public:

        /// Function to create the system defining a specific system manager
        /// \param t_systemManager The system manager that will manage this system.
        explicit AeSystemBase(AeSystemManager& t_systemManager);

        /// Function to destroy the system
        ~AeSystemBase();

        /// Do not allow this class to be copied (2 lines below)
        AeSystemBase(const AeSystemBase&) = delete;
        AeSystemBase& operator=(const AeSystemBase&) = delete;

        /// Do not allow this class to be moved (2 lines below)
        AeSystemBase(AeSystemBase&&) = delete;
        AeSystemBase& operator=(AeSystemBase&&) = delete;

        /// Get the system ID
        /// \return The ID of the system
        ecs_id getSystemId() const;

        /// Register system dependencies with the system manager
        /// \param t_predecessorSystemId The system ID that this system requires to operate prior to its own operation.
        void registerDependency(ecs_id t_predecessorSystemId);

        /// Get the frequency in which the system should run
        ecs_systemInterval getSystemInterval() const;

        /// Specify the frequency in which the system runs, for instance an object clean-up system may not run every frame
        /// \param t_systemInterval An integer representing the number of systemManager ticks to wait between system execution. 0 = every tick.
        void setSystemInterval(ecs_systemInterval t_systemInterval);

        /// Implements any setup required before executing the main functionality of the system.
        /// This is intentionally left empty for a derivative class to override but allow the system manager to access
        /// this function.
        virtual void systemSetup(){};

        /// Implements the main functionality of the system.
        /// This is intentionally left empty for a derivative class to override but allow the system manager to access
        /// this function.
        virtual void systemRun(){};

        /// Implements any processing or cleanup required after the execution of the main functionality of the system.
        /// This is intentionally left empty for a derivative class to override but allow the system manager to access
        /// this function.
        virtual void systemCleanup(){};

    private:


    protected:
        /// ID for the system
        ecs_id m_systemId;

        /// An integer representing the number of systemManager ticks to wait between system execution. 0 = every tick.
        ecs_systemInterval m_systemInterval = 0;

        /// Pointer to the system manager
        AeSystemManager& m_systemManager;
    };

}