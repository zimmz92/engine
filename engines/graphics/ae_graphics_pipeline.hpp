/*! \file ae_pipeline.hpp
    \brief The script defining the pipeline class.
    The pipeline class is defined.
*/
#pragma once

#include "ae_graphics_constants.hpp"
#include "ae_device.hpp"

// std
#include <string>
#include <vector>

namespace ae {

    /// Information required to create the Vulkan pipeline.
    struct GraphicsPipelineConfigInfo {

        /// Struct constructor, default currently.
        GraphicsPipelineConfigInfo() = default;

        /// Do not allow this structure to be copied.
        GraphicsPipelineConfigInfo(const GraphicsPipelineConfigInfo&) = delete;
        GraphicsPipelineConfigInfo& operator=(const GraphicsPipelineConfigInfo&) = delete;

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

    struct GraphicsShaderFilesPaths{
        std::string vertFilepath = "Not Used";
        std::string fragFilepath = "Not Used";
        std::string tessFilepath = "Not Used";
        std::string geometryFilepath = "Not Used";
    };

    /// Class for the Vulkan pipeline object
    class AeGraphicsPipeline {
    public:
        /// Create a Vulkan pipeline object.
        /// \param t_device The graphics device the pipeline will be implemented by.
        /// \param t_vertFilepath The relative path to the SPIR-V file which defines the pipeline's vertex shader.
        /// \param t_fragFilepath The relative path to the SPIR-V file which defines the pipeline's fragment shader.
        /// \param t_configInfo The GraphicsPipelineConfigInfo struct that defines the required vulkan properties of the
        /// pipeline to be created.
        AeGraphicsPipeline(
            AeDevice& t_device,
            const GraphicsShaderFilesPaths& t_shaderFilePaths,
            const GraphicsPipelineConfigInfo& t_configInfo);

        AeGraphicsPipeline(
                AeDevice& t_device,
                const GraphicsShaderFilesPaths& t_shaderFilePaths,
                const GraphicsPipelineConfigInfo& t_configInfo,
                uint32_t& t_materialId);

        /// Destroy a Vulkan pipeline object
        ~AeGraphicsPipeline();

        /// Do not allow this class to be copied
        AeGraphicsPipeline(const AeGraphicsPipeline&) = delete;
        AeGraphicsPipeline& operator=(const AeGraphicsPipeline&) = delete;

        /// Bind a command buffer to this pipeline.
        /// \param t_commandBuffer The command buffer to bind to this pipeline.
        void bind(VkCommandBuffer t_commandBuffer);

        /// Fills the specified configuration information reference with a default pipeline configuration.
        /// \param t_configInfo The pipeline configuration information variable that should be set to the default
        /// configuration.
        static void defaultPipelineConfigInfo(GraphicsPipelineConfigInfo& t_configInfo);

        /// Sets the pipeline configuration variables to enabled alpha blending.
        /// \param t_configInfo The configuration info object that requires alpha blending configuration variables to be
        /// set.
        static void enableAlphaBlending(GraphicsPipelineConfigInfo& t_configInfo);

    private:

        /// Reads a text file a vector variable.
        /// \param t_filepath The path to the file to be imported as a vector.
        /// \return A vector with the contents of the specified file.
        static std::vector<char> readFile(const std::string& t_filepath);

        /// Creates a Vulkan graphics pipeline.
        /// \param t_vertFilepath The relative path to the SPIR-V file which defines the pipeline's vertex shader.
        /// \param t_fragFilepath The relative path to the SPIR-V file which defines the pipeline's fragment shader.
        /// \param t_configInfo The GraphicsPipelineConfigInfo struct that defines the required vulkan properties of the
        /// pipeline to be created.
        void createGraphicsPipeline(
                const GraphicsShaderFilesPaths& t_shaderFilePaths,
                const GraphicsPipelineConfigInfo& t_configInfo,
                VkSpecializationInfo* spec_info);

        /// Function to create a Vulkan shader module from the supplied reference of code vector variable.
        /// \param t_code The vector containing the code for the shader module to be created.
        /// \param t_shaderModule A pointer to where the shader module should be created.
        void createShaderModule(const std::vector<char>& t_code, VkShaderModule* t_shaderModule);

        // Class variables
        /// The vulkan device this pipeline is implemented on
        AeDevice& m_aeDevice;

        /// This graphics pipeline
        VkPipeline m_graphicsPipeline;

    };
}  // namespace lve