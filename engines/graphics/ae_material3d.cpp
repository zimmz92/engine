/// \file ae_material3d.cpp
/// The ## class is implemented.
#include "ae_material3d.hpp"

// dependencies

// libraries

// std


namespace ae {

        // Constructor implementation
        template <int requiredTextures>
        AeMaterial3D<requiredTextures>::AeMaterial3D(ae_ecs::AeECS& t_ecs,
                                               GameComponents &t_game_components,
                                               AeDevice &t_aeDevice,
                                               VkRenderPass t_renderPass,
                                               VkDescriptorSetLayout t_globalSetLayout,
                                               VkDescriptorSetLayout t_textureSetLayout,
                                               VkDescriptorSetLayout t_objectSetLayout)
                : m_worldPositionComponent{t_game_components.worldPositionComponent},
                  m_modelComponent{t_game_components.modelComponent},
                  m_aeDevice{t_aeDevice},
                  ae_ecs::AeSystem<AeMaterial3D<requiredTextures>>(t_ecs) {

            // Register component dependencies
            m_worldPositionComponent.requiredBySystem(this->getSystemId());
            m_modelComponent.requiredBySystem(this->getSystemId());


            // Register system dependencies
            // This is a child system and dependencies, as well as execution, will be handled by the parent system,
            // RendererSystem.
            this->isChildSystem = true;

            // Creates the pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
            createPipelineLayout(t_globalSetLayout, t_textureSetLayout, t_objectSetLayout);

            // Creates a graphics pipeline for this render system and sets the m_aePipeline member variable.
            createPipeline(t_renderPass);

            // Enable the system so it will run.
            this->enableSystem();
        };


        // Destructor implementation
        template <int requiredTextures>
        AeMaterial3D<requiredTextures>::~AeMaterial3D() {
            vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
        };


        // Set up the system prior to execution. Currently not used.
        template <int requiredTextures>
        void AeMaterial3D<requiredTextures>::setupSystem() {};


        // Renders the models.
        template <int requiredTextures>
        void AeMaterial3D<requiredTextures>::executeSystem(VkCommandBuffer &t_commandBuffer,
                                               VkDescriptorSet t_globalDescriptorSet,
                                               VkDescriptorSet t_textureDescriptorSet,
                                               VkDescriptorSet t_objectDescriptorSet,
                                               uint64_t t_frameIndex) {

            // Tell the pipeline what the current command buffer being worked on is.
            m_aePipeline->bind(t_commandBuffer);

            VkDescriptorSet descriptorSetsToBind[] = {t_globalDescriptorSet,
                                                      t_textureDescriptorSet,
                                                      t_objectDescriptorSet};

            // Bind the descriptor sets to the command buffer.
            vkCmdBindDescriptorSets(
                    t_commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_pipelineLayout,
                    0,
                    3,
                    descriptorSetsToBind,
                    0,
                    nullptr);


            // Get the entities that use the components this system depends on.
            std::vector<ecs_id> validEntityIds = ae_ecs::AeSystem<AeMaterial3D<requiredTextures>>::m_systemManager.getEnabledSystemsEntities(this->getSystemId());


            // Loop through the entities if they have models render them.
            int j=0;
            for (ecs_id entityId: validEntityIds) {

                // Make sure the entity actually has a model to render.
                const ModelComponentStruct& entityModelData = m_modelComponent.getReadOnlyDataReference(entityId);
                if (entityModelData.m_model == nullptr) continue;

                // Bind the model buffer(s) to the command buffer.
                entityModelData.m_model->bind(t_commandBuffer);

                // Draw the model.
                entityModelData.m_model->draw(t_commandBuffer, j);
                j++;
            };
        };


        // Clean up the system after execution. Currently not used.
        template <int requiredTextures>
        void AeMaterial3D<requiredTextures>::cleanupSystem() {
            ae_ecs::AeSystem<AeMaterial3D<requiredTextures>>::m_systemManager.clearSystemEntityUpdateSignatures(ae_ecs::AeSystem<AeMaterial3D<requiredTextures>>::m_systemId);
        };


        // Creates the pipeline layout for the point light render system.
        template <int requiredTextures>
        void AeMaterial3D<requiredTextures>::createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout,
                                                      VkDescriptorSetLayout t_textureSetLayout,
                                                      VkDescriptorSetLayout t_objectSetLayout) {


            // Prepare the descriptor set layouts based on the global set layout for the device.
            std::vector<VkDescriptorSetLayout> descriptorSetLayouts{t_globalSetLayout,
                                                                    t_textureSetLayout,
                                                                    t_objectSetLayout};

            // Define the specific layout of the point light renderer.
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
            pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

            // Attempt to create the pipeline layout, if it cannot error out.
            if (vkCreatePipelineLayout(m_aeDevice.device(),
                                       &pipelineLayoutInfo,
                                       nullptr,
                                       &m_pipelineLayout)
                != VK_SUCCESS) {
                throw std::runtime_error("Failed to create the simple render system's pipeline layout!");
            }

        };


        // Creates the pipeline for the point light render system.
        template <int requiredTextures>
        void AeMaterial3D<requiredTextures>::createPipeline(VkRenderPass t_renderPass) {

            // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
            assert(m_pipelineLayout != nullptr &&
                   "Cannot create the simple render system's pipeline before pipeline layout!");

            // Define the pipeline to be created.
            PipelineConfigInfo pipelineConfig{};
            AePipeline::defaultPipelineConfigInfo(pipelineConfig);
            pipelineConfig.renderPass = t_renderPass;
            pipelineConfig.pipelineLayout = m_pipelineLayout;

            // Create the point light render system pipeline.
            m_aePipeline = std::make_unique<AePipeline>(
                    m_aeDevice,
                    "engines/graphics/shaders/shader.vert.spv",
                    "engines/graphics/shaders/shader.frag.spv",
                    pipelineConfig);
        };



        //
        template <int requiredTextures>
        void AeMaterial3D<requiredTextures>::registerEntity(ecs_id t_entityId, AeModel& t_model3D, std::shared_ptr<AeImage> textures[requiredTextures]){
            std::map<AeModel,std::map<ecs_id,uint64_t>>::const_iterator pos = draws.find(t_model3D);
            if (pos == draws.end()) {
                //handle the error
            } else {

            }
        };

} //namespace ae