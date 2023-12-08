/// \file ae_entity_manager.cpp
/// \brief The script implementing the entity manager class.
/// The entity manager class is implemented.
#include "ae_entity_manager.hpp"

#include <stdexcept>

namespace ae_ecs {

    // Create the component manager and initialize the entity ID stack.
    AeEntityManager::AeEntityManager(AeComponentManager& t_componentManager) : m_componentManager{t_componentManager} {
        m_livingEntities = new bool[MAX_NUM_ENTITIES]();
    };



    // Destroy the entity manager.
    AeEntityManager::~AeEntityManager() {
        delete[] m_livingEntities;
        m_livingEntities = nullptr;
    };



    // Release the entity ID by incrementing the top of stack pointer and putting the entity ID being released
    // at that location.
    void AeEntityManager::unRegisterEntity(ecs_id t_entityId) {
        m_entityIdStack.push(t_entityId);
        m_livingEntities[t_entityId] = false;
    };



    // Allocate an entity ID by popping the entity ID off the stack, indicated by the top of stack pointer, then
    // decrementing the top of stack pointer to point to the next available entity ID.
    ecs_id AeEntityManager::registerEntity() {
        // Get the new entity ID being allocated from the top of the stack and add the popped entity ID to the living
        // entities array then decrement the stack counter.
        ecs_id allocatedId = m_entityIdStack.pop();
        m_livingEntities[allocatedId] = true;
        return allocatedId;
    };



    // Tell the component manager that this entity has been enabled.
    void AeEntityManager::enableEntity(ecs_id t_entityId) {
        m_componentManager.enableEntity(t_entityId);
    };



    // Tell the component manager that this entity has been enabled.
    void AeEntityManager::disableEntity(ecs_id t_entityId) {
        m_componentManager.disableEntity(t_entityId);
    };



    // Free up the entity ID and ensure the component manager cleans up entity data appropriately.
    void AeEntityManager::destroyEntity(ecs_id t_entityId){
        m_componentManager.destroyEntity(t_entityId);
        unRegisterEntity(t_entityId);
    };

    //
    void AeEntityManager::destroyAllEntities(){
        for(ecs_id entityId=0; entityId<MAX_NUM_ENTITIES;entityId++){
            if(m_livingEntities[entityId]){
                destroyEntity(entityId);
            };
        };
    };



    // Returns the variable that tracks the number of living entities.
    bool* AeEntityManager::getEnabledEntities() { return m_livingEntities; };
}