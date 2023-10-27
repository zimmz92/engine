/// \file ae_material3d.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_ecs_include.hpp"
#include "ae_pipeline.hpp"
#include "ae_model.hpp"
#include "ae_material3d_base.hpp"
#include "game_components.hpp"

// libraries

//std
#include <map>
#include <memory>
#include <iostream>


namespace ae {

    template <typename T>
    class AeMaterial3D : public AeMaterial3DBase{

    public:

        /// This component is used to allow entities to store specific information required for this material to
        /// correctly function.
        class MaterialComponent : public ae_ecs::AeComponent<T>{
        public:
            /// The MaterialComponent constructor uses the AeComponent constructor with no additions.
            explicit MaterialComponent(ae_ecs::AeECS& t_ecs) : ae_ecs::AeComponent<T>(t_ecs)  {
            };

            /// The destructor of the MaterialComponent class. The MaterialComponent destructor
            /// uses the AeComponent constructor with no additions.
            ~MaterialComponent() = default;
        };


        /// This system is used to organized all the models and entity SSBO index data that use this material.
        class MaterialSystem : public ae_ecs::AeSystem<MaterialSystem>{
        public:
            // Constructor implementation
            MaterialSystem(ae_ecs::AeECS& t_ecs, GameComponents& t_game_components, MaterialComponent& t_materialComponent) : ae_ecs::AeSystem<MaterialSystem>(t_ecs), m_modelComponent{t_game_components.modelComponent}, m_materialComponent{t_materialComponent} {
                // Register component dependencies
                m_modelComponent.requiredBySystem(this->m_systemId);
                m_materialComponent.requiredBySystem(this->m_systemId);

                // There will be a call in the renderer system to all the material systems.
                this->isChildSystem = true;

                // Enable the system
                this->enableSystem();
            };

            // Destructor implementation
            ~MaterialSystem() = default;

            // Set up the system prior to execution. Currently not used.
            void setupSystem() {};

            // Update the time difference between the current execution and the previous.
            void executeSystem(int t_frameIndex) {

                // Delete the destroyed entities from the list first.
                std::vector<ecs_id> destroyedEntityIds = this->m_systemManager.getDestroyedSystemEntities(this->m_systemId);
                for(ecs_id entityId: destroyedEntityIds){

                    // Check if entity's model is already in the unique model map.
                    auto entityModel = m_modelComponent.getReadOnlyDataReference(entityId);
                    auto pos = m_uniqueModelMap.find(entityModel.m_model);
                    if (pos != m_uniqueModelMap.end()) {
                        // The model was already in the map, as it should have been. Now remove the entity from the map.
                        pos->second.erase(entityId);
                    }

                    //TODO: Need to ensure that the material information gets destroyed as well.
                }

                // Get the entities that have been updated that use this system.
                std::vector<ecs_id> updatedEntityIds = this->m_systemManager.getUpdatedSystemEntities(this->m_systemId);

                // Check to see if the entity is already in the map and if it's model is already correct. If the current
                // model association is no longer correct then remove it then add the new one. Otherwise, their material
                // properties for their model must have changed and need to be updated.
                for(ecs_id entityId: updatedEntityIds){

                    // Get the model for the entity that was updated.
                    auto entityModel = m_modelComponent.getReadOnlyDataReference(entityId);

                    // Create a command for the updated component.
                    VkDrawIndexedIndirectCommand entityCommand;
                    entityCommand.firstIndex = 0;
                    entityCommand.indexCount = entityModel.m_model->getIndexCount();
                    entityCommand.instanceCount = 1;
                    entityCommand.firstInstance = entityModel.m_modelMatrixIndex;
                    entityCommand.vertexOffset = 0;

                    // Check if entity's model is already in the unique model map.
                    auto modelInsertionResult = m_uniqueModelMap.insert( std::make_pair(entityModel.m_model, std::map<ecs_id,VkDrawIndexedIndirectCommand>()));
                    if (modelInsertionResult.second) {
                        // The model was not already in the map and was added. Now add the entity ID and model matrix
                        // index relation.
                        modelInsertionResult.first->second.insert(std::make_pair(entityId,entityCommand));
                    } else {
                        // The model was in the map already. Now see if the entity-model relation is already in the map.
                        auto entityInsertionResult = modelInsertionResult.first->second.insert(std::make_pair(entityId,entityCommand));
                        if(!entityInsertionResult.second){
                            entityInsertionResult.first->second = entityCommand;
                        }
                    }

                    //TODO: Need to ensure that the material information gets updated as well.
                }
            };

            // Clean up the system after execution. Currently not used.
            void cleanupSystem() {
                this->m_systemManager.clearSystemEntityUpdateSignatures(this->m_systemId);
            };

        private:
            /// A reference to
            ModelComponent& m_modelComponent;

            /// A reference to the material component this system is associated with.
            MaterialComponent& m_materialComponent;

            /// A vector to track unique models, and a list of which entities use them.
            std::map<std::shared_ptr<AeModel>,std::map<ecs_id,VkDrawIndexedIndirectCommand>> m_uniqueModelMap;

            /// Compiles the commands to be called by draw indexed indirect command for each frames.
            VkDrawIndexedIndirectCommand materialDrawIndexedCommands[MAX_FRAMES_IN_FLIGHT][MAX_OBJECTS];
        };

        /// Constructor of the SimpleRenderSystem
        /// \param t_game_components The game components available that this system may require.
        explicit AeMaterial3D(AeDevice &t_aeDevice,
                              GameComponents& t_game_components,
                              VkRenderPass t_renderPass,
                              ae_ecs::AeECS& t_ecs,
                              MaterialShaderFiles& t_materialShaderFiles,
                              VkDescriptorSetLayout t_globalSetLayout,
                              VkDescriptorSetLayout t_textureSetLayout,
                              VkDescriptorSetLayout t_objectSetLayout) :
                m_ecs{t_ecs},
                m_gameComponents{t_game_components},
                AeMaterial3DBase(t_aeDevice,
                                 t_renderPass,
                                 t_materialShaderFiles,
                                 t_globalSetLayout,
                                 t_textureSetLayout,
                                 t_objectSetLayout) {};

        /// Destructor of the SimpleRenderSystem
        ~AeMaterial3D()= default;

        void executeMaterialSystem(int t_frameIndex) override {m_materialSystem.executeSystem(t_frameIndex);};

    private:

        /// Reference to the ECS used for the component and the system associated with this material.
        ae_ecs::AeECS& m_ecs;

        // The game components the system uses.
        GameComponents& m_gameComponents;

    public:
        /// Create a component for the material to track entities that use the material and specify
        /// textures/properties specific to that entity.
        MaterialComponent m_materialComponent{m_ecs};

    private:

        /// Create a system to deal with organizing the models and entity information the material is responsible for
        /// rendering.
        MaterialSystem m_materialSystem{m_ecs,m_gameComponents,m_materialComponent};


    protected:

    };

} // namespace ae