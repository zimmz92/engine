/// \file renderer_start_pass_system.cpp
/// \brief The script implementing the renderer start pass system class.
/// The renderer start pass system class is implemented.

#include <iterator>
#include <iostream>
#include "ae_renderer_system.hpp"

namespace ae {

    // Constructor of the RendererStartPassSystem
    RendererStartPassSystem::RendererStartPassSystem(ae_ecs::AeECS& t_ecs,
                                                     GameComponents& t_game_components,
                                                     UpdateUboSystem& t_updateUboSystem,
                                                     TimingSystem& t_timingSystem,
                                                     AeRenderer& t_renderer,
                                                     AeDevice& t_aeDevice,
                                                     AeSamplers& t_aeSamplers,
                                                     AeResourceManager& t_aeResourceManager) :
                                                     m_updateUboSystem{t_updateUboSystem},
                                                     m_timingSystem{t_timingSystem},
                                                     m_renderer{t_renderer},
                                                     m_aeDevice{t_aeDevice},
                                                     m_aeSamplers{t_aeSamplers},
                                                     m_gameComponents{t_game_components},
                                                     m_aeResourceManager{t_aeResourceManager},
                                                     ae_ecs::AeSystem<RendererStartPassSystem>(t_ecs)  {

        // Register component dependencies
        // None currently.


        // Register system dependencies
        this->dependsOnSystem(m_updateUboSystem.getSystemId());
        this->dependsOnSystem(m_timingSystem.getSystemId());

        //==============================================================================================================
        // Global Descriptor Pool Initialization
        //==============================================================================================================

        // Create the global pool
        m_globalPool = AeDescriptorPool::Builder(m_aeDevice)
                .setMaxSets(MAX_FRAMES_IN_FLIGHT*8)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_TEXTURES * MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, MAX_FRAMES_IN_FLIGHT*7)
                .build();

        //==============================================================================================================
        // UBO Descriptor Set Initialization
        //==============================================================================================================

        // Initialize the ubo buffers
        m_uboBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Create a new buffer and push it to the back of the vector of uboBuffers.
            m_uboBuffers.push_back(std::make_unique<AeBuffer>(
                    m_aeDevice,
                    sizeof(GlobalUbo),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

            // Attempt to create the pipeline layout, if it cannot error out.
            if ( m_uboBuffers.back()->map() != VK_SUCCESS) {
                throw std::runtime_error("Failed to map the ubo buffer to memory!");
            };
        };

        // Get the layout of the device and specify some general rendering options for all render systems.
        auto globalSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT)
                .build();

        // Reserve space for and then initialize the global descriptors for each frame.
        m_globalDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Get the buffer information from the uboBuffers.
            auto bufferInfo = m_uboBuffers[i]->descriptorInfo();

            // Initialize the descriptor set for the current frame.
            AeDescriptorWriter(globalSetLayout, *m_globalPool)
                    .writeBuffer(0, &bufferInfo)
                    .build(m_globalDescriptorSets[i]);
        }


        //==============================================================================================================
        // Particle Descriptor Set Initialization
        //==============================================================================================================
        auto particleSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1)
                .addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1)
                .addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1)
                .build();

        // Allocate memory for the compute buffers
        m_particleBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Create a new buffer and push it to the back of the vector of uboBuffers.
            m_particleBuffers.push_back(std::make_unique<AeBuffer>(
                    m_aeDevice,
                    sizeof(Particle),
                    NUM_PARTICLES,
                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
        };


        m_particleDescriptorWriter = new AeDescriptorWriter(particleSetLayout, *m_globalPool);

        // Reserve space for compute descriptor sets for each frame.
        m_particleDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Get the buffer information from the uboBuffers.
            auto uboBufferInfo = m_uboBuffers[i]->descriptorInfo();
            auto prevbufferInfo = m_particleBuffers[(i - 1) % MAX_FRAMES_IN_FLIGHT]->descriptorInfo();
            auto bufferInfo = m_particleBuffers[i]->descriptorInfo();

            // Initialize the descriptor set for the current frame.
            m_particleDescriptorWriter->writeBuffer(0, &uboBufferInfo)
                                      .writeBuffer(1, &prevbufferInfo)
                                      .writeBuffer(2, &bufferInfo)
                                      .build(m_particleDescriptorSets[i]);
        }

        // Pack the descriptor sets used by the particle system into a vector.
        m_particleFrameDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        for(int i=0;i<MAX_FRAMES_IN_FLIGHT;i++){
            m_particleFrameDescriptorSets.push_back({m_particleDescriptorSets[i]});

        }

        // Create a particle system using the compute pipeline
        std::vector<VkDescriptorSetLayout> particleDescriptorSetLayouts = {particleSetLayout->getDescriptorSetLayout()};
