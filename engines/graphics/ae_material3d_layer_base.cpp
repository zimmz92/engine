/// \file ae_material3d.cpp
/// The ## class is implemented.
#include "ae_material3d_layer_base.hpp"

// dependencies

// libraries

// std
#include <stdexcept>
#include <cassert>

namespace ae {

    // Constructor implementation
    AeMaterial3DLayerBase::AeMaterial3DLayerBase(AeDevice &t_aeDevice,
                                                 VkRenderPass t_renderPass,
                                                 MaterialShaderFiles& t_materialShaderFiles,
                                                 std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts)
            : m_aeDevice{t_aeDevice},
              m_materialShaderFiles{t_materialShaderFiles}{

        static material_id materialIdCounter = 0;
        this->m_materialID = materialIdCounter++;

        // Creates the pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
        createPipelineLayout(t_descriptorSetLayouts);

        // Creates a graphics pipeline for this render system and sets the m_aePipeline member variable.
        createPipeline(t_renderPass);

    };


    // Destructor implementation
    AeMaterial3DLayerBase::~AeMaterial3DLayerBase() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    };


    // Creates the pipeline layout for the point light render system.
    void AeMaterial3DLayerBase::createPipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts) {

        // Define the layout of the material layer.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(t_descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = t_descriptorSetLayouts.data();

        // Attempt to create the pipeline layout, if it cannot error out.
        if (vkCreatePipelineLayout(m_aeDevice.device(),
                                   &pipelineLayoutInfo,
                                   nullptr,
                                   &m_pipelineLayout)
            != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the simple render system's pipeline layout!");
        }

    };


    // Creates the pipeline.
    void AeMaterial3DLayerBase::createPipeline(VkRenderPass t_renderPass) {

        // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
        assert(m_pipelineLayout != nullptr &&
               "Cannot create the simple render system's pipeline before pipeline layout!");

        // Define the pipeline to be created.
        PipelineConfigInfo pipelineConfig{};
        AePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = t_renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;

        // Create the material layer pipeline.
        m_pipeline = std::make_unique<AePipeline>(
                m_aeDevice,
                m_materialShaderFiles.vertexShaderFilepath,
                m_materialShaderFiles.fragmentShaderFilepath,
                m_materialShaderFiles.tessellationShaderFilepath,
                m_materialShaderFiles.geometryShaderFilepath,
                pipelineConfig,
                m_materialID);
    };

    material_id AeMaterial3DLayerBase::getMaterialId(){
        return m_materialID;
    };

} //namespace ae