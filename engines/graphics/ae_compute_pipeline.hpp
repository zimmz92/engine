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

    /// Class for the Vulkan pipeline object
    class AeComputePipeline {
    public:
        /// Create a Vulkan pipeline object.
        /// \param t_device The graphics device the pipeline will be implemented by.
        /// \param t_computeFilepath The filepath to the compute shader that will be used for this compute pipeline.
        AeComputePipeline( AeDevice& t_device,
                           VkPipelineLayout& t_pipelineLayout,
                           const std::string& t_computeFilepath);

        /// Destroy a Vulkan pipeline object
        ~AeComputePipeline();

        /// Do not allow this class to be copied
        AeComputePipeline(const AeComputePipeline&) = delete;
        AeComputePipeline& operator=(const AeComputePipeline&) = delete;

        /// Bind a command buffer to this pipeline.
        /// \param t_commandBuffer The command buffer to bind to this pipeline.
        void bind(VkCommandBuffer t_commandBuffer);

    private:

        /// Reads a text file a vector variable.
        /// \param t_filepath The path to the file to be imported as a vector.
        /// \return A vector with the contents of the specified file.
        static std::vector<char> readFile(const std::string& t_filepath);

        /// Creates a Vulkan graphics pipeline.
        /// \param t_computeFilepath The relative path to the SPIR-V file which defines the pipeline's compute shader.
        /// \param t_spec_info Any additional specialization information required to create the compute pipeline
        void createComputePipeline( const std::string& t_computeFilepath,
                                    VkPipelineLayout& t_pipelineLayout,
                                    VkSpecializationInfo* t_spec_info);

        /// Function to create a Vulkan shader module from the supplied reference of code vector variable.
        /// \param t_code The vector containing the code for the shader module to be created.
        /// \param t_shaderModule A pointer to where the shader module should be created.
        void createShaderModule(const std::vector<char>& t_code, VkShaderModule* t_shaderModule);

        // Class variables
        /// The vulkan device this pipeline is implemented on
        AeDevice& m_aeDevice;

        /// This graphics pipeline
        VkPipeline m_computePipeline;

    };
}  // namespace lve