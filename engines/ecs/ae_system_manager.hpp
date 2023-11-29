/// \file ae_system_manager.hpp
/// \brief The script defining the system manager.
/// The system manager is defined. Responsible for ensuring systems are executed in the proper order, handing out system IDs
#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_component_manager.hpp"

#include <cstdint>
#include <bitset>
#include <array>
#include <memory>
#include <unordered_map>
#include <forward_list>
#include <stdexcept>

namespace ae_ecs {

    /// Forward declaration of this class so the code will compile but order of operations is maintained properly.
    class AeSystemBase;

    /// A class that is used to register, prioritize, and run systems.
    class AeSystemManager {

        /// component type ID counter variable
        static inline ecs_id systemIdCount = 0;

    public:

        /// Create the system manager and initialize the system ID stack.
        explicit AeSystemManager(AeComponentManager& t_componentManager);

        /// Destroy the system manager.
        ~AeSystemManager();

        /// Registers the system with the system manager
        /// \return A system ID.
        ecs_id registerSystem();

        ///  A function that sets the field in the system dependency signature corresponding to required predecessor system.
        /// \param t_systemId The ID of the system.
        /// \param t_predecessorSystemId The ID of the system in which the system specified by t_systemId is dependent on.
        void setSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId);

        ///  A function that unsets the field in the system dependency signature corresponding to required predecessor system.
        /// \param t_systemId The ID of the system.
        /// \param t_predecessorSystemId The ID of the system in which the system specified by t_systemId was dependent on.
        void unsetSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId);

        /// Clears the system entity update signature for the specified system. Usually used at the end of a system's
        /// execution loop.
        /// \param t_systemId The ID of the system.
        void clearSystemEntityUpdateSignatures(ecs_id t_systemId);

        /// Clears the system entity destroyed signature for the specified system. Usually used at the end of a system's
        /// execution loop.
        /// \param t_systemId The ID of the system.
        void clearSystemEntityDestroyedSignatures(ecs_id t_systemId);

        /// Enables management of the system
        /// \param t_system A pointer to the system.
        void enableSystem(AeSystemBase* t_system);

        /// Disables system from be being managed
        /// \param t_system A pointer to the system.
        void disableSystem(AeSystemBase* t_system);

        /// Removes a system completely
        /// \param t_system A pointer to the system.
        void destroySystem(AeSystemBase* t_system);

        /// Get the entities that a system may act upon from the component manager.
        /// \param
        std::vector<ecs_id> getEnabledSystemsEntities(ecs_id t_systemId);

        /// Returns a list of enabled, compatible, entities that the system is to utilize that have had data been updated since the
        /// last system run loop.
        /// \param t_systemId The ID of the system to be removed.
        std::vector<ecs_id> getUpdatedSystemEntities(ecs_id t_systemId);

        /// Returns a list of entities that have been destroyed since the system last ran.
        /// \param t_systemId The ID of the system to be removed.
        std::vector<ecs_id> getDestroyedSystemEntities(ecs_id t_systemId);

        /// Returns a list of entity IDs that use one, or more, of the optional components provided.
        /// \param t_entityIds The entities to be check to see if they contain the optional component IDs.
        /// \param t_optionalComponentIds The optional components that the entities must have one or more of to be
        /// included in the returned vector.
        std::vector<ecs_id> getEntitiesWithSpecifiedComponents(std::vector<ecs_id>& t_entityIds, std::vector<ecs_id>& t_optionalComponentIds);

        /// Orders the currently enabled systems to ensure they are executed in the proper order.
        void orderSystems();

        /// Runs the systems that are managed by this system manager
        void runSystems();

    private:

        /// Release the system ID and put it back on the top of the stack.
        /// \param t_systemId The system ID to be released.
        void releaseSystemId(ecs_id t_systemId);

        /// Assign a system ID by taking the next available off the stack.
        /// \return A system ID.
        ecs_id allocateSystemId();

        /// System ID stack and a counter used for the stack
        ecs_id m_systemIdStack[MAX_NUM_SYSTEMS];

        /// The system ID stack current top value pointer.
        ecs_id m_systemIdStackTop = -1;

        /// Array storing the dependencies of systems on each other.
        std::bitset<MAX_NUM_SYSTEMS> m_systemDependencySignatures[MAX_NUM_SYSTEMS] = {0};

        /// Map of enabled systems
        std::unordered_map<ecs_id ,AeSystemBase*> m_enabledSystems;

        /// Systems in the order they shall be executed
        std::forward_list<AeSystemBase*> m_systemExecutionOrder;

        /// The component manager the system manager works with
        AeComponentManager& m_componentManager;

    protected:

    };
}