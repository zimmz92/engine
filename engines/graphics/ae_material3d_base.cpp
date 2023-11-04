/// \file ae_material3d.cpp
/// The ## class is implemented.
#include "ae_material3d.hpp"

// dependencies

// libraries

// std
#include <stdexcept>
#include <cassert>

namespace ae {

    // Constructor implementation
    AeMaterial3DBase::AeMaterial3DBase(AeDevice &t_aeDevice,
                               VkRenderPass t_renderPass,
                               MaterialShaderFiles& t_materialShaderFiles,
                               VkDescriptorSetLayout t_globalSetLayout,
                               VkDescriptorSetLayout t_textureSetLayout,
                               VkDescriptorSetLayout t_objectSetLayout)
            : m_aeDevice{t_aeDevice},
              m_materialShaderFiles{t_materialShaderFiles}{

        static material_id materialIdCounter = 0;
        this->m_materialID = materialIdCounter++;

        // Creates the pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
        createPipelineLayout(t_globalSetLayout, t_textureSetLayout, t_objectSetLayout);

        // Creates a graphics pipeline for this render system and sets the m_aePipeline member variable.
        createPipeline(t_renderPass);

    };


    // Destructor implementation
    AeMaterial3DBase::~AeMaterial3DBase() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    };


    // Creates the pipeline layout for the point light render system.
    void AeMaterial3DBase::createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout,
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
    void AeMaterial3DBase::createPipeline(VkRenderPass t_renderPass) {

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

    material_id AeMaterial3DBase::getMaterialId(){
        return m_materialID;
    };

} //namespace ae