//        m_particleSystem = new AeParticleSystem(m_aeDevice,
//                                                particleDescriptorSetLayouts,
//                                                m_particleBuffers,
//                                                m_renderer.getSwapChainRenderPass());
        m_particleSystem = new AeParticleSystem(m_aeDevice,
                                                particleDescriptorSetLayouts,
                                                m_particleBuffers,
                                                m_renderer.getSwapChainRenderPass());



        //==============================================================================================================
        // Texture Descriptor Set Initialization
        //==============================================================================================================
        // Define the descriptor set for the texture.
        auto textureSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES)
                .build();

        // Reserve space for and then initialize the texture descriptors for each frame.
        m_defaultImage = AeImage::createImageFromFile(m_aeDevice, "assets/ui_textures/default.jpg");
        m_textureDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_defaultImage->getImageView();
        imageInfo.sampler = m_aeSamplers.getDefaultSampler();

        // Initialize the descriptor writer that will be used to update the image data for the textures.
        m_textureDescriptorWriter = new AeDescriptorWriter(textureSetLayout, *m_globalPool);

        // Set the default image information for the imageBufferData array.
        for (auto & j : m_imageBufferData) {
            j = imageInfo;
        };

        // Initialize and build the description writer.
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            m_textureDescriptorWriter->writeImage(0, m_imageBufferData).build(m_textureDescriptorSets[i]);
        }

        //==============================================================================================================
        // Setup Object Shader Storage Buffer for 3D Objects (SSBO)
        //==============================================================================================================

        // Allocate memory for the object buffers
        m_object3DBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Create a new buffer and push it to the back of the vector of uboBuffers.
            m_object3DBuffers.push_back(std::make_unique<AeBuffer>(
                    m_aeDevice,
                    sizeof(SimplePushConstantData),
                    MAX_OBJECTS,
                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

            // Attempt to map memory for the object buffer.
            if (m_object3DBuffers.back()->map() != VK_SUCCESS) {
                throw std::runtime_error("Failed to map the ubo buffer to memory!");
            };
        };

        // Define the descriptor set for the object buffers.
        auto objectSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_ALL_GRAPHICS)
                .build();

        // Reserve space for and then initialize the global descriptors for each frame.
        m_object3DDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Get the buffer information from the uboBuffers.
            auto bufferInfo = m_object3DBuffers[i]->descriptorInfo();

            // Initialize the descriptor set for the current frame.
            AeDescriptorWriter(objectSetLayout, *m_globalPool)
                    .writeBuffer(0, &bufferInfo)
                    .build(m_object3DDescriptorSets[i]);
        }


        //==============================================================================================================
        // 3D Collision Descriptor Set Initialization
        //==============================================================================================================
        // Need a storage buffer for the model OBBs and a storage buffer for the 3D objects that use them to calculate
        // their AABB.
        auto collisionSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT, 1)
                .addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT, 1)
                .build();

        // Allocate memory for the OBB compute buffers
        m_collisionBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Create a new buffer and push it to the back of the vector of collision buffers.
            m_collisionBuffers.push_back(std::make_unique<AeBuffer>(
                    m_aeDevice,
                    sizeof(VkAabbPositionsKHR),
                    MAX_MODELS,
                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

            // Attempt to map memory for the collision buffer.
            if ( m_collisionBuffers.back()->map() != VK_SUCCESS) {
                throw std::runtime_error("Failed to map the ubo buffer to memory!");
            };
        };


        m_collisionDescriptorWriter = new AeDescriptorWriter(collisionSetLayout, *m_globalPool);

        // Reserve space for compute descriptor sets for each frame.
        m_collisionDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Get the buffer information for the OBB SSBO
            auto obb3dBufferInfo = m_collisionBuffers[i]->descriptorInfo();
            // Get the buffer information for the entities
            auto entity3dBufferInfo = m_object3DBuffers[i]->descriptorInfo();

            // Initialize the descriptor set for the current frame.
            m_collisionDescriptorWriter->writeBuffer(0, &obb3dBufferInfo)
                    .writeBuffer(1, &entity3dBufferInfo)
                    .build(m_collisionDescriptorSets[i]);
        }

        // Pack the descriptor sets used by the particle system into a vector.
        m_collisionFrameDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        for(int i=0;i<MAX_FRAMES_IN_FLIGHT;i++){
            m_collisionFrameDescriptorSets.push_back({m_collisionDescriptorSets[i]});
        }

        m_obbAabbFrameDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        for(int i=0;i<MAX_FRAMES_IN_FLIGHT;i++){
            m_obbAabbFrameDescriptorSets.push_back({m_globalDescriptorSets[i],
                                                      m_collisionDescriptorSets[i]});
        }

        // Create a collision compute system using the compute pipeline
        std::vector<VkDescriptorSetLayout> collisionDescriptorSetLayouts = {collisionSetLayout->getDescriptorSetLayout()};

        // Create a aabb render system
        std::vector<VkDescriptorSetLayout> aabbSetLayouts = {globalSetLayout->getDescriptorSetLayout(),
                                                             collisionSetLayout->getDescriptorSetLayout()};

        m_collisionSystem = new AeCollisionSystem(t_ecs,
                                                  t_game_components,
                                                  m_aeDevice,
                                                  m_renderer.getSwapChainRenderPass(),
                                                  collisionDescriptorSetLayouts,
                                                  aabbSetLayouts);


        //==============================================================================================================
        // Setup Object Shader Storage Buffer for 2D Objects (SSBO)
        //==============================================================================================================

        // Allocate memory for the object buffers
        m_object2DBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Create a new buffer and push it to the back of the vector of uboBuffers.
            m_object2DBuffers.push_back(std::make_unique<AeBuffer>(
                    m_aeDevice,
                    sizeof(UiPushConstantData),
                    MAX_OBJECTS,
                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

            // Attempt to map memory for the object buffer.
            if ( m_object2DBuffers.back()->map() != VK_SUCCESS) {
                throw std::runtime_error("Failed to map the ubo buffer to memory!");
            };
        };

        // Define the descriptor set for the object buffers.
        auto object2DSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_ALL_GRAPHICS)
                .build();

        // Reserve space for and then initialize the global descriptors for each frame.
        m_object2DDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Get the buffer information from the uboBuffers.
            auto bufferInfo = m_object2DBuffers[i]->descriptorInfo();

            // Initialize the descriptor set for the current frame.
            AeDescriptorWriter(object2DSetLayout, *m_globalPool)
                    .writeBuffer(0, &bufferInfo)
                    .build(m_object2DDescriptorSets[i]);
        }

        //==============================================================================================================
        // Create Draw Indirect Command Buffers
        //==============================================================================================================
        // Initialize the ubo buffers
        m_drawIndirectBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Create a new buffer and push it to the back of the vector of uboBuffers.
            m_drawIndirectBuffers.push_back(std::make_unique<AeBuffer>(
                    m_aeDevice,
                    sizeof(VkDrawIndexedIndirectCommand),
                    MAX_OBJECTS * MAX_3D_MATERIALS,
                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |  VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

            // Attempt to map the draw indirect command memory.
            if (m_drawIndirectBuffers.back()->map() != VK_SUCCESS) {
                throw std::runtime_error("Failed to map the draw indirect command buffer memory!");
            };
        };

        // Define the descriptor set for the object buffers.
        auto indirectSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_ALL_GRAPHICS)
                .build();

        // Reserve space for and then initialize the global descriptors for each frame.
        m_drawIndirectDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Get the buffer information from the uboBuffers.
            auto bufferInfo = m_drawIndirectBuffers[i]->descriptorInfo();

            // Initialize the descriptor set for the current frame.
            AeDescriptorWriter(indirectSetLayout, *m_globalPool)
                    .writeBuffer(0, &bufferInfo)
                    .build(m_drawIndirectDescriptorSets[i]);
        }

        //==============================================================================================================
        // Setup Indirect Object Shader Storage Buffer for 3D Objects (SSBO)
        //==============================================================================================================

        // Allocate memory for the object buffers
        m_object3DBuffersIndirect.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Create a new buffer and push it to the back of the vector of uboBuffers.
            m_object3DBuffersIndirect.push_back(std::make_unique<AeBuffer>(
                    m_aeDevice,
                    sizeof(Entity3DSSBOData),
                    MAX_OBJECTS,
                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

            // Attempt to map memory for the object buffer.
            if (m_object3DBuffersIndirect.back()->map() != VK_SUCCESS) {
                throw std::runtime_error("Failed to map the ubo buffer to memory!");
            };
        };

        // Define the descriptor set for the object buffers.
        auto objectSetLayoutIndirect = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_ALL_GRAPHICS)
                .build();

        // Reserve space for and then initialize the global descriptors for each frame.
        m_object3DDescriptorSetsIndirect.reserve(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Get the buffer information from the uboBuffers.
            auto bufferInfo = m_object3DBuffersIndirect[i]->descriptorInfo();

            // Initialize the descriptor set for the current frame.
            AeDescriptorWriter(objectSetLayoutIndirect, *m_globalPool)
                    .writeBuffer(0, &bufferInfo)
                    .build(m_object3DDescriptorSetsIndirect[i]);
        }


        //==============================================================================================================
        // Descriptor Set Gathering
        //==============================================================================================================
        // Gather all the descriptor set layouts.
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {globalSetLayout->getDescriptorSetLayout(),
                                                                   textureSetLayout->getDescriptorSetLayout(),
                                                                   objectSetLayoutIndirect->getDescriptorSetLayout(),
                                                                   indirectSetLayout->getDescriptorSetLayout()};

        std::vector<VkDescriptorSetLayout> descriptorSetLayoutsOLD = {globalSetLayout->getDescriptorSetLayout(),
                                                                      textureSetLayout->getDescriptorSetLayout(),
                                                                      objectSetLayout->getDescriptorSetLayout()};

        // Gather all the descriptor sets.
        m_frameDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        m_frameDescriptorSetsOLD.reserve(MAX_FRAMES_IN_FLIGHT);
        for(int i=0;i<MAX_FRAMES_IN_FLIGHT;i++){
            m_frameDescriptorSets.push_back({m_globalDescriptorSets[i],
                                             m_textureDescriptorSets[i],
                                             m_object3DDescriptorSetsIndirect[i],
                                             m_drawIndirectDescriptorSets[i]});

            m_frameDescriptorSetsOLD.push_back({m_globalDescriptorSets[i],
                                                m_textureDescriptorSets[i],
                                                m_object3DDescriptorSets[i]});
        }


        //==============================================================================================================
        // Setup child render systems
        //==============================================================================================================


        // Creates a buffer of entity model matrix and texture data for entities with 3D models that have a material.
        m_model3DBufferSystem = new AeModel3DBufferSystem(t_ecs,t_game_components);

        // Defines the materials available for entities to use.
        m_gameMaterials = new GameMaterials(m_aeDevice,
                                            t_game_components,
                                            m_renderer.getSwapChainRenderPass(),
                                            t_ecs,
                                            descriptorSetLayouts);

        // Create a list of the available material component IDs for quick reference when making/updating the model
        // matrix data.
        for(auto material : m_gameMaterials->m_materials){
            m_materialComponentIds.push_back(material->getMaterialLayerComponentId());
        };



        m_simpleRenderSystem = new SimpleRenderSystem(t_ecs,
                                                      t_game_components,
                                                      m_aeDevice,
                                                      m_renderer.getSwapChainRenderPass(),
                                                      descriptorSetLayoutsOLD);


        m_pointLightRenderSystem = new PointLightRenderSystem(t_ecs,
                                                              t_game_components,
                                                              m_aeDevice,
                                                              m_renderer.getSwapChainRenderPass(),
                                                              globalSetLayout->getDescriptorSetLayout());


        m_obbRenderSystem = new ObbRenderSystem(t_ecs,
                                                t_game_components,
                                                m_aeDevice,
                                                m_renderer.getSwapChainRenderPass(),
                                                globalSetLayout->getDescriptorSetLayout());


