/// \file ae_material3d.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_pipeline.hpp"
#include "ae_model.hpp"
#include "ae_graphics_constants.hpp"
#include "ae_image.hpp"
#include "pre_allocated_stack.hpp"

// libraries

//std
#include <map>
#include <memory>


namespace ae {

    struct MaterialShaderFiles{
        std::string vertexShaderFilepath = "Not Used";
        std::string fragmentShaderFilepath = "Not Used";
        std::string tessellationShaderFilepath =  "Not Used";
        std::string geometryShaderFilepath =  "Not Used";
    };

    class AeMaterial3DBase{
    public:
        /// Constructor of the SimpleRenderSystem
        /// \param t_game_components The game components available that this system may require.
        explicit AeMaterial3DBase(AeDevice &t_aeDevice,
                     VkRenderPass t_renderPass,
                     MaterialShaderFiles& t_materialShaderFiles,
                     VkDescriptorSetLayout t_globalSetLayout,
                     VkDescriptorSetLayout t_textureSetLayout,
                     VkDescriptorSetLayout t_objectSetLayout);

        /// Destructor of the SimpleRenderSystem
        ~AeMaterial3DBase();

        virtual void setupSystem(int t_frameIndex){};
        virtual void executeMaterialSystem(std::vector<Entity3DSSBOData>& t_entity3DSSBOData,
                                           std::map<ecs_id, uint32_t>& t_entity3DSSBOMap,
                                           VkDescriptorImageInfo t_imageBuffer[MAX_TEXTURES],
                                           std::map<std::shared_ptr<AeImage>,ImageBufferInfo>& t_imageBufferMap,
                                           PreAllocatedStack<uint64_t,MAX_TEXTURES>& t_imageBufferStack) {};
        virtual void cleanupSystem(){};
        virtual ecs_id getComponentId(){return 0;};

        material_id getMaterialId();

    private:

        /// Creates the pipeline layout for the material.
        /// \param t_globalSetLayout The general descriptor set for the devices and general rendering setting that need
        /// to be accounted for when setting up the render pipeline for this system.
        void createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout,
                                  VkDescriptorSetLayout t_textureSetLayout,
                                  VkDescriptorSetLayout t_objectSetLayout);

        /// Creates the pipeline based on the render pass this pipeline should be associated with for the
        /// SimpleRenderSystem.
        /// \param t_renderPass The render pass this pipeline should be associated with.
        void createPipeline(VkRenderPass t_renderPass);

        /// The graphics device this render system will be using.
        AeDevice& m_aeDevice;

        /// The layout of this render system's pipeline.
        VkPipelineLayout m_pipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AePipeline> m_aePipeline;

        /// A structure specifying the shader files this material uses.
        MaterialShaderFiles m_materialShaderFiles;

        /// The ID of the material
        material_id m_materialID;

    protected:

    };

} // namespace ae