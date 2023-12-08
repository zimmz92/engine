/// \file ae_component_manager.cpp
/// \brief The script implementing the component manager class.
/// The component manager class is implemented.
#include "ae_component_manager.hpp"
#include "ae_component_base.hpp"

#include <stdexcept>
#include <bits/stdc++.h>

namespace ae_ecs {

	// Initialize the component manager.
	AeComponentManager::AeComponentManager(){};


	// Destroy the component manager.
	AeComponentManager::~AeComponentManager() {};

	// Release the component ID by incrementing the top of stack pointer and putting the component ID being released
	// at that location.
	void AeComponentManager::releaseComponentId(ecs_id t_componentId) {
        m_componentIdStack.push(t_componentId);
	};



	// Allocate a component ID by popping the component ID off the stack, indicated by the top of stack pointer, then
	// decrementing the top of stack pointer to point to the next available component ID.
	ecs_id AeComponentManager::allocateComponentId(AeComponentBase* t_component) {
            ecs_id allocatedId = m_componentIdStack.pop();
            m_components[allocatedId] = t_component;
			return allocatedId;
	};



	// Returns the component signature of a specific entity from the entity component signature array.
	std::bitset<MAX_NUM_COMPONENTS + 1>  AeComponentManager::getComponentSignature(ecs_id t_entityId) {
		return m_entityComponentSignatures[t_entityId];
	};



	// Sets the entity component signature bit to indicate that the entity uses the component.
	void AeComponentManager::entityUsesComponent(ecs_id t_entityId, ecs_id t_componentId) {

        // Update the entities component signature to indicate that the entity now uses the component.
		m_entityComponentSignatures[t_entityId].set(t_componentId);

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

            // Check to ensure that the system also requires the component being removed. Only systems that require
            // the component being removed should be impacted by the entity removing the specified component.
            if(m_systemComponentSignatures[systemId].test(t_componentId)){

                // Need to isolate the entity component signature since the &= operator puts the result back into the
                // left hand variable.
                std::bitset<MAX_NUM_COMPONENTS + 1> entityComponentSignature = m_entityComponentSignatures[t_entityId];
                if (m_systemComponentSignatures[systemId].operator==(
                        entityComponentSignature.operator&=(m_systemComponentSignatures[systemId]))) {



                    // If the entity is currently already in the system's list of entities that have been updated and needs
                    // to be acted upon it should be removed from that list since it no longer has one of the required
                    // components to be compatible with that system.
                    auto it = find(m_systemEntityUpdateSignatures[systemId].begin(),
                                   m_systemEntityUpdateSignatures[systemId].end(),
                                   t_entityId);

                    if (it != m_systemEntityUpdateSignatures[systemId].end()) {
                        m_systemEntityUpdateSignatures[systemId].erase(it);
                    };

                    // Flag that this entity has been destroyed to this system.
                    m_systemEntityDestroyedSignatures[systemId].push_back(t_entityId);
                };
            };
        };

        m_entityComponentSignatures[t_entityId].reset(t_componentId);
    };



	// Set the last bit of the entityComponentSignature high to indicate that the Entity is enabled and systems can work
	// on it.
	void AeComponentManager::enableEntity(ecs_id t_entityId) {
		m_entityComponentSignatures[t_entityId].set(MAX_NUM_COMPONENTS);
	};



	// Unset the last bit of the entityComponentSignature, low, to indicate that the Entity is disabled and systems
	// should not work on it.
	void AeComponentManager::disableEntity(ecs_id t_entityId) {
		m_entityComponentSignatures[t_entityId].reset(MAX_NUM_COMPONENTS);
	};



    // Update the m_systemEntityUpdateSignatures for the entity that had its data updated.
    void AeComponentManager::entitiesComponentUpdated(ecs_id t_entityId, ecs_id t_componentId){
        for(ecs_id systemId=0; systemId < m_systemComponentSignatures.size() ; systemId++) {

            // If the entities component signature matches the system's component signature set the flag that indicates
            // that a component the system requires has been updated for the specific entity.
            // Need to isolate the entity component signature since the &= operator puts the result back into the
            // left hand variable.
            std::bitset<MAX_NUM_COMPONENTS+1> entityComponentSignature = m_entityComponentSignatures[t_entityId];

            // Ignore if the entity has not yet been enabled. This is so that when an entity is eventually enabled it
            // can be acted upon immediately.
            entityComponentSignature.set(MAX_NUM_COMPONENTS);

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

        // Ensure all the data for the entity is deleted or reset properly for its components.
        for(auto component:m_components){
            if(m_entityComponentSignatures[t_entityId].test(component.second->getComponentId())){
                // Let all the components that this entity uses know that it no longer uses it and let the component
                // clean up the entity as needed.
                component.second->unrequiredByEntity(t_entityId);
            };
        }

        // Reset the component signature for the entity so the next entity that is assigned the ID has a fresh slate. If
        // the for loop above worked properly this should only be resetting the last bit.
        m_entityComponentSignatures[t_entityId].reset();
    };




    // Registers the system with the component manager, creates a blank entry in the systemComponentSignature. The last
    // bit is set high so when called by the getSystemEntities function only enabled entities are returned.
    void AeComponentManager::registerSystem(ecs_id t_systemId) {

        // Get a systemComponent and systemEntity signature for the system
        m_systemComponentSignatures[t_systemId] = {0};
        m_systemComponentSignatures[t_systemId].set(MAX_NUM_COMPONENTS);

        // Get a systemEntityUpdate signature for the system.
        m_systemEntityUpdateSignatures[t_systemId] = {};
        m_systemEntityDestroyedSignatures[t_systemId] = {};
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


    // Clear the system's entity destroyed signature so the entities do not keep attempting to be destroyed.
    void AeComponentManager::clearSystemEntityDestroyedSignatures(ecs_id t_systemId){
        m_systemEntityDestroyedSignatures[t_systemId].clear();
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

    std::vector<ecs_id> AeComponentManager::getEntitiesWithSpecifiedComponents(std::vector<ecs_id>& t_entityIds, std::vector<ecs_id>& t_optionalComponentIds){

        // The set of entities that use one or more of the specified optional components.
        std::vector<ecs_id> compatibleComponentEntities;

        // Create a signature for the optional components.
        std::bitset<MAX_NUM_COMPONENTS + 1> optionalComponentsSignature = {0};

        // Set the optional component IDs.
        for(auto componentId:t_optionalComponentIds){
            optionalComponentsSignature.set(componentId);
        };

        // Loop through the provided entities to find the ones that have one or more of the optional components.
        for(auto entityId : t_entityIds){
            // Need to isolate the entity component signature since the &= operator puts the result back into the
            // left hand variable.
            std::bitset<MAX_NUM_COMPONENTS+1> entityComponentSignature = m_entityComponentSignatures[entityId];

            // If the entities component signature has any of the optional components then it will be added to the
            // returned list of valid entities.
            if((entityComponentSignature.operator&=(optionalComponentsSignature)).any()){
                compatibleComponentEntities.push_back(entityId);
            };
        };

        return compatibleComponentEntities;
    };

}