/// \file ae_material3d.cpp
/// The ## class is implemented.
#include "ae_material3d.hpp"

// dependencies

// libraries

// std


namespace ae {

    // Constructor implementation
    AeMaterial3D::AeMaterial3D(ae_ecs::AeECS& t_ecs,
                                  AeDevice &t_aeDevice,
                                  VkRenderPass t_renderPass,
                                  GameComponents& t_gameComponents,
                                  MaterialShaderFiles t_materialShaderFiles,
                                  VkDescriptorSetLayout t_globalSetLayout,
                                  VkDescriptorSetLayout t_textureSetLayout,
                                  VkDescriptorSetLayout t_objectSetLayout)
            : m_aeDevice{t_aeDevice},
              m_materialShaderFiles{t_materialShaderFiles},
              m_modelComponent{t_gameComponents.modelComponent},
              ae_ecs::AeSystem<AeMaterial3D>(t_ecs) {

    // The 3D material system will only update if an entities model updates. Otherwise, if an entity
    // changes its material it will need to register/unregister with those material(s).
    t_gameComponents.modelComponent.requiredBySystem(this->m_systemId);

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
AeMaterial3D::~AeMaterial3D() {
    vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
};


// Set up the system prior to execution. Currently not used.
void AeMaterial3D::setupSystem() {};


// Renders the models.
void AeMaterial3D::executeSystem() {

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
//
//
//        // Get the entities that use the components this system depends on.
//        std::vector<ecs_id> validEntityIds = ae_ecs::AeSystem<AeMaterial3D<T>>::m_systemManager.getEnabledSystemEntities(this->getSystemId());
//
//
//        // Loop through the entities if they have models render them.
//        int j=0;
//        for (ecs_id entityId: validEntityIds) {
//
//            // Make sure the entity actually has a model to render.
//            const ModelComponentStruct& entityModelData = m_modelComponent.getReadOnlyDataReference(entityId);
//            if (entityModelData.m_model == nullptr) continue;
//
//            // Bind the model buffer(s) to the command buffer.
//            entityModelData.m_model->bind(t_commandBuffer);
//
//            // Draw the model.
//            entityModelData.m_model->draw(t_commandBuffer, j);
//            j++;
//        };

    // Get the entities that use this material which have updated in a way that impacts the components this system
    // utilizes.
    std::vector<ecs_id> validEntityIds = ae_ecs::AeSystem<AeMaterial3D>::m_systemManager.getUpdatedSystemEntities(this->getSystemId());

    for(auto entityID:validEntityIds){

        // Get the model data for the entity.
        const ModelComponentStruct& entityModel = m_modelComponent.getReadOnlyDataReference(entityID);

        // Check to see if the
        std::map<std::shared_ptr<AeModel>,std::map<ecs_id,uint64_t>>::const_iterator pos = m_uniqueModelMap.find(entityModel.m_model);
        if (pos != m_uniqueModelMap.end()) {
            //handle the error
        }
    }


};


// Clean up the system after execution. Currently not used.
void AeMaterial3D::cleanupSystem() {
    m_systemManager.clearSystemEntityUpdateSignatures(m_systemId);
};


// Creates the pipeline layout for the point light render system.
void AeMaterial3D::createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout,
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
void AeMaterial3D::createPipeline(VkRenderPass t_renderPass) {

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
void AeMaterial3D::registerEntity(ecs_id t_entityId, Model3DMaterialProperties& t_entityMaterialProperties){

    auto entityModel = m_modelComponent.getReadOnlyDataReference(t_entityId);
    std::map<std::shared_ptr<AeModel>,std::map<ecs_id,uint64_t>>::const_iterator pos = m_uniqueModelMap.find(entityModel.m_model);
    if (pos == m_uniqueModelMap.end()) {
        //handle the error
    } else {

    }
};

} //namespace ae