#pragma once

#include "ae_device.hpp"

// std
#include <string>
#include <vector>

namespace ae {

    struct PipelineConfigInfo {
        VkViewport viewport;
        VkRect2D scissor;
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

    class AePipeline {
    public:
        AePipeline(
            AeDevice& t_device,
            const std::string& t_vertFilepath,
            const std::string& t_fragFilepath,
            const PipelineConfigInfo& t_configInfo);
        ~AePipeline();

        // Do not allow this class to be copied (2 lines below)
        AePipeline(const AePipeline&) = delete;
        void operator=(const AePipeline&) = delete;

        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t t_width, uint32_t t_height);

    private:
        static std::vector<char> readFile(const std::string& t_filepath);

        void createGraphicsPipeline(
            const std::string& t_vertFilepath,
            const std::string& t_fragFilepath,
            const PipelineConfigInfo& t_configInfo);

        void createShaderModule(const std::vector<char>& t_code, VkShaderModule* t_shaderModule);

        AeDevice& m_aeDevice;
        VkPipeline m_graphicsPipeline;
        VkShaderModule m_vertShaderModule;
        VkShaderModule m_fragShaderModule;
    };
}  // namespace lve