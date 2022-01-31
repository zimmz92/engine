#pragma once

#include "ae_device.hpp"

// std
#include <string>
#include <vector>

namespace ae {

    // Information required to create the a Vulkan pipeline 
    struct PipelineConfigInfo {
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    // Class for a the Vulkan pipeline object
    class AePipeline {
    public:
        // Create a Vulkan pipeline object
        AePipeline(
            AeDevice& t_device,
            const std::string& t_vertFilepath,
            const std::string& t_fragFilsepath,
            const PipelineConfigInfo& t_configInfo);

        // Destroy a Vulkan pipeline object
        ~AePipeline();

        // Do not allow this class to be copied
        AePipeline(const AePipeline&) = delete;
        void operator=(const AePipeline&) = delete;

        //
        void bind(VkCommandBuffer t_commandBuffer);

        // Function to create a default pipeline
        static void defaultPipelineConfigInfo(PipelineConfigInfo& t_configInfo, uint32_t t_width, uint32_t t_height);

    private:

        // Function to read a text file into a variable
        static std::vector<char> readFile(const std::string& t_filepath);

        // Function to create a Vulkan graphics pipeline
        void createGraphicsPipeline(
            const std::string& t_vertFilepath,
            const std::string& t_fragFilepath,
            const PipelineConfigInfo& t_configInfo);

        // Function to create a Vulkan shader module
        void createShaderModule(const std::vector<char>& t_code, VkShaderModule* t_shaderModule);

        // Class variables
        AeDevice& m_aeDevice;
        VkPipeline m_graphicsPipeline;
        VkShaderModule m_vertShaderModule;
        VkShaderModule m_fragShaderModule;
    };
}  // namespace lve