/*! \file ae_pipeline.cpp
    \brief The script implementing the pipeline class.
    The pipeline class is implemented.
*/
#include "ae_pipeline.hpp"
#include "ae_model.hpp"

// std
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace ae {

    // Create the Vulkan pipeline object
    AePipeline::AePipeline(
        AeDevice& t_device,
        const std::string& t_vertFilepath,
        const std::string& t_fragFilepath,
        const std::string& t_tessFilepath,
        const std::string& t_geometryFilepath,
        const PipelineConfigInfo& t_configInfo) : m_aeDevice{ t_device } {

        createGraphicsPipeline(t_vertFilepath, t_fragFilepath, t_tessFilepath, t_geometryFilepath, t_configInfo,nullptr);

    }

    // Create the Vulkan pipeline object
    AePipeline::AePipeline(
            AeDevice& t_device,
            const std::string& t_vertFilepath,
            const std::string& t_fragFilepath,
            const std::string& t_tessFilepath,
            const std::string& t_geometryFilepath,
            const PipelineConfigInfo& t_configInfo,
            uint32_t& t_materialId) : m_aeDevice{ t_device } {

        VkSpecializationMapEntry entry = { 0, 0, sizeof(int32_t) };
        VkSpecializationInfo spec_info = {
                1,
                &entry,
                sizeof(uint32_t),
                &t_materialId
        };

        createGraphicsPipeline(t_vertFilepath, t_fragFilepath, t_tessFilepath, t_geometryFilepath, t_configInfo, &spec_info);

    }



    // Destroy the Vulkan pipeline object
    AePipeline::~AePipeline() {
        vkDestroyPipeline(m_aeDevice.device(), m_graphicsPipeline, nullptr);
    }

    // Function to read a text file into a variable.
    std::vector<char> AePipeline::readFile(const std::string& t_filepath) {
        // Create an input file stream. Go to the end of the file to start and when reading in the file do so in binary
        // format.
        std::ifstream file{ t_filepath, std::ios::ate | std::ios::binary };

        // Fail out if file cannot be opened.
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + t_filepath);
        }

        // Since the file was opened and by default seeked the end of file the current position is the size of the file.
        size_t fileSize = static_cast<size_t>(file.tellg());

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
    void AePipeline::createGraphicsPipeline(
        const std::string& t_vertFilepath,
        const std::string& t_fragFilepath,
        const std::string& t_tessFilepath,
        const std::string& t_geometryFilepath,
        const PipelineConfigInfo& t_configInfo,
        VkSpecializationInfo* spec_info) {

        assert(
            t_configInfo.pipelineLayout != nullptr &&
            "Cannot create graphics pipeline: no pipelineLayout provided in config info");

        assert(
            t_configInfo.renderPass != nullptr &&
            "Cannot create graphics pipeline: no renderPass provided in config info");

        // Create a collection of shader stages information to create the pipeline using.
        std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo;

        // If a vertex shader has been provided create the information for it.
        VkShaderModule vertShaderModule;
        if(t_vertFilepath != "Not Used"){

            // Read in the vertex shader code from the specified file.
            auto vertCode = readFile(t_vertFilepath);

            // Create the vertex shader module the imported shader code
            createShaderModule(vertCode, &vertShaderModule);

            // Specify the vertex shader
            VkPipelineShaderStageCreateInfo shaderStage;
            shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
            shaderStage.module = vertShaderModule;
            shaderStage.pName = "main";
            shaderStage.flags = 0;
            shaderStage.pNext = nullptr;
            shaderStage.pSpecializationInfo = spec_info;

            shaderStagesInfo.push_back(shaderStage);
        }else{
            throw std::runtime_error("A vertex shader must be provided!");
        }

        // If a fragment shader has been provided, create the information for it.
        VkShaderModule fragShaderModule;
        if(t_fragFilepath != "Not Used"){

            // Read in the fragment shader code from the specified file.
            auto fragCode = readFile(t_fragFilepath);

            // Create the fragment shader module the imported shader code
            createShaderModule(fragCode, &fragShaderModule);

            // Specify the fragment shader
            VkPipelineShaderStageCreateInfo shaderStage;
            shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderStage.module = fragShaderModule;
            shaderStage.pName = "main";
            shaderStage.flags = 0;
            shaderStage.pNext = nullptr;
            shaderStage.pSpecializationInfo = spec_info;

            shaderStagesInfo.push_back(shaderStage);
        }else{
            throw std::runtime_error("A fragment shader must be provided!");
        }

        // If a tesselation control shader has been provided, create the information for it.
        VkShaderModule tessShaderModule;
        if(t_tessFilepath != "Not Used"){

            // Read in the fragment shader code from the specified file.
            auto tessCode = readFile(t_tessFilepath);

            // Create the fragment shader module the imported shader code
            createShaderModule(tessCode, &tessShaderModule);

            // Specify the fragment shader
            VkPipelineShaderStageCreateInfo shaderStage;
            shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStage.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            shaderStage.module = tessShaderModule;
            shaderStage.pName = "main";
            shaderStage.flags = 0;
            shaderStage.pNext = nullptr;
            shaderStage.pSpecializationInfo = spec_info;

            shaderStagesInfo.push_back(shaderStage);
        }

        // If a tesselation control shader has been provided, create the information for it.
        VkShaderModule geometryShaderModule;
        if(t_geometryFilepath != "Not Used"){

            // Read in the fragment shader code from the specified file.
            auto geometryCode = readFile(t_geometryFilepath);

            // Create the fragment shader module the imported shader code
            createShaderModule(geometryCode, &geometryShaderModule);

            // Specify the fragment shader
            VkPipelineShaderStageCreateInfo shaderStage;
            shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStage.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            shaderStage.module = geometryShaderModule;
            shaderStage.pName = "main";
            shaderStage.flags = 0;
            shaderStage.pNext = nullptr;
            shaderStage.pSpecializationInfo = spec_info;

            shaderStagesInfo.push_back(shaderStage);
        }



        // Configure the vertex shader, how to interpret the vertex buffer input.
        auto& bindingDescriptions = t_configInfo.bindingDescriptions;
        auto& attributeDescriptions = t_configInfo.attributeDescriptions;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());;
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        // Configure the graphics pipeline
        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStagesInfo.size();
        pipelineInfo.pStages = shaderStagesInfo.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &t_configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &t_configInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &t_configInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &t_configInfo.multisampleInfo;
        pipelineInfo.pColorBlendState = &t_configInfo.colorBlendInfo;
        pipelineInfo.pDynamicState = &t_configInfo.dynamicStateInfo;
        pipelineInfo.pDepthStencilState = &t_configInfo.depthStencilInfo;

        pipelineInfo.layout = t_configInfo.pipelineLayout;
        pipelineInfo.renderPass = t_configInfo.renderPass;
        pipelineInfo.subpass = t_configInfo.subpass;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional
        pipelineInfo.basePipelineIndex = -1;               // Optional

        // Attempt to create the graphics pipeline, error out if not possible
        if (vkCreateGraphicsPipelines( m_aeDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        // Clean up the shader modules now that they have been loaded into the pipeline.
        if(t_vertFilepath != "Not Used") {
            vkDestroyShaderModule(m_aeDevice.device(), fragShaderModule, nullptr);
            fragShaderModule = VK_NULL_HANDLE;
        }

        if(t_fragFilepath != "Not Used") {
            vkDestroyShaderModule(m_aeDevice.device(), vertShaderModule, nullptr);
            vertShaderModule = VK_NULL_HANDLE;
        }

        if(t_tessFilepath != "Not Used") {
            vkDestroyShaderModule(m_aeDevice.device(), tessShaderModule, nullptr);
            tessShaderModule = VK_NULL_HANDLE;
        }

        if(t_geometryFilepath != "Not Used") {
            vkDestroyShaderModule(m_aeDevice.device(), geometryShaderModule, nullptr);
            geometryShaderModule = VK_NULL_HANDLE;
        }
    }

    // Function to create a Vulkan shader module
    void AePipeline::createShaderModule(const std::vector<char>& t_code, VkShaderModule* t_shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = t_code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(t_code.data());

        if (vkCreateShaderModule(m_aeDevice.device(), &createInfo, nullptr, t_shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module");
        }
    }

    void AePipeline::bind(VkCommandBuffer t_commandBuffer) {
        vkCmdBindPipeline(t_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
    }

    void AePipeline::defaultPipelineConfigInfo(PipelineConfigInfo& t_configInfo) {

        t_configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

        // Each group of three vertices are grouped into a triangle.
        t_configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        // Used to break up line strip type of topology.
        t_configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        // Configure the viewport
        t_configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        t_configInfo.viewportInfo.viewportCount = 1;
        t_configInfo.viewportInfo.pViewports = nullptr;
        t_configInfo.viewportInfo.scissorCount = 1;
        t_configInfo.viewportInfo.pScissors = nullptr;

        // Breaks up geometry into fragments for each pixel it overlaps
        t_configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

        // Enabling this will require a GPU setting
        t_configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        t_configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        t_configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        t_configInfo.rasterizationInfo.lineWidth = 1.0f;

        // Will cull triangles depending on which way they are being viewed.
        t_configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        t_configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        t_configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        t_configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
        t_configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
        t_configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

        // Relates how the rasterizer handles edges of an object
        t_configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        t_configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        t_configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        t_configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
        t_configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
        t_configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
        t_configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

        // How colors are combined in frame buffer
        t_configInfo.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        t_configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        t_configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        t_configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        t_configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        t_configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        t_configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        t_configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

        t_configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        t_configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
        t_configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
        t_configInfo.colorBlendInfo.attachmentCount = 1;
        t_configInfo.colorBlendInfo.pAttachments = &t_configInfo.colorBlendAttachment;
        t_configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
        t_configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
        t_configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
        t_configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

        // Stores value for every fragment in the buffer
        t_configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        t_configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        t_configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        t_configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        t_configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        t_configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
        t_configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
        t_configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        t_configInfo.depthStencilInfo.front = {};  // Optional
        t_configInfo.depthStencilInfo.back = {};   // Optional

        // Configures the pipeline to expect a dynamic view port and dynamic scissor after creation.
        t_configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        t_configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        t_configInfo.dynamicStateInfo.pDynamicStates = t_configInfo.dynamicStateEnables.data();
        t_configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(t_configInfo.dynamicStateEnables.size());
        t_configInfo.dynamicStateInfo.flags = 0;

        // Specify that this pipeline by default will be dealing with model vertex buffers.
        t_configInfo.bindingDescriptions = AeModel::Vertex::getBindingDescriptions();
        t_configInfo.attributeDescriptions = AeModel::Vertex::getAttributeDescriptions();
    }

    void AePipeline::enableAlphaBlending(PipelineConfigInfo& t_configInfo) {
        // How colors are combined in the frame buffer
        t_configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
        t_configInfo.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        t_configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;   // Optional
        t_configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;  // farthest to closest
        t_configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        t_configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        t_configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        t_configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
    }

}  // namespace ae