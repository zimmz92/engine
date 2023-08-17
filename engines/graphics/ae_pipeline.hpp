/*! \file ae_pipeline.hpp
    \brief The script defining the pipeline class.
    The pipeline class is defined.
*/
#pragma once

#include "ae_device.hpp"

// std
#include <string>
#include <vector>

namespace ae {

    /// Information required to create the Vulkan pipeline.
    struct PipelineConfigInfo {

        /// Struct constructor, default currently.
        PipelineConfigInfo() = default;

        /// Do not allow this structure to be copied.
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    /// Class for the Vulkan pipeline object
    class AePipeline {
    public:
        /// Create a Vulkan pipeline object.
        /// \param t_device The graphics device the pipeline will be implemented by.
        /// \param t_vertFilepath The relative path to the SPIR-V file which defines the pipeline's vertex shader.
        /// \param t_fragFilepath The relative path to the SPIR-V file which defines the pipeline's fragment shader.
        /// \param t_configInfo The PipelineConfigInfo struct that defines the required vulkan properties of the
        /// pipeline to be created.
        AePipeline(
            AeDevice& t_device,
            const std::string& t_vertFilepath,
            const std::string& t_fragFilepath,
            const PipelineConfigInfo& t_configInfo);

        /// Destroy a Vulkan pipeline object
        ~AePipeline();

        /// Do not allow this class to be copied
        AePipeline(const AePipeline&) = delete;
        AePipeline& operator=(const AePipeline&) = delete;

        /// Bind a command buffer to this pipeline.
        /// \param t_commandBuffer The command buffer to bind to this pipeline.
        void bind(VkCommandBuffer t_commandBuffer);

        /// Fills the specified configuration information reference with a default pipeline configuration.
        /// \param t_configInfo The pipeline configuration information variable that should be set to the default
        /// configuration.
        static void defaultPipelineConfigInfo(PipelineConfigInfo& t_configInfo);

        /// Sets the pipeline configuration variables to enabled alpha blending.
        /// \param t_configInfo The configuration info object that requires alpha blending configuration variables to be
        /// set.
        static void enableAlphaBlending(PipelineConfigInfo& t_configInfo);

    private:

        /// Reads a text file a vector variable.
        /// \param t_filepath The path to the file to be imported as a vector.
        /// \return A vector with the contents of the specified file.
        static std::vector<char> readFile(const std::string& t_filepath);

        /// Creates a Vulkan graphics pipeline.
        /// \param t_vertFilepath The relative path to the SPIR-V file which defines the pipeline's vertex shader.
        /// \param t_fragFilepath The relative path to the SPIR-V file which defines the pipeline's fragment shader.
        /// \param t_configInfo The PipelineConfigInfo struct that defines the required vulkan properties of the
        /// pipeline to be created.
        void createGraphicsPipeline(
            const std::string& t_vertFilepath,
            const std::string& t_fragFilepath,
            const PipelineConfigInfo& t_configInfo);

        /// Function to create a Vulkan shader module from the supplied reference of code vector variable.
        /// \param t_code The vector containing the code for the shader module to be created.
        /// \param t_shaderModule A pointer to where the shader module should be created.
        void createShaderModule(const std::vector<char>& t_code, VkShaderModule* t_shaderModule);

        // Class variables
        /// The vulkan device this pipeline is implemented on
        AeDevice& m_aeDevice;

        /// This graphics pipeline
        VkPipeline m_graphicsPipeline;

        /// The vertex shader module this pipeline uses, this is just a placeholder for creating the pipeline.
        VkShaderModule m_vertShaderModule;

        /// The fragment shader module this pipeline uses, this is just a placeholder for creating the pipeline.
        VkShaderModule m_fragShaderModule;
    };
}  // namespace lve