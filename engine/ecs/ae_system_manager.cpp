/// \file ae_system_manager.cpp
/// \brief The script implementing the system manager class.
/// The system manager class is implemented.
#include <iostream>
#include "ae_system_manager.hpp"
#include "ae_system_base.hpp"

namespace ae_ecs {

    // Create the system manager and initialize the system ID stack.
    AeSystemManager::AeSystemManager(AeComponentManager& t_componentManager) : m_componentManager{t_componentManager} {
    };



    // Destroy the system manager.
    AeSystemManager::~AeSystemManager() {};



    // Give the system an ID and inform the component manager that a new system exists.
    ecs_id AeSystemManager::registerSystem(){
        ecs_id new_system_id = m_systemIdStack.pop();
        m_componentManager.registerSystem(new_system_id);
        return new_system_id;
    };



    // Set the system dependency signature bit that indicates that the system depends on another system.
    void AeSystemManager::setSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId) {
        m_systemDependencySignatures[t_systemId].set(t_predecessorSystemId);
    };



    // Reset the system dependency signature bit that indicates that the system depends on another system.
    void AeSystemManager::unsetSystemDependencySignature(ecs_id t_systemId, ecs_id t_predecessorSystemId) {
        m_systemDependencySignatures[t_systemId].reset(t_predecessorSystemId);
    };


    // Request that the component manager clears the entity update data for the system.
    void AeSystemManager::clearSystemEntityUpdateSignatures(ecs_id t_systemId){
        m_componentManager.clearSystemEntityUpdateSignatures(t_systemId);
    };

    // Request that the component manager clears the entity update data for the system.
    void AeSystemManager::clearSystemEntityDestroyedSignatures(ecs_id t_systemId){
        m_componentManager.clearSystemEntityDestroyedSignatures(t_systemId);
    };



    // Enable the system by adding it to the enabled systems map and alerting the component manager that the system has
    // been enabled. When a system is enabled ensure the systems are ordered correctly.
    void AeSystemManager::enableSystem(AeSystemBase* t_system) {
        m_systemDependencySignatures[t_system->m_systemId].set(MAX_NUM_SYSTEMS-1);
        // Add the system to the unordered map that indicates which systems need to be run.
        m_enabledSystems[t_system->m_systemId] = t_system;
        // Ensure the systems are run in the proper order now that a new system has been added.
        this->orderSystems();
    };



    // Reset the last bit of the systemDependencySignatures high to indicate that the system is disabled and should no
    // longer operate. When a system is disabled make sure to remove that system's entry from the ordered map.
    void AeSystemManager::disableSystem(AeSystemBase* t_system) {
        m_systemDependencySignatures[t_system->m_systemId].reset(MAX_NUM_SYSTEMS-1);

        // Check if any other systems depend on the system do be disabled system.
        bool dependentOnCurrentSystem = false;
        auto dependentSystem = m_systemExecutionOrder.begin();
        for (auto it = m_systemExecutionOrder.begin(); it != m_systemExecutionOrder.end(); ++it) {
            // Create the signature of the system to be disabled.
            std::bitset<MAX_NUM_SYSTEMS> disableSystemSignature;
            disableSystemSignature.set(t_system->m_systemId);

            // Compare the signature of the system to be disabled to the dependencies of the systems in the
            // execution list.
            if( (m_systemDependencySignatures[(*it)->m_systemId].operator&=(disableSystemSignature)).any() ) {
                dependentOnCurrentSystem = true;
                dependentSystem = it;
                break;
            };
        };
        if(dependentOnCurrentSystem){
            std::string errorMessage = std::string("Disabling this system will result in other enabled systems to not "
                                                   "have one of their required systems! System being disabled: ") +
                                       std::string(typeid(t_system).name()) +
                                       std::string(", dependent system: ") +
                                       std::string(typeid(*dependentSystem).name());
            throw std::runtime_error(errorMessage);
        };

        // remove the system to the unordered map that indicates which systems need to be run.
        m_enabledSystems.erase(t_system->m_systemId);

        // Remove the system from the execution order list. Since we already checked that no other enabled system needs
        // this system to operate.
        m_systemExecutionOrder.remove(t_system);
    };



    // Remove the system completely by removing it from the system and component managers.
    void AeSystemManager::destroySystem(AeSystemBase* t_system) {

        disableSystem(t_system);
        m_componentManager.removeSystem(t_system->m_systemId);

        // Give the system ID back to the stack of available systems.
        m_systemIdStack.push(t_system->m_systemId);
    };

    std::vector<ecs_id> AeSystemManager::getEnabledSystemsEntities(ecs_id t_systemId){
        return m_componentManager.getEnabledSystemsEntities(t_systemId);
    };

    std::vector<ecs_id> AeSystemManager::getUpdatedSystemEntities(ecs_id t_systemId){
        return m_componentManager.getUpdatedSystemEntities(t_systemId);
    };

    std::vector<ecs_id> AeSystemManager::getDestroyedSystemEntities(ecs_id t_systemId){
        return m_componentManager.getDestroyedSystemEntities(t_systemId);
    };

    // Orders the currently enabled systems to ensure they are executed in the proper order.
    void AeSystemManager::orderSystems() {
        // Clear the current system order list so the new list can be built up.
        m_systemExecutionOrder.clear();

        // Loop through the enable systems and put them into the execution order list based on their dependencies.
        for(const auto& [systemId , system] : m_enabledSystems ) {
            // If the system is a child system do not add it to the systems to be executed list. Assume parent will
            // handle their children system's execution.
            if(!system->isChildSystem) {
                if (m_systemExecutionOrder.empty()) {
                    m_systemExecutionOrder.push_front(system);
                } else {

                    // Check what other systems currently in the list this system depends on.
                    bool dependentOnListSystem = false;
                    auto lastListSystemDependency = m_systemExecutionOrder.begin();
                    for (auto it = m_systemExecutionOrder.begin(); it != m_systemExecutionOrder.end(); ++it) {
                        // Create the signature of the system already in the execution order list.
                        std::bitset<MAX_NUM_SYSTEMS> listSystemSignature;
                        listSystemSignature.set((*it)->m_systemId);

                        // Compare the signature of the system already in the ordered list to the dependencies of the system
                        // under evaluated for ordering. The system under evaluation needs to be placed in the list after
                        // all systems it requires to operate.
                        if ((listSystemSignature.operator&=(m_systemDependencySignatures[systemId])).any()) {
                            lastListSystemDependency = it;
                            dependentOnListSystem = true;
                        };
                    };

                    // Check what other systems depend on the current system.
                    bool dependentOnCurrentSystem = false;
                    auto firstDependentOnSystem = m_systemExecutionOrder.begin();
                    for (auto it = m_systemExecutionOrder.begin(); it != m_systemExecutionOrder.end(); ++it) {
                        // Create the signature of the system under evaluation.
                        std::bitset<MAX_NUM_SYSTEMS> evalSystemSignature;
                        evalSystemSignature.set(systemId);

                        // Compare the signature of the system under evaluation to the dependencies of the systems in the
                        // ordered list. The system under evaluation needs to be placed in the list before all systems it
                        // that require the current system in order to operate.
                        if ((evalSystemSignature.operator&=(m_systemDependencySignatures[(*it)->m_systemId])).any()) {
                            dependentOnCurrentSystem = true;
                            // Once we have hit the first system that depends on this system we have what is required.
                            break;
                        };
                        // A little odd but what we actually want here is the iterator to point to the element in the
                        // forward list that comes before the element that depends on the system being evaluated so when
                        // emplace_after is used we are actually placing it before the dependent system.
                        firstDependentOnSystem = it;
                    };

                    // Check to make sure a cyclic dependency does not exist aka. two systems depend on each other.
                    if (dependentOnListSystem && dependentOnCurrentSystem) {
                        if (std::distance(lastListSystemDependency, firstDependentOnSystem) < 1) {
                            std::string errorMessage =
                                    std::string("A cyclic system dependency has been found between: ") +
                                    std::string(typeid(*lastListSystemDependency).name()) +
                                    std::string(" and ") +
                                    std::string(typeid(*firstDependentOnSystem).name());
                            throw std::runtime_error(errorMessage);
                        };
                    };

                    // Emplace the current system into the list after it's dependencies but before systems that depend on
                    // it.
                    if ((!dependentOnListSystem && !dependentOnCurrentSystem)) {
                        m_systemExecutionOrder.push_front(system);
                    } else if ((!dependentOnListSystem && dependentOnCurrentSystem) &&
                               (lastListSystemDependency == m_systemExecutionOrder.begin())) {
                        m_systemExecutionOrder.push_front(system);
                    } else {
                        m_systemExecutionOrder.emplace_after(lastListSystemDependency, system);
                    };
                };
            };
        };

#ifdef ECS_DEBUG
        std::string headerString = "System Execution Order:\n";
        std::cout << headerString;
        for (auto executeSystem : m_systemExecutionOrder){
            std::string readBackString = std::to_string(executeSystem->m_systemId) + "\n";
            std::cout << readBackString;
        };
#endif

    };



    // Run the systems in the order specified in the enabled systems
    void AeSystemManager::runSystems(){
        // Loop through the enabled systems and if they are supposed to be run again reset their m_cyclesSinceExecution
        // counter and execute. If not then increment their cyclesSinceExecution counter.
        for(auto & m_System : m_systemExecutionOrder){
            // If the system is a child system do not execute here since the parent system will be handling the
            // execution of this system.
            if(!m_System->isChildSystem) {
                // Check to see if this system is ready to be run again.
                if (m_System->m_cyclesSinceExecution >= m_System->m_executionInterval) {
                    m_System->setupSystem();
                    m_System->executeSystem();
                    m_System->cleanupSystem();
                    m_System->m_cyclesSinceExecution = 0;
                } else {
                    m_System->m_cyclesSinceExecution++;
                };
            };
        };
    };


    // Call the component manager's function to get the entities that contain the desired required and optional components.
    std::vector<ecs_id> AeSystemManager::getEntitiesWithSpecifiedComponents(std::vector<ecs_id>& t_entityIds,
                                                                            std::vector<ecs_id>& t_optionalComponentIds){
        return m_componentManager.getEntitiesWithSpecifiedComponents(t_entityIds,t_optionalComponentIds);
    };
}