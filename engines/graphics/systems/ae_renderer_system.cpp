/// \file renderer_start_pass_system.cpp
/// \brief The script implementing the renderer start pass system class.
/// The renderer start pass system class is implemented.

#include <iterator>
#include "ae_renderer_system.hpp"

namespace ae {
    // Constructor of the RendererStartPassSystem
    RendererStartPassSystem::RendererStartPassSystem(ae_ecs::AeECS& t_ecs,
                                                     GameComponents& t_game_components,
                                                     UpdateUboSystem& t_updateUboSystem,
                                                     TimingSystem& t_timingSystem,
                                                     AeRenderer& t_renderer,
                                                     AeDevice& t_aeDevice,
                                                     AeSamplers& t_aeSamplers) :
                                                     m_updateUboSystem{t_updateUboSystem},
                                                     m_timingSystem{t_timingSystem},
                                                     m_renderer{t_renderer},
                                                     m_aeDevice{t_aeDevice},
                                                     m_aeSamplers{t_aeSamplers},
                                                     m_gameComponents{t_game_components},
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
                .setMaxSets(MAX_FRAMES_IN_FLIGHT*2)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,MAX_TEXTURE_DESCRIPTORS*MAX_FRAMES_IN_FLIGHT)
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
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
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
        // Texture Descriptor Set Initialization
        //==============================================================================================================
        // Define the descriptor set for the texture.
        auto textureSetLayout = AeDescriptorSetLayout::Builder(m_aeDevice)
                .addBinding(0,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURE_DESCRIPTORS)
                .build();

        // Reserve space for and then initialize the texture descriptors for each frame.
        m_defaultImage = AeImage::createModelFromFile(m_aeDevice,"assets/ui_textures/default.jpg");
        m_textureDescriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_defaultImage->getImageView();
        imageInfo.sampler = m_aeSamplers.getDefaultSampler();

        // Initialize the descriptor writer that will be used to update the image data for the textures.
        m_textureDescriptorWriter = new AeDescriptorWriter(textureSetLayout, *m_globalPool);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            // Create the set of images that are the textures for each frame.
            VkDescriptorImageInfo imageInfos[MAX_TEXTURE_DESCRIPTORS];

            // Set the default image information for each frame.
            for (auto & j : imageInfos) {
                j = imageInfo;
            }
            // Initialize the descriptor set for the frame.
            m_textureDescriptorWriter->writeImage(0, imageInfos).build(m_textureDescriptorSets[i]);
        }


        //==============================================================================================================
        // Setup child render systems
        //==============================================================================================================
        m_simpleRenderSystem = new SimpleRenderSystem(t_ecs,
                                                      t_game_components,
                                                      m_aeDevice,
                                                      m_renderer.getSwapChainRenderPass(),
                                                      globalSetLayout->getDescriptorSetLayout(),
                                                      textureSetLayout->getDescriptorSetLayout());

        m_pointLightRenderSystem = new PointLightRenderSystem(t_ecs,
                                                              t_game_components,
                                                              m_aeDevice,
                                                              m_renderer.getSwapChainRenderPass(),
                                                              globalSetLayout->getDescriptorSetLayout());

        m_uiRenderSystem = new UiRenderSystem(t_ecs,
                                              t_game_components,
                                              m_aeDevice,
                                              m_renderer.getSwapChainRenderPass(),
                                              globalSetLayout->getDescriptorSetLayout(),
                                              textureSetLayout->getDescriptorSetLayout());


        // Enable the system
        this->enableSystem();

    };



    // Destructor class of the RendererStartPassSystem
    RendererStartPassSystem::~RendererStartPassSystem(){
        delete m_simpleRenderSystem;
        m_simpleRenderSystem = nullptr;

        delete m_pointLightRenderSystem;
        m_pointLightRenderSystem = nullptr;

        delete m_uiRenderSystem;
        m_uiRenderSystem = nullptr;

        delete m_textureDescriptorWriter;
        m_textureDescriptorWriter = nullptr;

    };



    // There is no setup required for this system to execute.
    void RendererStartPassSystem::setupSystem(){};



    //
    void RendererStartPassSystem::executeSystem(){

        // Unless the renderer is not ready to begin a new frame and returns a nullptr start rendering a new frame.
        if ((m_commandBuffer = m_renderer.beginFrame())) {

            // Get the frame index for the frame the render pass is starting for.
            m_frameIndex = m_renderer.getFrameIndex();

            // Write the ubo data for the shaders for this frame.
            m_uboBuffers[m_frameIndex]->writeToBuffer(m_updateUboSystem.getUbo());
            m_uboBuffers[m_frameIndex]->flush();

            // Update the texture descriptor data for the models that are being rendered.
            updateTextureDescriptorSet();

            // Start the render pass.
            m_renderer.beginSwapChainRenderPass(m_commandBuffer);

            // Call subservient render systems. Order matters here to maintain object transparencies.
            m_simpleRenderSystem->executeSystem(m_commandBuffer,
                                                m_globalDescriptorSets[m_frameIndex],
                                                m_textureDescriptorSets[m_frameIndex],
                                                *m_textureDescriptorWriter,
                                                m_frameIndex);
            m_pointLightRenderSystem->executeSystem(m_commandBuffer,m_globalDescriptorSets[m_frameIndex]);
            m_uiRenderSystem->executeSystem(m_commandBuffer,
                                            m_globalDescriptorSets[m_frameIndex],
                                            m_textureDescriptorSets[m_frameIndex],
                                            *m_textureDescriptorWriter,
                                            m_frameIndex);

            // End the render pass and the frame.
            m_renderer.endSwapChainRenderPass(m_commandBuffer);
            m_renderer.endFrame();
        };
    };



    // There is no clean up required after this system executes.
    void RendererStartPassSystem::cleanupSystem(){};



    // Update the texture descriptor set for the current frame being rendered.
    void RendererStartPassSystem::updateTextureDescriptorSet(){

        // Get entities that might have textures from their respective render systems.
        std::vector<ecs_id> validEntityIds_simpleRenderSystem = m_systemManager.getValidEntities(m_simpleRenderSystem->getSystemId());
        std::vector<ecs_id> validEntityIds_uiRenderSystem = m_systemManager.getValidEntities(m_uiRenderSystem->getSystemId());

        // Declare a vector to store all the unique images that we find.
        std::vector<std::shared_ptr<AeImage>> uniqueImages{};

        // Loop through all the simple render system entities and check to make sure all their textures are included in
        // texture the array and their indexes into that array are set correctly for this frame.
        for(auto entityId:validEntityIds_simpleRenderSystem){
            auto entityModeData = m_gameComponents.modelComponent.getDataReference(entityId);
            if(entityModeData.m_texture == nullptr){
                continue;
            }

            bool imageIsUnique = true;
            for(int i = 0; uniqueImages.size() ; i++){
                if(entityModeData.m_texture == uniqueImages[i]){
                    imageIsUnique = false;
                    break;
                };
            };
            if(imageIsUnique){
                entityModeData.m_texture->setTextureDescriptorIndex(m_frameIndex,uniqueImages.size());
                uniqueImages.push_back(entityModeData.m_texture);
            };
        };

        // Loop through all the ui render system entities and check to make sure all their textures are included in
        // texture the array and their indexes into that array are set correctly for this frame.
        for(auto entityId:validEntityIds_uiRenderSystem){
            auto entityModeData = m_gameComponents.model2DComponent.getDataReference(entityId);
            if(entityModeData.m_texture == nullptr){
                continue;
            }

            bool imageIsUnique = true;
            for(int i = 0; uniqueImages.size()-1; i++){
                if(entityModeData.m_texture == uniqueImages[i]){
                    imageIsUnique = false;
                    break;
                };
            };
            if(imageIsUnique){
                entityModeData.m_texture->setTextureDescriptorIndex(m_frameIndex,uniqueImages.size());
                uniqueImages.push_back(entityModeData.m_texture);
            };
        };

        // Ensure that we're not about to exceed the maximum number of descriptors that we have.
        if(uniqueImages.size() > MAX_TEXTURE_DESCRIPTORS){
            throw std::runtime_error("Too many textures trying to be bound!");
        };

        // Create the set of images that are the textures for each frame.
        VkDescriptorImageInfo imageInfos[MAX_TEXTURE_DESCRIPTORS];

        // Set the image information array for the frame.
        for (auto i=0; i<uniqueImages.size();i++) {
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i].imageView = uniqueImages[i]->getImageView();
            imageInfos[i].sampler = m_aeSamplers.getDefaultSampler();
        }

        // Set the rest to the default image.
        for(auto i=uniqueImages.size(); i<MAX_TEXTURE_DESCRIPTORS;i++){
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i].imageView = m_defaultImage->getImageView();
            imageInfos[i].sampler = m_aeSamplers.getDefaultSampler();
        }

        // Write the texture array to the descriptor set.
        m_textureDescriptorWriter->clearWriteData();
        m_textureDescriptorWriter->writeImage(0, imageInfos);
        m_textureDescriptorWriter->overwrite(m_textureDescriptorSets[m_frameIndex]);
    };

}