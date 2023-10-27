/// \file ae_component_manager.cpp
/// \brief The script implementing the component manager class.
/// The component manager class is implemented.
#include "ae_component_manager.hpp"

#include <stdexcept>
#include <bits/stdc++.h>

namespace ae_ecs {

	// Initialize the component manager.
	AeComponentManager::AeComponentManager(){
		// Initialize the component ID array with all allowed component IDs
		for (ecs_id i = 0; i < MAX_NUM_COMPONENTS; i++) {
			releaseComponentId(MAX_NUM_COMPONENTS - 1 - i);
		};
	};



	// Destroy the component manager.
	AeComponentManager::~AeComponentManager() {};

	// Release the component ID by incrementing the top of stack pointer and putting the component ID being released
	// at that location.
	void AeComponentManager::releaseComponentId(ecs_id t_componentId) {
		if (m_componentIdStackTop >= MAX_NUM_COMPONENTS - 1) {
			throw std::runtime_error("Component ID Stack Overflow!");
		}
		else {
			m_componentIdStackTop++;
			m_componentIdStack[m_componentIdStackTop] = t_componentId;
		}
	};



	// Allocate a component ID by popping the component ID off the stack, indicated by the top of stack pointer, then
	// decrementing the top of stack pointer to point to the next available component ID.
	ecs_id AeComponentManager::allocateComponentId() {
		if (m_componentIdStackTop <= -1) {
			throw std::runtime_error("Component ID Stack Underflow! No more component IDs to give out!");
		}
		else {
			return m_componentIdStack[m_componentIdStackTop--];

		}
	};



	// Gets the number of available components that can be allocated by returning the current value of the top of stack
	// pointer and adding one since the stack array starts at 0.
	ecs_id AeComponentManager::getNumAvailableComponents() {
		return m_componentIdStackTop + 1;
	};



	// Returns the component signature of a specific entity from the entity component signature array.
	std::bitset<MAX_NUM_COMPONENTS + 1>  AeComponentManager::getComponentSignature(ecs_id t_entityId) {
		return m_entityComponentSignatures[t_entityId];
	};



	// Sets the entity component signature bit to indicate that the entity uses the component.
	void AeComponentManager::entityUsesComponent(ecs_id t_entityId, ecs_id t_componentId) {
		m_entityComponentSignatures[t_entityId].set(t_componentId);

        // Search through systems to see if the system matches the new component signature of the entity. If the system
        // uses the component check to see if that entity was already in its list of entities to update, if  not add it.
        for (ecs_id systemId = 0; systemId < m_systemComponentSignatures.size(); systemId++) {

            // Before changing the entities component signature search which systems the entity is compatible with
            // currently.

            // Need to isolate the entity component signature since the &= operator puts the result back into the
            // left hand variable.
            std::bitset<MAX_NUM_COMPONENTS + 1> entityComponentSignature = m_entityComponentSignatures[t_entityId];
            if (m_systemComponentSignatures[systemId].operator==(
                    entityComponentSignature.operator&=(m_systemComponentSignatures[systemId]))) {

                // Check to ensure that the system also requires the component being removed. Only systems that require
                // the component being removed should be impacted by the entity removing the specified component.
                if(m_systemComponentSignatures[systemId].test(t_componentId)){

                    // If the entity is currently already in the system's list of entities that have been updated and needs
                    // to be acted upon it should be removed from that list since it no longer has one of the required
                    // components to be compatible with that system.
                    auto it = find(m_systemEntityUpdateSignatures[systemId].begin(),
                                   m_systemEntityUpdateSignatures[systemId].end(),
                                   t_entityId);

                    if (it == m_systemEntityUpdateSignatures[systemId].end()) {
                        m_systemEntityUpdateSignatures[systemId].push_back(t_entityId);
                    }
                }
            };
        };
	};



	// Resets the entity component signature bit to indicate that the entity does not use the component.
	void AeComponentManager::entityErstUsesComponent(ecs_id t_entityId, ecs_id t_componentId) {
        // Search through systems to see if they use the component that is being removed from the entity. If the system
        // uses the component being removed by the entity check to see if that entity was already in its list of
        // entities to update, if it is removed it from that list.
        // The entity will be added to the system's destroyed entities list since it is no longer eligible to be worked
        // upon by that system.
        for (ecs_id systemId = 0; systemId < m_systemComponentSignatures.size(); systemId++) {

            // Before changing the entities component signature search which systems the entity is compatible with
            // currently.

            // Need to isolate the entity component signature since the &= operator puts the result back into the
            // left hand variable.
            std::bitset<MAX_NUM_COMPONENTS + 1> entityComponentSignature = m_entityComponentSignatures[t_entityId];
            if (m_systemComponentSignatures[systemId].operator==(
                    entityComponentSignature.operator&=(m_systemComponentSignatures[systemId]))) {

                // Check to ensure that the system also requires the component being removed. Only systems that require
                // the component being removed should be impacted by the entity removing the specified component.
                if(m_systemComponentSignatures[systemId].test(t_componentId)){

                    // If the entity is currently already in the system's list of entities that have been updated and needs
                    // to be acted upon it should be removed from that list since it no longer has one of the required
                    // components to be compatible with that system.
                    auto it = find(m_systemEntityUpdateSignatures[systemId].begin(),
                                   m_systemEntityUpdateSignatures[systemId].end(),
                                   t_entityId);

                    if (it != m_systemEntityUpdateSignatures[systemId].end()) {
                        m_systemEntityUpdateSignatures[systemId].erase(it);
                    }

                    // Flag that this entity has been destroyed to this system.
                    m_systemEntityDestroyedSignatures[systemId].push_back(t_entityId);
                }
            };
        };

        m_entityComponentSignatures[t_entityId].reset(t_componentId);
    };



