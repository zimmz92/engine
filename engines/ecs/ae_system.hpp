/// \file ae_system.hpp
/// \brief The script defining the template system class.
/// The template system class is defined.
#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_system_manager.hpp"

#include <cstdint>
#include <vector>
#include <array>


namespace ae_ecs {

    /// The base class for a system
    class AeSystem {

    public:

        /// Function to create the system using the default system and component managers of the ecs engine
        AeSystem() : AeSystem(ecsSystemManager) {};

        /// Function to create the system defining a specific system manager
        /// \param t_systemManager The system manager that will manage this system.
        explicit AeSystem(AeSystemManager& t_systemManager) : m_systemManager{ t_systemManager} {
            // Get an ID for the system from the system manager
            m_systemId = m_systemManager.allocateSystemId();
        };

        /// Function to destroy the system
        ~AeSystem() {
            m_systemManager.releaseSystemId(m_systemId);
        };

        /// Do not allow this class to be copied (2 lines below)
        AeSystem(const AeSystem&) = delete;
        AeSystem& operator=(const AeSystem&) = delete;

        /// Do not allow this class to be moved (2 lines below)
        AeSystem(AeSystem&&) = delete;
        AeSystem& operator=(AeSystem&&) = delete;

        /// Get the system ID
        /// \return The ID of the system
        ecs_id getSystemId() const { return m_systemId; }

        /// Register system dependencies with the system manager
        /// \param t_predecessorSystemId The system ID that this system requires to operate prior to its own operation.
        void registerDependency(ecs_id t_predecessorSystemId){
            m_systemManager.setSystemDependencySignature(m_systemId,t_predecessorSystemId);
        };

        /// Get the frequency in which the system should run
        ecs_systemInterval getSystemInterval() const {return m_systemInterval; };

        /// Specify the frequency in which the system runs, for instance an object clean-up system may not run every frame
        /// \param t_systemInterval An integer representing the number of systemManager ticks to wait between system execution. 0 = every tick.
        void setSystemInterval(ecs_systemInterval t_systemInterval){
            m_systemInterval = t_systemInterval;
        };

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