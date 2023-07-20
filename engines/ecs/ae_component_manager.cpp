/// \file ae_component_manager.cpp
/// \brief The script implementing the component manager class.
/// The component manager class is implemented.
#include "ae_component_manager.hpp"

#include <stdexcept>

namespace ae_ecs {

	// Initialize the component manager.
	AeComponentManager::AeComponentManager() {
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
			throw std::runtime_error("Entity ID Stack Overflow!");
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
			throw std::runtime_error("Entity ID Stack Underflow! No more component IDs to give out!");
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
	void AeComponentManager::setEntityComponentSignature(ecs_id t_entityId, ecs_id t_componentId) {
		m_entityComponentSignatures[t_entityId].set(t_componentId);
		// TODO: When the entity sets a component, and is live, force the component manager to update applicable systems lists of valid entities to act upon
	};

	// Resets the entity component signature bit to indicate that the entity does not use the component.
	void AeComponentManager::unsetEntityComponentSignature(ecs_id t_entityId, ecs_id t_componentId) {
		m_entityComponentSignatures[t_entityId].reset(t_componentId);
		// TODO: When the entity removes a component force the component manager to update applicable systems lists of valid entities to act upon
	};

	// Set the last bit of the entityComponentSignature high to indicate that the Entity is enabled and systems can work
	// on it.
	void AeComponentManager::enableEntity(ecs_id t_entityId) {
		m_entityComponentSignatures[t_entityId].set(MAX_NUM_COMPONENTS);
		// TODO: When the entity is set live, force the component manager to update applicable systems lists of valid entities to act upon
	};

	// Unset the last bit of the entityComponentSignature, low, to indicate that the Entity is disabled and systems
	// should not work on it.
	void AeComponentManager::disableEntity(ecs_id t_entityId) {
		m_entityComponentSignatures[t_entityId].reset(MAX_NUM_COMPONENTS);
		// TODO: When the entity is disabled, force the component manager to update applicable systems lists of valid entities to act upon
	};

	// Check to see if the bit in the entityComponentSignature of the entity is set high that corresponds to the
	// component. If high then the component is used by the entity.
	bool AeComponentManager::isComponentUsed(ecs_id t_entityId, ecs_id t_componentId) {
		return m_entityComponentSignatures[t_entityId].test(t_componentId);
	};

	// Removes the entity by clearing the component signature of the entity so the next entity that is allocated the
	// same ID as the one to be removed it will not have the same components, and component data, by default.
	void AeComponentManager::removeEntity(ecs_id t_entityId){
		m_entityComponentSignatures[t_entityId].reset();
	};

	// Sets the system component signature bit to indicate that the system uses the component.
	void AeComponentManager::setSystemComponentSignature(ecs_id t_systemId, ecs_id t_componentId) {
		m_systemComponentSignatures[t_systemId].set(t_componentId);
	};

	// Resets the system component signature bit to indicate that the system does not use the component.
	void AeComponentManager::unsetSystemComponentSignature(ecs_id t_systemId, ecs_id t_componentId) {
		m_systemComponentSignatures[t_systemId].reset(t_componentId);
	};

	// TODO: Implement this function
	void AeComponentManager::removeSystem(ecs_id t_systemId) {

	};

	// TODO: Implement function that compares system component signatures to the entity component signatures and returns a list of valid entities to the systems to act upon.
	void AeComponentManager::updateSystemsEntities() {

	};

	//  Give each component class a unique type ID at runtime and increment the counter used generate the unique IDs.
	template <class T>
	const ecs_id AeComponentManager::allocateComponentTypeId() {
		static const ecs_id staticTypeId{ componentIdCount++ };
		return staticTypeId;
	};

}