	// Set the last bit of the entityComponentSignature high to indicate that the Entity is enabled and systems can work
	// on it.
	void AeComponentManager::enableEntity(ecs_id t_entityId) {
		m_entityComponentSignatures[t_entityId].set(MAX_NUM_COMPONENTS);

// Removed because if a system tracks entities in some way on their own they should deal with the fact that an entity
// was destroyed first before dealing with the newly enabled entity that has the same ID.
//        // Ensure when an entity is re-enabled that it is removed from the list of deleted entities if a previously
//        // deleted entity had the same ID as the new one.
//        for(ecs_id systemId=0; systemId < m_systemComponentSignatures.size() ; systemId++) {
//            auto it = find(m_systemEntityDestroyedSignatures[systemId].begin(),
//                           m_systemEntityDestroyedSignatures[systemId].end(),
//                           t_entityId);
//
//            if (it != m_systemEntityDestroyedSignatures[systemId].end()) {
//                m_systemEntityDestroyedSignatures[systemId].erase(it);
//            }
//        }
	};



	// Unset the last bit of the entityComponentSignature, low, to indicate that the Entity is disabled and systems
	// should not work on it.
	void AeComponentManager::disableEntity(ecs_id t_entityId) {
		m_entityComponentSignatures[t_entityId].reset(MAX_NUM_COMPONENTS);
		// TODO: When the entity is disabled, force the component manager to update applicable systems lists of valid entities to act upon
	};



    // Update the m_systemEntityUpdateSignatures for the entity that had its data updated.
    void AeComponentManager::entitiesComponentUpdated(ecs_id t_entityId, ecs_id t_componentId){
        for(ecs_id systemId=0; systemId < m_systemComponentSignatures.size() ; systemId++) {

            // If the entities component signature matches the system's component signature set the flag that indicates
            // that a component the system requires has been updated for the specific entity.
            // Need to isolate the entity component signature since the &= operator puts the result back into the
            // left hand variable.
            std::bitset<MAX_NUM_COMPONENTS+1> entityComponentSignature = m_entityComponentSignatures[t_entityId];
            if( m_systemComponentSignatures[systemId].operator==(entityComponentSignature.operator&=(m_systemComponentSignatures[systemId]))){
                if(m_systemComponentSignatures[systemId].test(t_componentId)){
                    // Add the entity to the update list if it is unique. No need to have duplicates in the list.
                    if(std::find(m_systemEntityUpdateSignatures[systemId].begin(), m_systemEntityUpdateSignatures[systemId].end(), t_entityId) == m_systemEntityUpdateSignatures[systemId].end()){
                        m_systemEntityUpdateSignatures[systemId].push_back(t_entityId);
                    }
                }
            };
        };
    };



	// Check to see if the bit in the entityComponentSignature of the entity is set high that corresponds to the
	// component. If high then the component is used by the entity.
	bool AeComponentManager::isComponentUsed(ecs_id t_entityId, ecs_id t_componentId) {
		return m_entityComponentSignatures[t_entityId].test(t_componentId);
	};



	// Removes the entity by clearing the component signature of the entity so the next entity that is allocated the
    // same ID as the one to be removed it will not have the same components, and component data, by default.
    void AeComponentManager::destroyEntity(ecs_id t_entityId){
        m_entityComponentSignatures[t_entityId].reset();

        for(ecs_id systemId=0; systemId < m_systemComponentSignatures.size() ; systemId++) {

            // If the entity had been updated by a previous system before being deleted ensure that a future system does
            // not think this entity still exists and needs to be updated.
            auto it = find(m_systemEntityUpdateSignatures[systemId].begin(),
                           m_systemEntityUpdateSignatures[systemId].end(),
                           t_entityId);

            if (it != m_systemEntityUpdateSignatures[systemId].end()) {
                m_systemEntityUpdateSignatures[systemId].erase(it);
            }


            // Flag that this entity has been destroyed to all systems.
            m_systemEntityDestroyedSignatures[systemId].push_back(t_entityId);
        }
        // TODO: With certain memory management there will need to be memory cleanup done here eventually.
    };




