/// \file ae_material3d.cpp
/// The ## class is implemented.
#include "ae_3d_material_layer_base.hpp"

// dependencies

// libraries

// std
#include <stdexcept>
#include <cassert>

namespace ae {

    // Constructor implementation
    Ae3DMaterialLayerBase::Ae3DMaterialLayerBase(AeDevice &t_aeDevice,
                                                 VkRenderPass t_renderPass,
                                                 MaterialShaderFiles& t_materialShaderFiles,
                                                 std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts)
            : m_aeDevice{t_aeDevice},
              m_materialShaderFiles{t_materialShaderFiles}{

        // Create a counter for the material layer IDs. This will allow them to be identified by the renderer.
        static material_id materialIdCounter = 0;
        // Increment the counter for each material that is created.
        this->m_materialID = materialIdCounter++;

        // Creates the pipeline layout accounting for the global layout and sets the m_pipelineLayout member variable.
        createPipelineLayout(t_descriptorSetLayouts);

        // Creates a graphics pipeline for this render system and sets the m_aePipeline member variable.
        createPipeline(t_renderPass);

    };


    // Destructor implementation
    Ae3DMaterialLayerBase::~Ae3DMaterialLayerBase() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    };


    // Creates the pipeline layout for the point light render system.
    void Ae3DMaterialLayerBase::createPipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts) {

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
    void Ae3DMaterialLayerBase::createPipeline(VkRenderPass t_renderPass) {

        // Ensure the pipeline layout has already been created, cannot create a pipeline otherwise.
        assert(m_pipelineLayout != nullptr &&
               "Cannot create the simple render system's pipeline before pipeline layout!");

        // Define the pipeline to be created.
        PipelineConfigInfo pipelineConfig{};
        AePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = t_renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;

        shaderFilesPaths shaderPaths{};
        shaderPaths.vertFilepath = m_materialShaderFiles.vertexShaderFilepath;
        shaderPaths.fragFilepath = m_materialShaderFiles.fragmentShaderFilepath;
        shaderPaths.tessFilepath = m_materialShaderFiles.tessellationShaderFilepath;
        shaderPaths.geometryFilepath = m_materialShaderFiles.geometryShaderFilepath;

        // Create the material layer pipeline.
        m_pipeline = std::make_unique<AePipeline>(
                m_aeDevice,
                shaderPaths,
                pipelineConfig,
                m_materialID);
    };

    // Get the material layer's ID.
    material_id Ae3DMaterialLayerBase::getMaterialLayerId() const{
        return m_materialID;
    };

    // Bind the pipeline to the specified command buffer.
    void Ae3DMaterialLayerBase::bindPipeline(VkCommandBuffer& t_commandBuffer){
        m_pipeline->bind(t_commandBuffer);
    }

    // Get the material layer's pipeline layout.
    VkPipelineLayout& Ae3DMaterialLayerBase::getPipelineLayout(){
        return m_pipelineLayout;
    }

} //namespace ae