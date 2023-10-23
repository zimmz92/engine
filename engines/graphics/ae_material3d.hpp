/// \file ae_material3d.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "game_components.hpp"
#include "ae_ecs_include.hpp"
#include "ae_image.hpp"
#include "ae_pipeline.hpp"

// libraries

//std
#include <map>


namespace ae {

    struct Model3DMaterialProperties{
        // Number of textures provided will be checked at runtime and error will be thrown if not enough are given.
        std::vector<std::shared_ptr<AeImage>> vertexShaderTextures;
        std::vector<std::shared_ptr<AeImage>> fragmentShaderTextures;
        std::vector<std::shared_ptr<AeImage>> tessellationShaderTextures;
        std::vector<std::shared_ptr<AeImage>> geometryShaderTextures;
    };

    class AeMaterial3D : public ae_ecs::AeSystem<AeMaterial3D>{

    public:

        struct MaterialShaderFiles{
            std::string vertexShaderFilepath = "Not Used";
            std::int8_t numVertexShaderTextures = 0;

            std::string fragmentShaderFilepath = "Not Used";
            std::int8_t numFragmentShaderTextures = 0;

            std::string tessellationShaderFilepath =  "Not Used";
            std::int8_t numTesselationShaderTextures = 0;

            std::string geometryShaderFilepath =  "Not Used";
            std::int8_t numGeometryShaderTextures = 0;
        };

        /// Constructor of the SimpleRenderSystem
        /// \param t_game_components The game components available that this system may require.
        AeMaterial3D(ae_ecs::AeECS& t_ecs,
                     AeDevice &t_aeDevice,
                     VkRenderPass t_renderPass,
                     GameComponents& t_gameComponents,
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

        /// Registers that an entity uses this material.
        void registerEntity(ecs_id t_entityId, Model3DMaterialProperties& t_entityMaterialProperties);

        /// Informs the material that an entity that was registered with the material no longer does.
        void deregisterEntity(ecs_id t_entityId, std::shared_ptr<AeModel> t_model3D);

    private:

        ModelComponent& m_modelComponent;

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

        /// A vector to track unique models, and a list of which entities use them.
        std::map<std::shared_ptr<AeModel>,std::map<ecs_id,uint64_t>> m_uniqueModelMap;

        /// Compiles the commands to be called by draw indexed indirect command for each frames.
        VkDrawIndexedIndirectCommand materialDrawIndexedCommands[MAX_FRAMES_IN_FLIGHT][MAX_OBJECTS];

    protected:

    };

} // namespace ae