//        m_aabbRenderSystem = new AabbRenderSystem(t_ecs,
//                                                t_game_components,
//                                                m_aeDevice,
//                                                m_renderer.getSwapChainRenderPass(),
//                                                globalSetLayout->getDescriptorSetLayout());


//        m_uiRenderSystem = new UiRenderSystem(t_ecs,
//                                              t_game_components,
//                                              m_aeDevice,
//                                              m_renderer.getSwapChainRenderPass(),
//                                              globalSetLayout->getDescriptorSetLayout(),
//                                              textureSetLayout->getDescriptorSetLayout(),
//                                              object2DSetLayout->getDescriptorSetLayout());


        // Enable the system
        this->enableSystem();

    };



    // Destructor class of the RendererStartPassSystem
    RendererStartPassSystem::~RendererStartPassSystem(){

//        delete m_uiRenderSystem;
//        m_uiRenderSystem = nullptr;

//        delete m_aabbRenderSystem;
//        m_aabbRenderSystem = nullptr;

        delete m_obbRenderSystem;
        m_obbRenderSystem = nullptr;

        delete m_pointLightRenderSystem;
        m_pointLightRenderSystem = nullptr;

        delete m_simpleRenderSystem;
        m_simpleRenderSystem = nullptr;

        delete m_gameMaterials;
        m_gameMaterials = nullptr;

        delete m_model3DBufferSystem;
        m_model3DBufferSystem = nullptr;

        delete m_collisionSystem;
        m_collisionSystem = nullptr;

        delete m_collisionDescriptorWriter;
        m_collisionDescriptorWriter = nullptr;

        delete m_textureDescriptorWriter;
        m_textureDescriptorWriter = nullptr;

        delete m_particleSystem;
        m_particleSystem = nullptr;

        delete m_particleDescriptorWriter;
        m_particleDescriptorWriter = nullptr;

    };



    // There is no setup required for this system to execute.
    void RendererStartPassSystem::setupSystem(){

    };



    //
    void RendererStartPassSystem::executeSystem(){

        // Unless the renderer is not ready to begin a new frame and returns a nullptr start rendering a new frame.
        if ((m_graphicsCommandBuffer = m_renderer.beginFrame())) {

            // Get the frame index for the frame the render pass is starting for.
            m_frameIndex = m_renderer.getFrameIndex();

            // Write the ubo data for the shaders for this frame.
            m_uboBuffers[m_frameIndex]->writeToBuffer(m_updateUboSystem.getUbo());
            m_uboBuffers[m_frameIndex]->flush();

            // Update the texture descriptor data for the models that are being rendered.
            //updateDescriptorSets();

            // Update the model matrix data before updating the materials so that the materials know where to put the
            // image buffer indices for an entities textures.
            m_model3DBufferSystem->executeSystem(m_materialComponentIds,
                                                 m_object3DBufferData,
                                                 m_object3DBufferEntityMap,
                                                 m_object3DBufferDataIndexStack);

            // After the indexes have been updated for textures entities utilize, call each of the material's system to
            // organize the model objects for each of the materials to use draw indirect.
            uint64_t drawIndirectCount = 0;
            auto* frameDrawIndirectCommands  = static_cast<VkDrawIndexedIndirectCommand *>(m_drawIndirectBuffers[m_frameIndex]->getMappedMemory());
            for(auto material : m_gameMaterials->m_materials){
                // TODO: Much of this information does not change every cycle. Should pass the references in on material
                //  creation.
                const std::vector<VkDrawIndexedIndirectCommand>& materialCommands =
                        material->updateMaterialLayerEntities(m_object3DBufferData,
                                                              m_object3DBufferEntityMap,
                                                              m_imageBufferData,
                                                              m_imageBufferEntityMaterialMap,
                                                              m_imageBufferDataIndexStack,
                                                              drawIndirectCount);
                for(auto command : materialCommands){
                    frameDrawIndirectCommands[drawIndirectCount] = command;
                    drawIndirectCount++;
                }
            }

            // Write the texture array to the descriptor set.
            m_textureDescriptorWriter->clearWriteData();
            m_textureDescriptorWriter->writeImage(0, m_imageBufferData);
            m_textureDescriptorWriter->overwrite(m_textureDescriptorSets[m_frameIndex]);

            // Write the object buffer data to the descriptor set.
            m_object3DBuffersIndirect[m_frameIndex]->writeToBuffer(m_object3DBufferData.data());
            m_object3DBuffersIndirect[m_frameIndex]->flush();

            // Update model OBB data before running computer
            m_aeResourceManager.updateObbSsbo(m_collisionBuffers[m_frameIndex]);

            // Run compute
            m_computeCommandBuffer = m_renderer.getCurrentComputeCommandBuffer();
            //m_particleSystem->recordComputeCommandBuffer(m_computeCommandBuffer,m_particleFrameDescriptorSets[m_frameIndex]);
            m_collisionSystem->recordComputeCommandBuffer(m_computeCommandBuffer,m_collisionFrameDescriptorSets[m_frameIndex]);

            if (vkEndCommandBuffer(m_computeCommandBuffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to record compute command buffer!");
            }

            // Start the render pass.
            m_renderer.beginSwapChainRenderPass(m_graphicsCommandBuffer);


            // Loop through each material and have them draw their entities.
            for(auto material : m_gameMaterials->m_materials){

                // Draw the material's entities.
                material->executeSystem(m_graphicsCommandBuffer,
                                        m_drawIndirectBuffers[m_frameIndex]->getBuffer(),
                                        m_frameDescriptorSets[m_frameIndex]);

                // Clean up the system after it executes.
                material->cleanupSystem();
            }

            // Draw particles
//            m_particleSystem->drawParticles(m_graphicsCommandBuffer,
//                                            m_particleBuffers[m_frameIndex]->getBufferReference());

            //m_obbRenderSystem->executeSystem(m_graphicsCommandBuffer, m_globalDescriptorSets[m_frameIndex]);

            //m_aabbRenderSystem->executeSystem(m_graphicsCommandBuffer, m_globalDescriptorSets[m_frameIndex]);

            m_collisionSystem->drawAABBs(m_graphicsCommandBuffer,m_obbAabbFrameDescriptorSets[m_frameIndex]);


            m_pointLightRenderSystem->executeSystem(m_graphicsCommandBuffer, m_globalDescriptorSets[m_frameIndex]);

            // Call subservient render systems. Order matters here to maintain object transparencies.
//            m_simpleRenderSystem->executeSystem(m_graphicsCommandBuffer,
//                                                m_frameDescriptorSetsOLD[m_frameIndex]);

            //m_pointLightRenderSystem->executeSystem(m_graphicsCommandBuffer,m_globalDescriptorSets[m_frameIndex]);
//            m_uiRenderSystem->executeSystem(m_graphicsCommandBuffer,
//                                            m_globalDescriptorSets[m_frameIndex],
//                                            m_textureDescriptorSets[m_frameIndex],
//                                            m_object2DDescriptorSets[m_frameIndex],
//                                            m_frameIndex);

            // End the render pass and the frame.
            m_renderer.endSwapChainRenderPass(m_graphicsCommandBuffer);
            m_renderer.endFrame();
        };
    };



    // There is no clean up required after this system executes.
    void RendererStartPassSystem::cleanupSystem(){
        for(auto material : m_gameMaterials->m_materials){
            material->cleanupSystem();
        }
        m_systemManager.clearSystemEntityUpdateSignatures(m_systemId);
    };



    // Update the texture descriptor set for the current frame being rendered.
    void RendererStartPassSystem::updateDescriptorSets(){

        // Get entities that might have textures from their respective render systems.
        std::vector<ecs_id> validEntityIds_simpleRenderSystem = m_systemManager.getEnabledSystemsEntities(
                m_simpleRenderSystem->getSystemId());
        std::vector<ecs_id> validEntityIds_uiRenderSystem = m_systemManager.getEnabledSystemsEntities(
                m_uiRenderSystem->getSystemId());

        //==============================================================================================================
        // Testing
#ifdef MY_DEBUG
        std::string headerString = "Entities that have updated:\n";
        std::cout << headerString;
        std::vector<ecs_id> updatedEntityIds_simpleRenderSystem = m_systemManager.getUpdatedSystemEntities(m_simpleRenderSystem->getSystemId());
        for (auto entityIds : updatedEntityIds_simpleRenderSystem){
            std::string readBackString = std::to_string(entityIds) + "\n";
            std::cout << readBackString;
        };

        headerString = "Entities that have been destroyed:\n";
        std::vector<ecs_id> destroyedEntityIds_simpleRenderSystem = m_systemManager.getDestroyedSystemEntities(m_simpleRenderSystem->getSystemId());
        for (auto entityIds : destroyedEntityIds_simpleRenderSystem){
            std::string readBackString = std::to_string(entityIds) + "\n";
            std::cout << readBackString;
        };
#endif

        //==============================================================================================================

        // Declare a vector to store all the unique images that we find.
        std::vector<std::shared_ptr<AeImage>> uniqueImages{};

        // Loop through all the simple render system entities and check to make sure all their textures are included in
        // texture the array and their indexes into that array are set correctly for this frame.
        int j = 0;
        SimplePushConstantData data[MAX_OBJECTS];
        for(auto entityId:validEntityIds_simpleRenderSystem){
            ModelComponentStruct& entityModelData = m_gameComponents.modelComponent.getWriteableDataReference(entityId);
            glm::vec3 entityWorldPosition = m_gameComponents.worldPositionComponent.getWorldPositionVec3(entityId);

            if (entityModelData.m_model == nullptr) continue;

            // Set the model matrix data to be pushed to the object buffer.
            data[j] = SimpleRenderSystem::calculatePushConstantData(entityWorldPosition,
                                                                    entityModelData.rotation,
                                                                    entityModelData.scale);

            // Check if the object has a texture. If not set it such that the model is rendered using only its vertex
            // colors.
            if(entityModelData.m_texture == nullptr){
                data[j].textureIndex = MAX_TEXTURES + 1;
                j++;
                continue;
            }

            // Check to see if the texture is already part of the texture array.
            bool imageIsUnique = true;
            for(int i = 0; uniqueImages.size()>i; i++){
                // If the texture is already part of the array set the data for the object to that texture array index.
                if(entityModelData.m_texture == uniqueImages[i]){
                    imageIsUnique = false;
                    data[j].textureIndex = i;
                    j++;
                    break;
                };
            };

            // If the image was found to be unique then add it to the texture array and set the object data texture
            // index accordingly.
            if(imageIsUnique){
                entityModelData.m_texture->setTextureDescriptorIndex(m_frameIndex, uniqueImages.size());
                data[j].textureIndex = uniqueImages.size();
                j++;
                uniqueImages.push_back(entityModelData.m_texture);
            };
        };

        m_object3DBuffers[m_frameIndex]->writeToBuffer(&data);
        m_object3DBuffers[m_frameIndex]->flush();


        
        // Loop through all the ui render system entities and check to make sure all their textures are included in
        // texture the array and their indexes into that array are set correctly for this frame.
        j = 0;
        UiPushConstantData data2d[MAX_OBJECTS];
        for(auto entityId:validEntityIds_uiRenderSystem){
            auto entityModelData = m_gameComponents.model2DComponent.getWriteableDataReference(entityId);

            data2d[j] = UiRenderSystem::calculatePushConstantData(entityModelData.translation,
                                               entityModelData.rotation,
                                               entityModelData.scale);

            if(entityModelData.m_texture == nullptr){
                data2d[j].textureIndex = MAX_TEXTURES + 1;
                j++;
                continue;
            }

            bool imageIsUnique = true;
            for(int i = 0; uniqueImages.size()>i; i++){
                if(entityModelData.m_texture == uniqueImages[i]){
                    imageIsUnique = false;
                    data2d[j].textureIndex = i;
                    j++;
                    break;
                };
            };
            if(imageIsUnique){
                entityModelData.m_texture->setTextureDescriptorIndex(m_frameIndex, uniqueImages.size());
                data2d[j].textureIndex = uniqueImages.size();
                j++;
                uniqueImages.push_back(entityModelData.m_texture);
            };
        };

        m_object2DBuffers[m_frameIndex]->writeToBuffer(&data2d);
        m_object2DBuffers[m_frameIndex]->flush();


        // Ensure that we're not about to exceed the maximum number of descriptors that we have.
        if(uniqueImages.size() > MAX_TEXTURES){
            throw std::runtime_error("Too many textures trying to be bound!");
        };

        // Create the set of images that are the textures for each frame.
        VkDescriptorImageInfo imageInfos[MAX_TEXTURES];

        // Set the image information array for the frame.
        for (auto i=0; i<uniqueImages.size();i++) {
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i].imageView = uniqueImages[i]->getImageView();
            imageInfos[i].sampler = m_aeSamplers.getDefaultSampler();
        }

        // Set the rest to the default image.
        for(auto i=uniqueImages.size(); i < MAX_TEXTURES; i++){
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i].imageView = m_defaultImage->getImageView();
            imageInfos[i].sampler = m_aeSamplers.getDefaultSampler();
        }

        // Write the texture array to the descriptor set.
        m_textureDescriptorWriter->clearWriteData();
        m_textureDescriptorWriter->writeImage(0, imageInfos);
        m_textureDescriptorWriter->overwrite(m_textureDescriptorSets[m_frameIndex]);
    };

} // namespace ae