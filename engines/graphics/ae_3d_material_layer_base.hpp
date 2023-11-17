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

    /// Specifies the material shader files that the material layer utilizes in it's pipeline.
    struct MaterialShaderFiles{
        /// The filepath of the vertex shader. By default this is set to "Not Used" to indicate to pipeline creation
        /// that there is no vertex shader specified for it.
        std::string vertexShaderFilepath = "Not Used";

        /// The filepath of the fragment shader. By default this is set to "Not Used" to indicate to pipeline creation
        /// that there is no fragment shader specified for it.
        std::string fragmentShaderFilepath = "Not Used";

        /// The filepath of the vertex shader. By default this is set to "Not Used" to indicate to pipeline creation
        /// that there is no tessellation shader specified for it.
        std::string tessellationShaderFilepath =  "Not Used";

        /// The filepath of the vertex shader. By default this is set to "Not Used" to indicate to pipeline creation
        /// that there is no geometry shader specified for it.
        std::string geometryShaderFilepath =  "Not Used";
    };

    class Ae3DMaterialLayerBase{
    public:
        /// Constructs the graphics pipeline for the 3D material layer given the graphics device, render pass, shader
        /// files, and descriptor sets the material layer will be interfacing with.
        /// \param t_aeDevice The device that the material layer pipeline will be implemented on.
        /// \param t_renderPass The render pass that will utilize the material layer.
        /// \param t_materialShaderFiles The structure that specifies the shader files that outline the configuration of
        /// the dynamic pipeline stages.
        /// \param t_descriptorSetLayouts The descriptor sets that the pipeline, and the pipeline shaders, will be
        /// utilizing in their operation.
        explicit Ae3DMaterialLayerBase(AeDevice &t_aeDevice,
                                       VkRenderPass t_renderPass,
                                       MaterialShaderFiles& t_materialShaderFiles,
                                       std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts);


        /// Destructor of the AeMaterial3DLayerBase.
        ~Ae3DMaterialLayerBase();


        /// Loop through the unique models that entities that utilize this material layer have and use
        /// drawIndexedIndirect to render all entities that use a unique model using a single drawIndexedIndirect
        /// call.
        /// \param t_commandBuffer The command buffer where the drawIndexedIndirect commands will be recorded to.
        /// \param t_drawIndexedIndirectBuffer The buffer that stores the drawIndexedIndirect commands.
        /// \param t_descriptorSets The descriptor sets that the shaders of this material layer need access to for
        /// rendering their entities.
        virtual void executeSystem(VkCommandBuffer& t_commandBuffer,
                                   VkBuffer t_drawIndexedIndirectBuffer,
                                   std::vector<VkDescriptorSet>& t_descriptorSets){};



        /// Updates the material layer entity data trackers and the image buffer for entity information that has
        /// updated and/or entities that have been removed or had this material layer removed.
        /// \param t_entity3DSSBOData The model matrix and texture indices data for an entity that is in the 3D-SSBO
        /// that will be accessed by the material layer shaders when rendering.
        /// \param t_entity3DSSBOMap The map of where entities data are within the 3D-SSBO.
        /// \param t_imageBuffer The buffer that stores the images and their samplers that will be accessed by the
        /// material layer shaders when rendering.
        /// \param t_imageBufferMap The map of images and their samplers and which entities use them with which
        /// materials.
        /// \param t_imageBufferStack The stack of available positions within the imageBufferMap where new images
        /// can be placed. When no entities utilize a specific image the image's position within the imageBufferMap
        /// is placed at the top of this stack.
        /// \param t_drawIndexedIndirectCommandBufferIndex The position within the drawIndexedIndirectCommandBuffer
        /// that the drawIndexedIndirectCommands required for this material layer will start. This position will be
        /// required when executing the drawIndexedIndirect commands for this material.
        virtual const std::vector<VkDrawIndexedIndirectCommand>& updateMaterialLayerEntities(std::vector<Entity3DSSBOData>& t_entity3DSSBOData,
                                                                                             std::map<ecs_id, uint32_t>& t_entity3DSSBOMap,
                                                                                             VkDescriptorImageInfo t_imageBuffer[MAX_TEXTURES],
                                                                                             std::map<std::shared_ptr<AeImage>,ImageBufferInfo>& t_imageBufferMap,
                                                                                             PreAllocatedStack<uint64_t,MAX_TEXTURES>& t_imageBufferStack,
                                                                                             uint64_t t_drawIndexedIndirectCommandBufferIndex)=0;


        /// Performs any required clean up of the material layer system that is required after it's execution.
        virtual void cleanupSystem()=0;


        /// Returns the ID of the ECS component that this material layer uses to track entity specific information
        /// required for this material layer.
        /// \return The ID of the material layer component.
        virtual ecs_id getMaterialLayerComponentId()=0;


        /// Returns the material layer ID for this material layer.
        /// \return The material layer ID for this material layer.
        [[nodiscard]] material_id getMaterialLayerId() const;


        /// Binds the pipeline to the specified command buffer so commands submitted after this are associated with this
        /// material layer's pipeline.
        /// \param t_commandBuffer The command buffer this pipeline should be bound to.
        void bindPipeline(VkCommandBuffer& t_commandBuffer);


        /// Returns the pipeline layout for this material layer.
        /// \return A reference to the VkPipelineLayout of this material layer.
        VkPipelineLayout& getPipelineLayout();

    private:

        /// Creates the pipeline layout for the material.
        /// \param t_globalSetLayout The general descriptor set for the devices and general rendering setting that need
        /// to be accounted for when setting up the render pipeline for this system.
        void createPipelineLayout(std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts);


        /// Creates the pipeline based on the render pass this pipeline should be associated with for the
        /// material layer.
        /// \param t_renderPass The render pass this pipeline should be associated with.
        void createPipeline(VkRenderPass t_renderPass);


        /// The graphics device this render system will be using.
        AeDevice& m_aeDevice;

        /// The name of the material.
        std::string m_materialName="JON-DOE";

        /// The layout of this render system's pipeline.
        VkPipelineLayout m_pipelineLayout;

        /// The pipeline created for this render system.
        std::unique_ptr<AePipeline> m_pipeline;

        /// A structure specifying the shader files this material uses.
        MaterialShaderFiles m_materialShaderFiles;

        /// The ID of the material
        material_id m_materialID;

    protected:

    };

} // namespace ae