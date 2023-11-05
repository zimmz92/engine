/// \file ae_simple_render_system.cpp
/// \brief The script implementing the system that general 3D objects in the game.
/// The simple render system is implemented.

#include "ae_model_3d_buffer_system.hpp"

// Standard Libraries
#include <map>

namespace ae {

    // Constructor implementation
    AeModel3DBufferSystem::AeModel3DBufferSystem(ae_ecs::AeECS &t_ecs,
                                                 GameComponents &t_game_components)
            : m_worldPositionComponent{t_game_components.worldPositionComponent},
              m_modelComponent{t_game_components.modelComponent},
              ae_ecs::AeSystem<AeModel3DBufferSystem>(t_ecs) {

        // Register component dependencies
        m_worldPositionComponent.requiredBySystem(this->getSystemId());
        m_modelComponent.requiredBySystem(this->getSystemId());


        // Register system dependencies
        // This is a child system and dependencies, as well as execution, will be handled by the parent system,
        // RendererSystem.
        this->isChildSystem = true;

        // Enable the system so it will run.
        this->enableSystem();
    };


    // Destructor implementation
    AeModel3DBufferSystem::~AeModel3DBufferSystem() {};


    // Set up the system prior to execution. Currently not used.
    void AeModel3DBufferSystem::setupSystem(uint64_t t_frameIndex) {

    };


    // Manages the model matrix data for the 3D SSBO.
    void AeModel3DBufferSystem::executeSystem(std::vector<ecs_id>&  t_materialComponentIds,
                                              std::vector<Entity3DSSBOData>& t_object3DBufferData,
                                              std::map<ecs_id, uint32_t>& t_object3DBufferEntityMap,
                                              PreAllocatedStack<uint64_t,MAX_OBJECTS>& t_object3DBufferDataIndexStack) {

        // Deal with any entities that were deleted between the last time this system ran and now.
        std::vector<ecs_id> destroyedEntities = m_systemManager.getUpdatedSystemEntities(m_systemId);

        // Loop through all the destroyed entities that were compatible with this system.
        for(auto entityId:destroyedEntities){

            // Check to make sure the entity was actually already mapped in the system. If it was then free the buffer
            // index it was using then remove it from the map.
            if(t_object3DBufferEntityMap.find(entityId) != t_object3DBufferEntityMap.end()){
                // Entity was in the map, free its index in the SSBO.
                t_object3DBufferDataIndexStack.push(t_object3DBufferEntityMap[entityId]);

                // Now that the index has been returned it can be removed from the map.
                t_object3DBufferEntityMap.erase(entityId);
            };
        };



        // Get the entities with the required components that have updated since the last time this system was run.
        std::vector<ecs_id> updatedEntities = m_systemManager.getUpdatedSystemEntities(m_systemId);

        // Only are interested in entities that use materials since they are the only entities that will actually be
        // able to be rendered.
        std::vector<ecs_id> renderableUpdatedEntities = m_systemManager.getEntitiesWithSpecifiedComponents(updatedEntities,
                                                                                                           t_materialComponentIds);

        // Loop through all the 3D entities that can be rendered.
        for(auto entityId:renderableUpdatedEntities){

            // Get easy references to the data that will be required.
            const ModelComponentStruct& entityModelData = m_modelComponent.getReadOnlyDataReference(entityId);
            glm::vec3 entityWorldPosition = m_worldPositionComponent.getWorldPositionVec3(entityId);

            // Ensure that the entity actually has a model to render. If it has a material attached to it a model should
            // have been attached as well.
            if (entityModelData.m_model == nullptr){
                // Must be a point light.
                continue;
            }

            // If the entity has not already been assigned a buffer position get one to assign to it.
            auto entitySSBOIndex = t_object3DBufferDataIndexStack.pop();

            // Attempt to put the entity into the map.
            auto entityObjectBufferIterator = t_object3DBufferEntityMap.insert(std::make_pair(entityId, entitySSBOIndex));

            // If the insert was successful then the entity has been mapped to the assigned buffer position and the data
            // at that buffer position can be updated.
            if(entityObjectBufferIterator.second){
                t_object3DBufferData[entitySSBOIndex] = calculateModelMatrixData(entityWorldPosition,
                                                                                 entityModelData.rotation,
                                                                                 entityModelData.scale);
            } else {
                // If the insert failed then the entity must already be assigned a position in the buffer so the newly
                // assigned position can be given back. This may seem silly but when there are many entities it is
                // faster to do this than to attempt a find and if it fails to then do an insert.
                t_object3DBufferDataIndexStack.push(entitySSBOIndex);

                // Update the entities model matrix data.
                t_object3DBufferData[entityObjectBufferIterator.first->second] = calculateModelMatrixData(entityWorldPosition,
                                                                                                          entityModelData.rotation,
                                                                                                          entityModelData.scale);
            };
        };

        // Clear the updated entities signatures so if nothing changes they are not updated again.
        m_systemManager.clearSystemEntityUpdateSignatures(m_systemId);
    };


    // Clean up the system after execution. Currently not used.
    void AeModel3DBufferSystem::cleanupSystem() {
    };

    /// Calculates the model, and normal, matrix data.
    /// \param t_translation The translation data for the entity, this normally corresponds to world position but
    /// could be the world position plus an additional offset.
    /// \param t_rotation The rotation of the entity, typically the direction the entity is facing.
    /// \param t_scale The scaling for the entity's model.
    Entity3DSSBOData AeModel3DBufferSystem::calculateModelMatrixData(glm::vec3 t_translation, glm::vec3 t_rotation, glm::vec3 t_scale){

        // Calculate the components of the Tait-bryan angles matrix.
        const float c3 = glm::cos(t_rotation.z);
        const float s3 = glm::sin(t_rotation.z);
        const float c2 = glm::cos(t_rotation.x);
        const float s2 = glm::sin(t_rotation.x);
        const float c1 = glm::cos(t_rotation.y);
        const float s1 = glm::sin(t_rotation.y);
        const glm::vec3 invScale = 1.0f / t_scale;

        // Matrix corresponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 modelMatrix = {
                {
                        t_scale.x * (c1 * c3 + s1 * s2 * s3),
                        t_scale.x * (c2 * s3),
                        t_scale.x * (c1 * s2 * s3 - c3 * s1),
                        0.0f,
                },
                {
                        t_scale.y * (c3 * s1 * s2 - c1 * s3),
                        t_scale.y * (c2 * c3),
                        t_scale.y * (c1 * c3 * s2 + s1 * s3),
                        0.0f,
                },
                {
                        t_scale.z * (c2 * s1),
                        t_scale.z * (-s2),
                        t_scale.z * (c1 * c2),
                        0.0f,
                },
                {
                        t_translation.x,
                        t_translation.y,
                        t_translation.z,
                        1.0f
                }};

        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        // Normal Matrix is calculated to facilitate non-uniform model scaling scale.x != scale.y =! scale.z
        // TODO benchmark if this is faster than just calculating the normal matrix in the shader when there are many objects.
        glm::mat3 normalMatrix = {
                {
                        invScale.x * (c1 * c3 + s1 * s2 * s3),
                        invScale.x * (c2 * s3),
                        invScale.x * (c1 * s2 * s3 - c3 * s1),
                },
                {
                        invScale.y * (c3 * s1 * s2 - c1 * s3),
                        invScale.y * (c2 * c3),
                        invScale.y * (c1 * c3 * s2 + s1 * s3),
                },
                {
                        invScale.z * (c2 * s1),
                        invScale.z * (-s2),
                        invScale.z * (c1 * c2),
                }};

        return {modelMatrix, normalMatrix};
    };

} // namespace ae