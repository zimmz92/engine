/// \file ae_material3d.hpp
/// The ## class is defined.
#pragma once

// dependencies
#include "ae_ecs_include.hpp"
#include "ae_pipeline.hpp"
#include "ae_model.hpp"
#include "ae_material3d_base.hpp"

// libraries

//std
#include <map>
#include <memory>


namespace ae {

    template <typename T>
    class AeMaterial3D : public AeMaterial3DBase{

    public:

        /// This component is used to allow entities to store specific information required for this material to
        /// correctly function.
        class MaterialComponent : public ae_ecs::AeComponent<T>{
        public:
            /// The MaterialComponent constructor uses the AeComponent constructor with no additions.
            explicit MaterialComponent(ae_ecs::AeECS& t_ecs, ecs_id t_materialSystemId) : ae_ecs::AeComponent<T>(t_ecs) {

                // Link the material component of this material with the system for the material.
                this->requiredBySystem(t_materialSystemId);
            };

            /// The destructor of the MaterialComponent class. The MaterialComponent destructor
            /// uses the AeComponent constructor with no additions.
            ~MaterialComponent() = default;
        };


        /// This system is used to organized all the models and entity SSBO index data that use this material.
        class MaterialSystem : public ae_ecs::AeSystem<MaterialSystem>{
        public:
            // Constructor implementation
            MaterialSystem(ae_ecs::AeECS& t_ecs) : ae_ecs::AeSystem<MaterialSystem>(t_ecs) {
                // There will be a call in the renderer system to all the material systems.
                this->isChildSystem = true;
            };

            // Destructor implementation
            ~MaterialSystem() = default;

            // Set up the system prior to execution. Currently not used.
            void setupSystem() {};

            // Update the time difference between the current execution and the previous.
            void executeSystem() {};

            // Clean up the system after execution. Currently not used.
            void cleanupSystem() {
                this->m_systemManager.clearSystemEntityUpdateSignatures(this->getSystemId());
            };
        };

        /// Constructor of the SimpleRenderSystem
        /// \param t_game_components The game components available that this system may require.
        explicit AeMaterial3D(AeDevice &t_aeDevice,
                              VkRenderPass t_renderPass,
                              ae_ecs::AeECS& t_ecs,
                              MaterialShaderFiles& t_materialShaderFiles,
                              VkDescriptorSetLayout t_globalSetLayout,
                              VkDescriptorSetLayout t_textureSetLayout,
                              VkDescriptorSetLayout t_objectSetLayout) :
                m_ecs{t_ecs},
                AeMaterial3DBase(t_aeDevice,
                                 t_renderPass,
                                 t_materialShaderFiles,
                                 t_globalSetLayout,
                                 t_textureSetLayout,
                                 t_objectSetLayout) {};

        /// Destructor of the SimpleRenderSystem
        ~AeMaterial3D()= default;

    private:

        /// Reference to the ECS used for the component and the system associated with this material.
        ae_ecs::AeECS& m_ecs;

        /// A vector to track unique models, and a list of which entities use them.
        std::map<std::shared_ptr<AeModel>,std::map<ecs_id,uint64_t>> m_uniqueModelMap;

        /// Compiles the commands to be called by draw indexed indirect command for each frames.
        VkDrawIndexedIndirectCommand materialDrawIndexedCommands[MAX_FRAMES_IN_FLIGHT][MAX_OBJECTS];

        /// Create a system to deal with organizing the models and entity information the material is responsible for
        /// rendering.
        MaterialSystem m_materialSystem{m_ecs};

        /// Create a component for the material to track entities that use the material and specify
        /// textures/properties specific to that entity.
        MaterialComponent m_materialComponent{m_ecs,m_materialSystem.getSystemId()};

    protected:

    };

} // namespace ae