/// \file ae_material3d.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_ecs_include.hpp"
#include "ae_image.hpp"
#include "ae_pipeline.hpp"
#include "game_components.hpp"

// libraries

//std
#include <map>


namespace ae {

    /// The typename T is intended to be a structure defining anything what the specific material requires of an entity
    /// and need to be specified when an entity registers with the material.
    template <typename T>
    class AeMaterial3D : public ae_ecs::AeSystem<AeMaterial3D<T>>{

    public:

        struct Model3DMaterial{
            AeMaterial3D& material3D;
            T entityMaterialProperties;
        };

        struct MaterialShaderFiles{
            std::string vertexShaderFilepath = "Not Used";
            std::string fragmentShaderFilepath = "Not Used";
            std::string tessellationShaderFilepath =  "Not Used";
            std::string geometryShaderFilepath =  "Not Used";
        };

        /// Constructor of the SimpleRenderSystem
        /// \param t_game_components The game components available that this system may require.
        AeMaterial3D(ae_ecs::AeECS& t_ecs,
                     AeDevice& t_aeDevice,
                     VkRenderPass t_renderPass,
                     MaterialShaderFiles t_materialShaderFiles,
                     VkDescriptorSetLayout t_globalSetLayout,
                     VkDescriptorSetLayout t_textureSetLayout,
                     VkDescriptorSetLayout t_objectSetLayout);

        /// Destructor of the SimpleRenderSystem
        ~AeMaterial3D();

        /// Setup the SimpleRenderSystem, this is handled by the RendererSystem.
        void setupSystem() override;

        /// Execute the SimpleRenderSystem, this is handled by the RendererSystem.
        void executeSystem() override;

        /// Clean up the SimpleRenderSystem, this is handled by the RendererSystem.
        void cleanupSystem() override;

        void registerEntity(ecs_id t_entityId, AeModel& t_model3D, T t_entityMaterialProperties);

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

        // A structure specifying the shaders this material uses.
        MaterialShaderFiles m_materialShaderFiles;

        /// A vector to track unique models, and a list of which entities use them.
        std::map<AeModel,std::map<ecs_id,uint64_t>> m_uniqueModelMap;

        /// Compiles the commands to be called by draw indexed indirect command for each frames.
        VkDrawIndexedIndirectCommand materialDrawIndexedCommands[MAX_FRAMES_IN_FLIGHT][MAX_OBJECTS];

    protected:

    };

} // namespace ae