// TODO: Responsible for ensuring systems are executed in the proper order, handing out system IDs

/*! \file ae_system_manager.hpp
    \brief The script defining the system manager.

    The system manager is defined.

*/
#pragma once

#include "ae_ecs_constants.hpp"

#include <cstdint>
#include <bitset>
#include <array>
#include <memory>

namespace ae_ecs {

    class AeSystemManager {

        /// component type ID counter variable
        static inline ecs_id systemIdCount = 0;

    public:

        AeSystemManager();
        ~AeSystemManager();

        void releaseSystemId(ecs_id t_systemId);
        ecs_id allocateSystemId();

        ///  A function that sets the field in the system dependency signature corresponding to required predecessor system.
        /// \param t_systemId The ID of the system.
        /// \param t_predecessorSystemId The ID of the system in which the system specified by t_systemId is dependent on.
        void setSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId) {
            m_systemDependencySignatures[t_systemId].set(t_predecessorSystemId);
        };

        ///  A function that unsets the field in the system dependency signature corresponding to required predecessor system.
        /// \param t_systemId The ID of the system.
        /// \param t_predecessorSystemId The ID of the system in which the system specified by t_systemId was dependent on.
        void unsetSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId) {
            m_systemDependencySignatures[t_systemId].reset(t_predecessorSystemId);
        };

        /// Enables management of the system
        /// \param t_systemId The ID of the system
        void enableSystem(ecs_id t_systemId) {
            m_systemDependencySignatures[t_systemId].set(MAX_NUM_SYSTEMS);
            // TODO: When the entity is set live, force the component manager to update applicable systems lists of valid entities to act upon
        };

        /// Disables system from be being managed
        /// \param t_systemId The ID of the system
        void disableSystem(ecs_id t_systemId) {
            m_systemDependencySignatures[t_systemId].reset(MAX_NUM_SYSTEMS);
            // TODO: When the entity is disabled, force the component manager to update applicable systems lists of valid entities to act upon
        };

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

    inline AeSystemManager ecsSystemManager;
}