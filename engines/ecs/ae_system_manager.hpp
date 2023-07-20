/// \file ae_system_manager.hpp
/// \brief The script defining the system manager.
/// The system manager is defined. Responsible for ensuring systems are executed in the proper order, handing out system IDs
#pragma once

#include "ae_ecs_constants.hpp"

#include <cstdint>
#include <bitset>
#include <array>
#include <memory>

namespace ae_ecs {

    /// A class that is used to register, prioritize, and run systems.
    class AeSystemManager {

        /// component type ID counter variable
        static inline ecs_id systemIdCount = 0;

    public:

        /// Create the system manager and initialize the system ID stack.
        AeSystemManager();

        /// Destroy the system manager.
        ~AeSystemManager();

        /// Release the system ID and put it back on the top of the stack.
        /// \param t_systemId The system ID to be released.
        void releaseSystemId(ecs_id t_systemId);

        /// Assign a system ID by taking the next available off the stack.
        /// \return A system ID.
        ecs_id allocateSystemId();

        ///  A function that sets the field in the system dependency signature corresponding to required predecessor system.
        /// \param t_systemId The ID of the system.
        /// \param t_predecessorSystemId The ID of the system in which the system specified by t_systemId is dependent on.
        void setSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId);

        ///  A function that unsets the field in the system dependency signature corresponding to required predecessor system.
        /// \param t_systemId The ID of the system.
        /// \param t_predecessorSystemId The ID of the system in which the system specified by t_systemId was dependent on.
        void unsetSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId);

        /// Enables management of the system
        /// \param t_systemId The ID of the system
        void enableSystem(ecs_id t_systemId);

        /// Disables system from be being managed
        /// \param t_systemId The ID of the system
        void disableSystem(ecs_id t_systemId);

    private:

        /// System ID stack and a counter used for the stack
        ecs_id m_systemIdStack[MAX_NUM_SYSTEMS];

        /// The system ID stack current top value pointer.
        ecs_id m_systemIdStackTop = -1;

        /// Vector storing the dependencies of systems on each other. This is used to determine the order of systems
        /// execution, last bit is to indicate that the system is active.
        std::bitset<MAX_NUM_SYSTEMS+1> m_systemDependencySignatures[MAX_NUM_SYSTEMS] = {0};

    protected:

    };

    /// Declare the default system manager for the ecs.
    inline AeSystemManager ecsSystemManager;
}