    // Registers the system with the component manager, creates a blank entry in the systemComponentSignature. The last
    // bit is set high so when called by the getSystemEntities function only enabled entities are returned.
    void AeComponentManager::registerSystem(ecs_id t_systemId) {

        // Get a systemComponent and systemEntity signature for the system
        m_systemComponentSignatures[t_systemId] = {0};
        setSystemComponentSignature(t_systemId,MAX_NUM_COMPONENTS);

        // Get a systemEntityUpdate signature for the system and ensure on the system's run of all entities will indicate they
        // have updated data.
        m_systemEntityUpdateSignatures[t_systemId] = {1};
    };



	// Sets the system component signature bit to indicate that the system uses the component.
	void AeComponentManager::setSystemComponentSignature(ecs_id t_systemId, ecs_id t_componentId) {
        if (m_systemComponentSignatures.find(t_systemId) != m_systemComponentSignatures.end()){
            m_systemComponentSignatures.find(t_systemId)->second.set(t_componentId);
        } else {
            throw std::runtime_error("Cannot set a system component signature for a system that doesn't exist. Has it"
                                     " been registered?");
        };
	};



	// Resets the system component signature bit to indicate that the system does not use the component.
	void AeComponentManager::unsetSystemComponentSignature(ecs_id t_systemId, ecs_id t_componentId) {
        if (m_systemComponentSignatures.find(t_systemId) != m_systemComponentSignatures.end()){
            m_systemComponentSignatures.find(t_systemId)->second.reset(t_componentId);
        } else {
            throw std::runtime_error("Cannot reset a system component signature for a system that doesn't exist. Has it"
                                     " been registered?");
        };

	};

    void AeComponentManager::clearSystemEntityUpdateSignatures(ecs_id t_systemId){
        m_systemEntityUpdateSignatures[t_systemId].clear();
    };



	// TODO: Implement this function
	void AeComponentManager::removeSystem(ecs_id t_systemId) {
        m_systemComponentSignatures.erase(t_systemId);
	};



	// Compare the system component signature of interest to the entity component signatures and return a list of
    // entities whose component signatures match the system's component signature and the system can act upon.
    std::vector<ecs_id> AeComponentManager::getEnabledSystemsEntities(ecs_id t_systemId) {

        // The set of valid entities for a system to act upon.
        std::vector<ecs_id> enabledEntities;

        auto systemSignaturePair = m_systemComponentSignatures.find(t_systemId);
        if(systemSignaturePair != m_systemComponentSignatures.end()){
            // Loop through the entity component signatures
            for(ecs_id entityId=0; entityId < MAX_NUM_ENTITIES ; entityId++){

                // Need to isolate the entity component signature since the &= operator puts the result back into the
                // left hand variable.
                std::bitset<MAX_NUM_COMPONENTS+1> entityComponentSignature = m_entityComponentSignatures[entityId];

                // If the entities component signature matches the system's component signature add it to the list of entity
                // indexes being returned.
                if( m_systemComponentSignatures[t_systemId].operator==(entityComponentSignature.operator&=(systemSignaturePair->second))){
                    enabledEntities.push_back(entityId);
                };
            };
        }

        return enabledEntities;
	};


    // Returns the vector of entities
    std::vector<ecs_id> AeComponentManager::getUpdatedSystemEntities(ecs_id t_systemId) {
        std::vector<ecs_id> enabledUpdatedEntities;

        // Check if the entities that are to be updated are still enabled when this system is executing.
        for(auto entityId : m_systemEntityUpdateSignatures[t_systemId]){
            if(m_entityComponentSignatures[entityId].test(MAX_NUM_COMPONENTS)){
                enabledUpdatedEntities.push_back(entityId);
            }
        }
        return enabledUpdatedEntities;
    };

    // Returns the vector of entities
    std::vector<ecs_id> AeComponentManager::getDestroyedSystemEntities(ecs_id t_systemId) {
        return m_systemEntityDestroyedSignatures[t_systemId];
    };



    // Compare the signature of the specified component to the entity signatures.
    std::vector<ecs_id> AeComponentManager::getComponentEntities(ecs_id t_componentId){

        // The set of entities that use the specified component.
        std::vector<ecs_id> valid_entities;

        // Create the signature of the component.
        std::bitset<MAX_NUM_COMPONENTS + 1> evalComponentSignature;
        evalComponentSignature.set(t_componentId);

        // Loop through the entity component signatures
        for(ecs_id entityId=0; entityId < MAX_NUM_ENTITIES ; entityId++){
            auto entityComponentSignature = m_entityComponentSignatures[entityId];
            if((entityComponentSignature.operator&=(evalComponentSignature)).any()){
                valid_entities.push_back(entityId);
            };
        };

        return  valid_entities;
    };

}