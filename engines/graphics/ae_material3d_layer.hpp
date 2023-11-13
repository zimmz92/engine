/// \file ae_material3d.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_ecs_include.hpp"
#include "ae_pipeline.hpp"
#include "ae_model.hpp"
#include "ae_material3d_layer_base.hpp"
#include "game_components.hpp"
#include "pre_allocated_stack.hpp"

// libraries

//std
#include <map>
#include <memory>
#include <iostream>


namespace ae {

    struct TextureSamplerPair{
        std::shared_ptr<AeImage> m_texture = nullptr;
        VkSampler m_sampler= nullptr;
    };

    template <uint32_t numVertTexts, uint32_t numFragTexts, uint32_t numTessTexts, uint32_t numGeometryTexts>
    struct MaterialTextures{

        TextureSamplerPair m_vertexTextures[numVertTexts]{};
        uint32_t m_numVertexTextures = numVertTexts;

        TextureSamplerPair m_fragmentTextures[numFragTexts]{};
        uint32_t m_numFragmentTextures = numFragTexts;

        TextureSamplerPair m_tessellationTextures[numTessTexts]{};
        uint32_t m_numTessellationTextures = numTessTexts;

        TextureSamplerPair m_geometryTextures[numGeometryTexts]{};
        uint32_t m_numGeometryTextures = numGeometryTexts;

        // TODO: make functions to add/get shader image/sampler pairs to enforce only adding information to the
        //  materials where the textures are supposed to exist.
    };

    template <uint32_t numVertTexts, uint32_t numFragTexts, uint32_t numTessTexts, uint32_t numGeometryTexts>
    class AeMaterial3DLayer : public AeMaterial3DLayerBase{
        static_assert(numVertTexts+numFragTexts+numTessTexts+numGeometryTexts<=MAX_TEXTURES_PER_MATERIAL,
                      "The total number of textures specified to create a material is greater than the maximum allowed number "
                      "of textures per material, MAX_TEXTURES_PER_MATERIAL.");
        using T = MaterialTextures<numVertTexts,numFragTexts,numTessTexts,numGeometryTexts>;

    public:

        /// This component is used to allow entities to store specific information required for this material to
        /// correctly function.
        class MaterialLayerComponent : public ae_ecs::AeComponent<T>{
        public:
            /// The MaterialComponent constructor uses the AeComponent constructor with no additions.
            explicit MaterialLayerComponent(ae_ecs::AeECS& t_ecs) : ae_ecs::AeComponent<T>(t_ecs)  {
            };

            /// The destructor of the MaterialComponent class. The MaterialComponent destructor
            /// uses the AeComponent constructor with no additions.
            ~MaterialLayerComponent() = default;
        };


        /// This system is used to organized all the models and entity SSBO index data that use this material.
        class MaterialLayerSystem : public ae_ecs::AeSystem<MaterialLayerSystem>{
        public:
            // Constructor implementation
            MaterialLayerSystem(ae_ecs::AeECS& t_ecs,
                                GameComponents& t_game_components,
                                MaterialLayerComponent& t_materialComponent,
                                AeMaterial3DLayer<numVertTexts,numFragTexts,numTessTexts,numGeometryTexts>& t_material) :
                           ae_ecs::AeSystem<MaterialLayerSystem>(t_ecs),
                                   m_modelComponent{t_game_components.modelComponent},
                                   m_materialComponent{t_materialComponent},
                                   m_worldPositionComponent{t_game_components.worldPositionComponent},
                                   m_material{t_material}{
                // Register component dependencies
                m_modelComponent.requiredBySystem(this->m_systemId);
                m_materialComponent.requiredBySystem(this->m_systemId);
                m_worldPositionComponent.requiredBySystem(this->m_systemId);

                // There will be a call in the renderer system to all the material systems.
                this->isChildSystem = true;

                // Enable the system
                this->enableSystem();
            };

            // Destructor implementation
            ~MaterialLayerSystem() = default;

