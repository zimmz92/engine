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

    /// Specifies a texture and the sampler that should be used with it.
    struct TextureSamplerPair{
        /// The texture being sampled.
        std::shared_ptr<AeImage> m_texture = nullptr;

        /// The sampler which defines how to sample the texture.
        VkSampler m_sampler= nullptr;
    };


    /// Defines the textures, and the samplers, required for each shader stage of a material layer for an entity.
    /// \tparam numVertTexts The number of textures required for the material layer's vertex shader.
    /// \tparam numFragTexts The number of textures required for the material layer's fragment shader.
    /// \tparam numTessTexts The number of textures required for the material layer's tessellation shader.
    /// \tparam numGeometryTexts The number of textures required for the material layer's geometry shader.
    template <uint32_t numVertTexts, uint32_t numFragTexts, uint32_t numTessTexts, uint32_t numGeometryTexts>
    struct MaterialLayerTextures{

        /// The entities texture(s) for the material layer's vertex shader.
        TextureSamplerPair m_vertexTextures[numVertTexts]{};
        /// The number of textures required for the material layer's vertex shader.
        static const uint32_t m_numVertexTextures = numVertTexts;

        /// The entities texture(s) for the material layer's fragment shader.
        TextureSamplerPair m_fragmentTextures[numFragTexts]{};
        /// The number of textures required for the material layer's fragment shader.
        static const uint32_t m_numFragmentTextures = numFragTexts;

        /// The entities texture(s) for the material layer's tessellation shader.
        TextureSamplerPair m_tessellationTextures[numTessTexts]{};
        /// The number of textures required for the material layer's tessellation shader.
        static const uint32_t m_numTessellationTextures = numTessTexts;

        /// The entities texture(s) for the material layer's geometry shader.
        TextureSamplerPair m_geometryTextures[numGeometryTexts]{};
        /// The number of textures required for the material layer's geometry shader.
        static const uint32_t m_numGeometryTextures = numGeometryTexts;

        // TODO: make functions to add/get shader image/sampler pairs to enforce only adding information to the
        //  materials where the textures are supposed to exist.
    };

    /// Defines the textures, and the samplers, required for each shader stage of a material layer for an entity.
    /// \tparam numVertTexts The number of textures required for the material layer's vertex shader.
    /// \tparam numFragTexts The number of textures required for the material layer's fragment shader.
    /// \tparam numTessTexts The number of textures required for the material layer's tessellation shader.
    /// \tparam numGeometryTexts The number of textures required for the material layer's geometry shader.
    template <uint32_t numVertTexts, uint32_t numFragTexts, uint32_t numTessTexts, uint32_t numGeometryTexts>
    class AeMaterial3DLayer : public AeMaterial3DLayerBase{

        /// Ensure that the number of textures is less than the total number of textures allowed for each material
        /// layer.
        static_assert(numVertTexts+numFragTexts+numTessTexts+numGeometryTexts <= MAX_3D_MATERIAL_TEXTURES,
                      "The total number of textures specified to create a material is greater than the maximum allowed number "
                      "of textures per material, MAX_TEXTURES_PER_MATERIAL.");

        /// Represent the MaterialLayerTextures structure that represents the entity data for this material layer as T
        /// to to make it easier to read the rest of this class definition.
        using T = MaterialLayerTextures<numVertTexts,numFragTexts,numTessTexts,numGeometryTexts>;



    public:

        /// This component is used to allow entities to store specific information required for the material layer to
        /// correctly render the entity.
        class MaterialLayerComponent : public ae_ecs::AeComponent<T>{
        public:
            /// The MaterialLayerComponent constructor. Uses the AeComponent constructor with no additions.
            explicit MaterialLayerComponent(ae_ecs::AeECS& t_ecs) : ae_ecs::AeComponent<T>(t_ecs){};

            /// The destructor of the MaterialLayerComponent class. The MaterialComponent destructor
            /// uses the AeComponent constructor with no additions.
            ~MaterialLayerComponent() = default;
        };



        /// Organizes, tracks, creates draw indexed indirect commands, and submits draw indexed indirect command for the
        /// entities that use this material layer. Since draw calls are done using draw indexed indirect commands unique
        /// models are managed and mapped to their dependent entities. Additionally, textures used by entities that use
        /// this material layer are added tracked and managed within the texture SSBO.
        class MaterialLayerSystem : public ae_ecs::AeSystem<MaterialLayerSystem>{

        public:


            /// Constructs the MaterialLayerSystem to organize, track, create draw indexed indirect commands, and submit
            /// draw indexed indirect command for the entities that use this material layer.
            /// \param t_ecs The entity component system that will be responsible for this system.
            /// \param t_game_components The game components that may be utilized by this system.
            /// \param t_materialComponent The material component for the material layer this system is associated with.
            /// \param t_materialLayer The material layer this system is associated with.
            MaterialLayerSystem(ae_ecs::AeECS& t_ecs,
                                GameComponents& t_game_components,
                                MaterialLayerComponent& t_materialComponent,
                                AeMaterial3DLayer<numVertTexts,numFragTexts,numTessTexts,numGeometryTexts>& t_materialLayer) :
                           ae_ecs::AeSystem<MaterialLayerSystem>(t_ecs),
                                   m_modelComponent{t_game_components.modelComponent},
                                   m_materialComponent{t_materialComponent},
                                   m_worldPositionComponent{t_game_components.worldPositionComponent},
                                   m_material{t_materialLayer}{


                // Register component dependencies
                m_modelComponent.requiredBySystem(this->m_systemId);
                m_materialComponent.requiredBySystem(this->m_systemId);
                m_worldPositionComponent.requiredBySystem(this->m_systemId);

                // There will be a call in the renderer system to all the material systems.
                this->isChildSystem = true;

                // Enable the system
                this->enableSystem();
            };



            /// Destructor of the MaterialLayerSystem. Uses the default destructor.
            ~MaterialLayerSystem() = default;



            /// Updates the material layer entity data trackers and the image buffer for entity information that has
            /// updated and/or entities that have been removed or had this material layer removed.
            /// \param t_entity3DSSBOData The model matrix and texture indices data for an entity that is in the 3D-SSBO
            /// that will be accessed by the material layer shaders when rendering.
            /// \param t_entity3DSSBOMap The map of where entities data are within the 3D-SSBO.
            /// \param t_imageBuffer The buffer that stores the images and their samplers that will be accessed by the
            /// material layer shaders when rendering.
            /// \param t_imageBufferMap The map of images and their samplers and which entities use them with which
            /// materials.
            /// \param t_imageBufferStack The stack of available positions within the imageBufferMap where new images
            /// can be placed. When no entities utilize a specific image the image's position within the imageBufferMap
            /// is placed at the top of this stack.
            /// \param t_drawIndexedIndirectCommandBufferIndex The position within the drawIndexedIndirectCommandBuffer
            /// that the drawIndexedIndirectCommands required for this material layer will start. This position will be
            /// required when executing the drawIndexedIndirect commands for this material.
            const std::vector<VkDrawIndexedIndirectCommand>& updateMaterialEntities(std::vector<Entity3DSSBOData>& t_entity3DSSBOData,
                                                                                    std::map<ecs_id, uint32_t>& t_entity3DSSBOMap,
                                                                                    VkDescriptorImageInfo t_imageBuffer[MAX_TEXTURES],
                                                                                    std::map<std::shared_ptr<AeImage>,ImageBufferInfo>& t_imageBufferMap,
                                                                                    PreAllocatedStack<uint64_t,MAX_TEXTURES>& t_imageBufferStack,
                                                                                    uint64_t t_drawIndexedIndirectCommandBufferIndex) {

                // Store the current index of the indexed indirect command buffer so when this material executes it
                // knows where it's commands start in the buffer.
                m_drawIndexedIndirectCommandBufferIndex = t_drawIndexedIndirectCommandBufferIndex;

                // Set the flag to false, if no entities were updated or destroyed then no need to remake the command
                // vector.
                m_remakeCommandVector = false;

                // Delete the destroyed entities from the list first.
                std::vector<ecs_id> destroyedEntityIds = this->m_systemManager.getDestroyedSystemEntities(this->m_systemId);
                for(ecs_id entityId: destroyedEntityIds){
                    // An update occurred to an entity, remake the command array for this material.
                    m_remakeCommandVector = true;

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
                    m_remakeCommandVector = true;

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


                // TODO find a better way to do this where the entire vector does not need to be recreated every time
                //  one entity updates, or at least the memory does not need to be reallocated for the vector.
                // If entities have updated then the drawIndexedIndirect commands need to be updated. This is to ensure
                // that they are properly packed in the drawIndexedIndirectCommandBuffer for drawIndexedIndirect to
                // draw all entities that use the same model, and this material layer, in a row since
                // drawIndexedIndirect assumes a starting position, and a fixed stride, to draw a fixed set of entities
                // with commands stored in the drawIndexedIndirectCommandBuffer.
                if(m_remakeCommandVector){
                    // Clear the vector to start over packing the drawIndexedIndirect commands.
                    m_materialDrawIndexedCommands.clear();

                    // Loop through each of the models, and entities that use the model, to reform the command vector.
                    for(const auto& model : m_uniqueModelMap){
                        for(const auto& entityCommands : model.second){
                            m_materialDrawIndexedCommands.push_back(entityCommands.second);
                        }
                    }
                }

                // Return the drawIndexedIndirect commands to be integrated into the drawIndexedIndirect command buffer.
                return m_materialDrawIndexedCommands;
            };



            /// Ensures the images for an entity, and material layer shader, are properly stored within the image buffer
            /// and the location of the image in the image buffer is stored within the entities 3D-SSBO data structure
            /// to be accessible during rendering.
            /// \param t_entitySSBOIndex The location of the entities data in the 3D-SSBO.
            /// \param t_entityTextureIndex The current index into the 3D-SSBO structure material layer texture index
            /// array.
            /// \param t_entityId The ID of the entity which images for this material layer, and shader, are currently
            /// being handled.
            /// \param t_shaderTextures The textures the entity uses for the current shader being handled.
            /// \param t_numShaderTextures The number of textures the material layer requires for the current shader
            /// being handled.
            /// \param t_entity3DSSBOData The 3D-SSBO where the entities data is stored and texture indices must be
            /// recorded for the shader to access the correct shader during rendering.
            /// \param t_imageBuffer The image buffer where the textures for the entity must be stored to be accessible
            /// by the shaders.
            /// \param t_imageBufferMap The map of which entities, and which of their materials, utilize which images in
            /// the image buffer.
            /// \param t_imageBufferStack The stack of available positions in the imageBuffer where new images may be
            /// placed and where indices for images that were removed from the imageBuffer need to be placed on top of
            /// for reuse.
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

                // Loop through all the textures required for the shader being processed for this entity.
                for(int i = 0; i<t_numShaderTextures; i++){

                    // Check if the object has a texture. If not set it such that the model is rendered using only its
                    // vertex colors.
                    if(t_shaderTextures[i].m_texture == nullptr){
                        t_entity3DSSBOData[t_entitySSBOIndex].textureIndex[m_material.getMaterialLayerId()][t_entityTextureIndex] = MAX_TEXTURES + 1;
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
                                                 ImageBufferInfo(newImageIndex,t_entityId,
                                                                 m_material.getMaterialLayerId())});

                        // Set the texture information in the entities SSBO object.
                        t_entity3DSSBOData[t_entitySSBOIndex].textureIndex[m_material.getMaterialLayerId()][t_entityTextureIndex] = newImageIndex;

                    } else{

                        // If the image was already in the map attempt to insert the relation between that image and
                        // this entity-material combination.
                        auto entityImageSearchIterator= imageSearchIterator->second.
                                m_entityMaterialMap.
                                insert(std::pair<ecs_id,
                                       std::unordered_set<material_id>>(t_entityId,
                                                                        m_material.
                                                                                getMaterialLayerId()));

                        // Check to see if the insertion was successful.
                        if(!entityImageSearchIterator.second){

                            // If the insertion was not successful check to see if the material ID is already in the
                            // list if not add it.
                            if (entityImageSearchIterator.first->second.find(m_material.getMaterialLayerId()) == entityImageSearchIterator.first->second.end()){
                                entityImageSearchIterator.first->second.insert(m_material.getMaterialLayerId());
                            };
                        };

                        // Since the image was already in the image buffer, update the entities SSBO index data accordingly.
                        t_entity3DSSBOData[t_entitySSBOIndex].textureIndex[m_material.getMaterialLayerId()][t_entityTextureIndex] = imageSearchIterator->second.m_imageBufferIndex;
                    };
                };
            };



            /// Loop through the unique models that entities that utilize this material layer have and use
            /// drawIndexedIndirect to render all entities that use a unique model using a single drawIndexedIndirect
            /// call.
            /// \param t_commandBuffer The command buffer where the drawIndexedIndirect commands will be recorded to.
            /// \param t_drawIndexedIndirectBuffer The buffer that stores the drawIndexedIndirect commands.
            /// \param t_descriptorSets The descriptor sets that the shaders of this material layer need access to for
            /// rendering their entities.
            void executeSystem(VkCommandBuffer& t_commandBuffer,
                               VkBuffer t_drawIndexedIndirectBuffer,
                               std::vector<VkDescriptorSet>& t_descriptorSets) {

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

                // Set the buffer index to the correct offset in the command buffer where the commands for this
                // material layer start.
                uint64_t indirectBufferIndex = m_drawIndexedIndirectCommandBufferIndex;

                // Pre-allocate the memory for this.
                VkDeviceSize indirect_offset;

                // Loop through each of the unique models
                for(const auto& model : m_uniqueModelMap){

                    // Bind the model to the pipeline.
                    model.first->bind(t_commandBuffer);

                    // Calculate the offset value.
                    indirect_offset = indirectBufferIndex * sizeof(VkDrawIndexedIndirectCommand);

                    // Draw all instances of the model with this material.
                    vkCmdDrawIndexedIndirect(t_commandBuffer, t_drawIndexedIndirectBuffer, indirect_offset, model.second.size(), DRAW_INDEXED_INDIRECT_COMMAND_SIZE);

                    // Increment the offset by the number of entities drawn that had the current model.
                    indirectBufferIndex += model.second.size();
                }
            };

            /// Clean up the system after execution.
            void cleanupSystem() {
                // Clear the material layer system's update/destroy flags since they have been handled.
                this->m_systemManager.clearSystemEntityUpdateSignatures(this->m_systemId);
            };

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

            /// The starting position in the indirect command buffer where this materials commands start.
            uint64_t m_drawIndexedIndirectCommandBufferIndex = 0;

            /// A flag to indicate if the draw indexed indirect command vector needs to be recreated for the material
            /// layer.
            bool m_remakeCommandVector = false;
        };




        /// Creates a material layer for the specified shaders. This will create a graphics pipeline that implements the
        /// shaders, a component that entities can utilize to specify the textures required for the shader, and a system
        /// which manages entities that use the material and submits drawIndexedIndirect calls to render them.
        /// \param t_aeDevice The graphics device that this material layer will be implemented on.
        /// \param t_game_components The game components available that this system may require.
        /// \param t_renderPass The render pass that this material layer will executed by.
        /// \param t_ecs The entity component system the Component and System this material layer implements and
        /// utilizes will be managed by.
        /// \param t_materialLayerShaderFiles The shader files which are to be implemented by this material layer and
        /// specify the dynamic stages of it's graphics pipeline.
        /// \param t_descriptorSetLayouts The layouts of the descriptor sets that will be required by the shaders of the
        /// material layer being created.
        explicit AeMaterial3DLayer(AeDevice &t_aeDevice,
                                   GameComponents& t_game_components,
                                   VkRenderPass t_renderPass,
                                   ae_ecs::AeECS& t_ecs,
                                   MaterialShaderFiles& t_materialLayerShaderFiles,
                                   std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts) :
                m_ecs{t_ecs},
                m_gameComponents{t_game_components},
                AeMaterial3DLayerBase(t_aeDevice,
                                      t_renderPass,
                                      t_materialLayerShaderFiles,
                                      t_descriptorSetLayouts) {

            // All setup of the material layer pipeline is handled by the AeMaterial3DLayerBase. Everything else is done
            // by initialization of member variables by reference.

        };



        /// The destructor is default.
        ~AeMaterial3DLayer()= default;


        /// Loop through the unique models that entities that utilize this material layer have and use
        /// drawIndexedIndirect to render all entities that use a unique model using a single drawIndexedIndirect
        /// call.
        /// \param t_commandBuffer The command buffer where the drawIndexedIndirect commands will be recorded to.
        /// \param t_drawIndexedIndirectBuffer The buffer that stores the drawIndexedIndirect commands.
        /// \param t_descriptorSets The descriptor sets that the shaders of this material layer need access to for
        /// rendering their entities.
        void executeSystem(VkCommandBuffer& t_commandBuffer,
                           VkBuffer t_drawIndexedIndirectBuffer,
                           std::vector<VkDescriptorSet>& t_descriptorSets) override {

            // Call the material system's executeSystem function. The system tracks which compatible
            // entities have been updated/destroyed and need to be acted upon.
            m_materialSystem.executeSystem(t_commandBuffer,
                                           t_drawIndexedIndirectBuffer,
                                           t_descriptorSets);
        };



        /// Performs any required clean up of the material layer system that is required after it's execution.
        void cleanupSystem() override {
            m_materialSystem.cleanupSystem();
        };


        /// Updates the material layer entity data trackers and the image buffer for entity information that has
        /// updated and/or entities that have been removed or had this material layer removed.
        /// \param t_entity3DSSBOData The model matrix and texture indices data for an entity that is in the 3D-SSBO
        /// that will be accessed by the material layer shaders when rendering.
        /// \param t_entity3DSSBOMap The map of where entities data are within the 3D-SSBO.
        /// \param t_imageBuffer The buffer that stores the images and their samplers that will be accessed by the
        /// material layer shaders when rendering.
        /// \param t_imageBufferMap The map of images and their samplers and which entities use them with which
        /// materials.
        /// \param t_imageBufferStack The stack of available positions within the imageBufferMap where new images
        /// can be placed. When no entities utilize a specific image the image's position within the imageBufferMap
        /// is placed at the top of this stack.
        /// \param t_drawIndexedIndirectCommandBufferIndex The position within the drawIndexedIndirectCommandBuffer
        /// that the drawIndexedIndirectCommands required for this material layer will start. This position will be
        /// required when executing the drawIndexedIndirect commands for this material.
        const std::vector<VkDrawIndexedIndirectCommand>& updateMaterialLayerEntities(std::vector<Entity3DSSBOData>& t_entity3DSSBOData,
                                                                                     std::map<ecs_id, uint32_t>& t_entity3DSSBOMap,
                                                                                     VkDescriptorImageInfo t_imageBuffer[MAX_TEXTURES],
                                                                                     std::map<std::shared_ptr<AeImage>,ImageBufferInfo>& t_imageBufferMap,
                                                                                     PreAllocatedStack<uint64_t,MAX_TEXTURES>& t_imageBufferStack,
                                                                                     uint64_t t_drawIndexedIndirectCommandBufferIndex) override {

            // Call the material system's updateMaterialLayerEntities function. The system tracks which compatible
            // entities have been updated/destroyed and need to be acted upon.
            return m_materialSystem.updateMaterialEntities(t_entity3DSSBOData,
                                                           t_entity3DSSBOMap,
                                                           t_imageBuffer,
                                                           t_imageBufferMap,
                                                           t_imageBufferStack,
                                                           t_drawIndexedIndirectCommandBufferIndex);
        };



        /// Returns the ID of the ECS component that this material layer uses to track entity specific information
        /// required for this material layer.
        /// \return The ID of the material layer component.
        ecs_id getMaterialLayerComponentId() override {return m_materialComponent.getComponentId();};

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