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
        void registerSystemDependency(ecs_id t_predecessorSystemId);

        /// Get the frequency in which the system should run
        ecs_systemInterval getExecutionInterval() const;

        /// Specify the frequency in which the system runs, for instance an object clean-up system may not run every frame
        /// \param t_systemInterval An integer representing the number of systemManager ticks to wait between system execution. 0 = every tick.
        void setExecutionInterval(ecs_systemInterval t_systemInterval);

        /// Enables this system. Tells the system manager that this system should execute.
        void enableSystem();

        /// Disables this system. Tells the system manager that this system should no longer execute.
        void disableSystem();

        /// Tells the system manager that this system depends on another system to execute prior to it's own execution.
        /// \param t_systemId The system ID of the system that this system requires as a predecessor. Systems are
        /// assumed to be independent of other systems by default.
        void dependsOnSystem(ecs_id t_systemId);

        /// Tells the system manager that this system no longer depends on another system to execute prior to it's own
        /// execution. Systems are assumed to be independent of other systems by default.
        /// \param t_systemId The system ID of the system that this system no longer requires as a predecessor.
        void independentOfSystem(ecs_id t_systemId);

        /// Implements any setup required before executing the main functionality of the system.
        /// This is intentionally left empty for a derivative class to override but allow the system manager to access
        /// this function.
        virtual void setupSystem(){};

        /// Implements the main functionality of the system.
        /// This is intentionally left empty for a derivative class to override but allow the system manager to access
        /// this function.
        virtual void executeSystem(){};

        /// Implements any processing or cleanup required after the execution of the main functionality of the system.
        /// This is intentionally left empty for a derivative class to override but allow the system manager to access
        /// this function.
        virtual void cleanupSystem(){};

    private:


    protected:
        /// ID for the system
        ecs_id m_systemId;

        /// An integer representing the number of systemManager ticks to wait between system execution. 0 = every tick.
        ecs_systemInterval m_executionInterval = 0;

        /// Counter that keeps track of how many cycles have past since it was last run.
        ecs_systemInterval m_cyclesSinceExecution = 0;

        /// Pointer to the system manager
        AeSystemManager& m_systemManager;
    };

}