            // Update the time difference between the current execution and the previous.
            const std::vector<VkDrawIndexedIndirectCommand>& updateMaterialEntities(std::vector<Entity3DSSBOData>& t_entity3DSSBOData,
                               std::map<ecs_id, uint32_t>& t_entity3DSSBOMap,
                               VkDescriptorImageInfo t_imageBuffer[MAX_TEXTURES],
                               std::map<std::shared_ptr<AeImage>,ImageBufferInfo>& t_imageBufferMap,
                               PreAllocatedStack<uint64_t,MAX_TEXTURES>& t_imageBufferStack,
                               uint64_t t_drawIndirectCommandBufferIndex) {

                m_drawIndirectCommandBufferIndex = t_drawIndirectCommandBufferIndex;

                bool remakeCommandMap = false;

                // Delete the destroyed entities from the list first.
                std::vector<ecs_id> destroyedEntityIds = this->m_systemManager.getDestroyedSystemEntities(this->m_systemId);
                for(ecs_id entityId: destroyedEntityIds){
                    // An update occurred to an entity, remake the command array for this material.
                    remakeCommandMap = true;

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

                // For the entities that have updated ensure they exist in the list and their drawIndirect command is
                // updated with the new information.
                for(auto entityId:updatedEntityIds){
                    // An update occurred to an entity, remake the command array for this material.
                    remakeCommandMap = true;

                    // Get the model for the entity that was updated.
                    auto entityModel = m_modelComponent.getReadOnlyDataReference(entityId);

                    // Create a command for the updated component.
                    VkDrawIndexedIndirectCommand entityCommand;
                    entityCommand.firstIndex = 0;
                    entityCommand.indexCount = entityModel.m_model->getIndexCount();
                    entityCommand.instanceCount = 1;
                    entityCommand.firstInstance = t_entity3DSSBOMap[entityId];
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

                    // Get the material properties for the entity.
                    auto entityMaterialProperties = m_materialComponent.getReadOnlyDataReference(entityId);

                    // Loop through each of the different possible textures for each of the possible shaders.
                    // Shaders texture indices will be packed into the 3D SSBO in the order they are called here and in
                    // the order they are placed into the material texture array for the specific material.
                    uint32_t textureIndex = 0;

                    // Update the entities vertex textures.
                    handleShaderImages(t_entity3DSSBOMap[entityId],
                                       textureIndex,
                                       entityId,
                                       entityMaterialProperties.m_vertexTextures,
                                       entityMaterialProperties.m_numVertexTextures,
                                       t_entity3DSSBOData,
                                       t_imageBuffer,
                                       t_imageBufferMap,
                                       t_imageBufferStack);

                    // Update the entities fragment textures.
                    handleShaderImages(t_entity3DSSBOMap[entityId],
                                       textureIndex,
                                       entityId,
                                       entityMaterialProperties.m_fragmentTextures,
                                       entityMaterialProperties.m_numFragmentTextures,
                                       t_entity3DSSBOData,
                                       t_imageBuffer,
                                       t_imageBufferMap,
                                       t_imageBufferStack);

                    // Update the entities Tesselation textures.
                    handleShaderImages(t_entity3DSSBOMap[entityId],
                                       textureIndex,
                                       entityId,
                                       entityMaterialProperties.m_tessellationTextures,
                                       entityMaterialProperties.m_numTessellationTextures,
                                       t_entity3DSSBOData,
                                       t_imageBuffer,
                                       t_imageBufferMap,
                                       t_imageBufferStack);

                    // Update the entities Geometry textures.
                    handleShaderImages(t_entity3DSSBOMap[entityId],
                                       textureIndex,
                                       entityId,
                                       entityMaterialProperties.m_geometryTextures,
                                       entityMaterialProperties.m_numGeometryTextures,
                                       t_entity3DSSBOData,
                                       t_imageBuffer,
                                       t_imageBufferMap,
                                       t_imageBufferStack);
                };

                if(remakeCommandMap){
                    m_entityCommandCount = 0;
                    m_materialDrawIndexedCommands.clear();
                    for(const auto& model : m_uniqueModelMap){
                        for(const auto& entityCommands : model.second){
                            m_materialDrawIndexedCommands.push_back(entityCommands.second);
                            m_entityCommandCount++;
                        }
                    }
                }

                return m_materialDrawIndexedCommands;
            };

            void handleShaderImages(uint32_t& t_entitySSBOIndex,
                                    uint32_t& t_entityTextureIndex,
                                    ecs_id& t_entityId,
                                    TextureSamplerPair t_shaderTextures[],
                                    uint32_t t_numShaderTextures,
                                    std::vector<Entity3DSSBOData>& t_entity3DSSBOData,
                                    VkDescriptorImageInfo t_imageBuffer[MAX_TEXTURES],
                                    std::map<std::shared_ptr<AeImage>,ImageBufferInfo>& t_imageBufferMap,
                                    PreAllocatedStack<uint64_t,MAX_TEXTURES>& t_imageBufferStack){

                // TODO This could be optimized, right now all the texture relations, and the images themselves if
                //  nothing else uses, them for the entity being updated are being removed even if they are still being
                //  used or remained the same.

                // TODO: Might need to remove all information related to the updated entity before updating...
                //  otherwise if the entity has a new texture for something the old one never is dropped.

                for(int i = 0; i<t_numShaderTextures; i++){

                    // Check if the object has a texture. If not set it such that the model is rendered using only its
                    // vertex colors.
                    if(t_shaderTextures[i].m_texture == nullptr){
                        t_entity3DSSBOData[t_entitySSBOIndex].textureIndex[m_material.getMaterialId()][t_entityTextureIndex] = MAX_TEXTURES + 1;
                        //t_entity3DSSBOData[t_entitySSBOIndex].textureIndex = MAX_TEXTURES + 1;
                        t_entityTextureIndex++;
                        continue;
                    }

                    // Search the image buffer map to see if the image has already been added to it.
                    auto imageSearchIterator = t_imageBufferMap.find(t_shaderTextures[i].m_texture);

                    if(imageSearchIterator==t_imageBufferMap.end()){
                        // If the image was not already in the map it is time to add it. First get an available image
                        // buffer index to add the image into the buffer.
                        auto newImageIndex = t_imageBufferStack.pop();

                        // Add the image to the image buffer.
                        t_imageBuffer[newImageIndex].sampler = t_shaderTextures[i].m_sampler;
                        t_imageBuffer[newImageIndex].imageView = t_shaderTextures[i].m_texture->getImageView();
                        t_imageBuffer[newImageIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                        // Add the image and the entity/material relation into the map.
                        // TODO need to make this also account for the sampler used for the texture.....
                        t_imageBufferMap.insert({t_shaderTextures[i].m_texture,
                                                 ImageBufferInfo(newImageIndex,t_entityId,m_material.getMaterialId())});

                        // Set the texture information in the entities SSBO object.
                        t_entity3DSSBOData[t_entitySSBOIndex].textureIndex[m_material.getMaterialId()][t_entityTextureIndex] = newImageIndex;
                        //t_entity3DSSBOData[t_entitySSBOIndex].textureIndex = newImageIndex;

                    } else{

                        // If the image was already in the map attempt to insert the relation between that image and
                        // this entity-material combination.
                        auto entityImageSearchIterator= imageSearchIterator->second.
                                m_entityMaterialMap.
                                insert(std::pair<ecs_id,
                                       std::unordered_set<material_id>>(t_entityId,
                                               m_material.
                                               getMaterialId()));

                        // Check to see if the insertion was successful.
                        if(!entityImageSearchIterator.second){

                            // If the insertion was not successful check to see if the material ID is already in the
                            // list if not add it.
                            if (entityImageSearchIterator.first->second.find(m_material.getMaterialId()) == entityImageSearchIterator.first->second.end()){
                                entityImageSearchIterator.first->second.insert(m_material.getMaterialId());
                            };
                        };

                        // Since the image was already in the image buffer, update the entities SSBO index data accordingly.
                        t_entity3DSSBOData[t_entitySSBOIndex].textureIndex[m_material.getMaterialId()][t_entityTextureIndex] = imageSearchIterator->second.m_imageBufferIndex;
                        //t_entity3DSSBOData[t_entitySSBOIndex].textureIndex = imageSearchIterator->second.m_imageBufferIndex;
                    };
                };
            };

            // Set up the system prior to execution. Currently not used.
            void executeSystem(VkCommandBuffer& t_commandBuffer,
                               VkBuffer t_drawIndirectBuffer,
                               std::vector<VkDescriptorSet>& t_descriptorSets,
                               ecs_id test) {

                // Bind this material's pipeline to the command buffer.
                m_material.bindPipeline(t_commandBuffer);

                // Bind the descriptor sets to the command buffer.
                vkCmdBindDescriptorSets(
                        t_commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        m_material.getPipelineLayout(),
                        0,
                        static_cast<uint32_t>(t_descriptorSets.size()),
                        t_descriptorSets.data(),
                        0,
                        nullptr);

                // Initialize the index into the
                uint64_t indirectBufferIndex = m_drawIndirectCommandBufferIndex;

                // Loop through each of the unique models
                for(const auto& model : m_uniqueModelMap){

                    // Bind the model to the pipeline.
                    model.first->bind(t_commandBuffer);

                    // Calculate the offset value.
                    VkDeviceSize indirect_offset = indirectBufferIndex * sizeof(VkDrawIndexedIndirectCommand);

                    // Draw all instances of the model with this material.
                    vkCmdDrawIndexedIndirect(t_commandBuffer, t_drawIndirectBuffer, indirect_offset, model.second.size(), DRAW_INDEXED_INDIRECT_STRING);

                    // Increment the offset by the number of entities drawn that had the current model.
                    indirectBufferIndex += model.second.size();
                }
            };

            // Clean up the system after execution. Currently not used.
            void cleanupSystem() {
                this->m_systemManager.clearSystemEntityUpdateSignatures(this->m_systemId);
            };

            ecs_id getComponentId(){return m_modelComponent.getComponentId();};

        private:
            /// A reference to the model component this system is associated with.
            ModelComponent& m_modelComponent;

            /// A reference to the world position component this system is associated with.
            WorldPositionComponent& m_worldPositionComponent;

            /// A reference to the material component this system is associated with.
            MaterialLayerComponent& m_materialComponent;

            /// A reference to the material that this system supports.
            AeMaterial3DLayer<numVertTexts,numFragTexts,numTessTexts,numGeometryTexts>& m_material;

            /// A vector to track unique models, and a list of which entities use them.
            std::map<std::shared_ptr<AeModel>,std::map<ecs_id,VkDrawIndexedIndirectCommand>> m_uniqueModelMap;

            /// Compiles the commands to be called by draw indexed indirect command for each frame.
            std::vector<VkDrawIndexedIndirectCommand> m_materialDrawIndexedCommands;

            /// A count of the number of materialDrawIndexedCommands this material has.
            uint64_t m_entityCommandCount = 0;

            /// The starting position in the indirect command buffer where this materials commands start.
            uint64_t m_drawIndirectCommandBufferIndex = 0;
        };




        /// Constructor of the SimpleRenderSystem
        /// \param t_game_components The game components available that this system may require.
        explicit AeMaterial3DLayer(AeDevice &t_aeDevice,
                                   GameComponents& t_game_components,
                                   VkRenderPass t_renderPass,
                                   ae_ecs::AeECS& t_ecs,
                                   MaterialShaderFiles& t_materialShaderFiles,
                                   std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts) :
                m_ecs{t_ecs},
                m_gameComponents{t_game_components},
                AeMaterial3DLayerBase(t_aeDevice,
                                      t_renderPass,
                                      t_materialShaderFiles,
                                      t_descriptorSetLayouts) {
        };

        /// Destructor of the SimpleRenderSystem
        ~AeMaterial3DLayer()= default;

        void executeSystem(VkCommandBuffer& t_commandBuffer,
                           VkBuffer t_drawIndirectBuffer,
                           std::vector<VkDescriptorSet>& t_descriptorSets,
                           ecs_id test) override {

            m_materialSystem.executeSystem(t_commandBuffer,
                                           t_drawIndirectBuffer,
                                           t_descriptorSets,
                                           test);
        };

        const std::vector<VkDrawIndexedIndirectCommand>& updateMaterialEntities(std::vector<Entity3DSSBOData>& t_entity3DSSBOData,
                                   std::map<ecs_id, uint32_t>& t_entity3DSSBOMap,
                                   VkDescriptorImageInfo t_imageBuffer[MAX_TEXTURES],
                                   std::map<std::shared_ptr<AeImage>,ImageBufferInfo>& t_imageBufferMap,
                                   PreAllocatedStack<uint64_t,MAX_TEXTURES>& t_imageBufferStack,
                                   uint64_t t_drawIndirectCommandBufferIndex) override {

            return m_materialSystem.updateMaterialEntities(t_entity3DSSBOData,
                                           t_entity3DSSBOMap,
                                           t_imageBuffer,
                                           t_imageBufferMap,
                                           t_imageBufferStack,
                                           t_drawIndirectCommandBufferIndex);
        };

        ecs_id getMaterialSystemId(){return m_materialSystem.getSystemId();};
        ecs_id getComponentId() override {return m_materialComponent.getComponentId();};

    private:

        /// Reference to the ECS used for the component and the system associated with this material.
        ae_ecs::AeECS& m_ecs;

        /// The game components the system uses.
        GameComponents& m_gameComponents;

    public:
        /// Create a component for the material to track entities that use the material and specify
        /// textures/properties specific to that entity.
        MaterialLayerComponent m_materialComponent{m_ecs};

    private:

        /// Create a system to deal with organizing the models and entity information the material is responsible for
        /// rendering.
        MaterialLayerSystem m_materialSystem{m_ecs, m_gameComponents, m_materialComponent, *this};


    protected:

    };

} // namespace ae