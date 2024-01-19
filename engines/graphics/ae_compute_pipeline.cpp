/*! \file ae_pipeline.cpp
    \brief The script implementing the pipeline class.
    The pipeline class is implemented.
*/
#include "ae_compute_pipeline.hpp"
#include "ae_model.hpp"

// std
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace ae {

    // Create the Vulkan pipeline object
    AeComputePipeline::AeComputePipeline(AeDevice& t_device,
                                         VkPipelineLayout& t_pipelineLayout,
                                         const std::string& t_computeFilepath) : m_aeDevice{ t_device } {

        createComputePipeline(t_computeFilepath, t_pipelineLayout, nullptr);

    }


    // Destroy the Vulkan pipeline object
    AeComputePipeline::~AeComputePipeline() {
        vkDestroyPipeline(m_aeDevice.device(), m_computePipeline, nullptr);
    }

    // Function to read a text file into a variable.
    std::vector<char> AeComputePipeline::readFile(const std::string& t_filepath) {
        // Create an input file stream. Go to the end of the file to start and when reading in the file do so in binary
        // format.
        std::ifstream file{ t_filepath, std::ios::ate | std::ios::binary };

        // Fail out if file cannot be opened.
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + t_filepath);
        }

        // Since the file was opened and by default seeked the end of file the current position is the size of the file.
        std::streamsize fileSize = file.tellg();

        // Create a local variable to store file contents.
        std::vector<char> buffer(fileSize);

        // Go to the beginning of the file.
        file.seekg(0);

        // Read file contents into the buffer.
        file.read(buffer.data(), fileSize);

        // Close the file and return the file contents
        file.close();
        return buffer;
    }

    // Function to create a Vulkan graphics pipeline
    void AeComputePipeline::createComputePipeline( const std::string& t_computeFilepath,
                                                   VkPipelineLayout& t_pipelineLayout,
                                                   VkSpecializationInfo* t_spec_info){

        assert(t_pipelineLayout != nullptr &&
            "Cannot create compute pipeline: no pipelineLayout provided in config info");

        // Read in the vertex shader code from the specified file.
        auto computeCode = readFile(t_computeFilepath);

        // If a vertex shader has been provided create the information for it.
        VkShaderModule computeShaderModule;

        // Create the vertex shader module the imported shader code
        createShaderModule(computeCode, &computeShaderModule);

        // Specify the vertex shader
        VkPipelineShaderStageCreateInfo computeShaderStage;
        computeShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        computeShaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeShaderStage.module = computeShaderModule;
        computeShaderStage.pName = "main";
        computeShaderStage.flags = 0;
        computeShaderStage.pNext = nullptr;
        computeShaderStage.pSpecializationInfo = t_spec_info;

        // Configure the graphics pipeline
        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = t_pipelineLayout;
        pipelineInfo.stage = computeShaderStage;

        // Attempt to create the graphics pipeline, error out if not possible
        if (vkCreateComputePipelines( m_aeDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_computePipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute pipeline!");
        }

        // Clean up the shader modules now that they have been loaded into the pipeline.
        vkDestroyShaderModule(m_aeDevice.device(), computeShaderModule, nullptr);
        computeShaderModule = VK_NULL_HANDLE;
    }

    // Function to create a Vulkan shader module
    void AeComputePipeline::createShaderModule(const std::vector<char>& t_code, VkShaderModule* t_shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = t_code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(t_code.data());

        if (vkCreateShaderModule(m_aeDevice.device(), &createInfo, nullptr, t_shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module");
        }
    }

    void AeComputePipeline::bind(VkCommandBuffer t_commandBuffer) {
        vkCmdBindPipeline(t_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_computePipeline);
    }

    // Creates the pipeline layout for the point light render system.
    void AeComputePipeline::createDefaultComputePipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts,
                                                               VkPipelineLayout& t_pipelineLayout) {

        // Define the specific layout of the point light renderer.
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(t_descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = t_descriptorSetLayouts.data();

        // Attempt to create the pipeline layout, if it cannot error out.
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &t_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the default compute pipeline layout!");
        }

    };

}  // namespace ae