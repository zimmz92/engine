/// \file ae_material3d.cpp
/// The ## class is implemented.
#include "ae_material3d.hpp"

// dependencies

// libraries

// std


namespace ae {

    // Constructor implementation
    template <typename T>
    AeMaterial3D<T>::AeMaterial3D(ae_ecs::AeECS& t_ecs,
                                  AeDevice &t_aeDevice,
                                  VkRenderPass t_renderPass,
                                  MaterialShaderFiles t_materialShaderFiles,
                                  VkDescriptorSetLayout t_globalSetLayout,
                                  VkDescriptorSetLayout t_textureSetLayout,
                                  VkDescriptorSetLayout t_objectSetLayout)
            : m_aeDevice{t_aeDevice},
              m_materialShaderFiles{t_materialShaderFiles},
              ae_ecs::AeSystem<AeMaterial3D<T>>(t_ecs) {

        // Materials are children systems to the renderer system is a child system. Dependencies on other systems, as
        // well as execution, will be handled by the parent system, RendererSystem.
        this->isChildSystem = true;

        // Creates the pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
        createPipelineLayout(t_globalSetLayout, t_textureSetLayout, t_objectSetLayout);

        // Creates a graphics pipeline for this render system and sets the m_aePipeline member variable.
        createPipeline(t_renderPass);

        // Enable the system so it will run.
        this->enableSystem();
    };


    // Destructor implementation
    template <typename T>
    AeMaterial3D<T>::~AeMaterial3D() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    };


    // Set up the system prior to execution. Currently not used.
    template <typename T>
    void AeMaterial3D<T>::setupSystem() {};


    // Renders the models.
    template <typename T>
    void AeMaterial3D<T>::executeSystem() {

//        // Tell the pipeline what the current command buffer being worked on is.
//        m_aePipeline->bind(t_commandBuffer);
//
//        VkDescriptorSet descriptorSetsToBind[] = {t_globalDescriptorSet,
//                                                  t_textureDescriptorSet,
//                                                  t_objectDescriptorSet};
//
//        // Bind the descriptor sets to the command buffer.
//        vkCmdBindDescriptorSets(
//                t_commandBuffer,
//                VK_PIPELINE_BIND_POINT_GRAPHICS,
//                m_pipelineLayout,
//                0,
//                3,
//                descriptorSetsToBind,
//                0,
//                nullptr);


        // Get the entities that use the components this system depends on.
        std::vector<ecs_id> validEntityIds = ae_ecs::AeSystem<AeMaterial3D<T>>::m_systemManager.getUpdatedSystemEntities(this->getSystemId());


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
    template <typename T>
    void AeMaterial3D<T>::cleanupSystem() {
        ae_ecs::AeSystem<AeMaterial3D<T>>::m_systemManager.clearSystemEntityUpdateSignatures(ae_ecs::AeSystem<AeMaterial3D<T>>::m_systemId);
    };


    // Creates the pipeline layout for the point light render system.
    template <typename T>
    void AeMaterial3D<T>::createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout,
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
    template <typename T>
    void AeMaterial3D<T>::createPipeline(VkRenderPass t_renderPass) {

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
                m_materialShaderFiles.vertexShaderFilepath,
                m_materialShaderFiles.fragmentShaderFilepath,
                m_materialShaderFiles.tessellationShaderFilepath,
                m_materialShaderFiles.geometryShaderFilepath,
                pipelineConfig);
    };



    //
    template <typename T>
    void AeMaterial3D<T>::registerEntity(ecs_id t_entityId, AeModel& t_model3D, T t_entityMaterialProperties){
        std::map<AeModel,std::map<ecs_id,uint64_t>>::const_iterator pos = draws.find(t_model3D);
        if (pos == draws.end()) {
            //handle the error
        } else {

        }
    };

} //namespace ae