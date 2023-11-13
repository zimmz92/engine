/*! \file GameComponents.hpp
    \brief The script that declares the components of the game.
    The game components are declared.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "ae_material3d_layer.hpp"
#include "ae_image.hpp"

#include <memory>

namespace ae {

    class SimpleMaterial : public AeMaterial3DLayer<0,1,0,0>{
    public:
        SimpleMaterial(AeDevice &t_aeDevice,
                       GameComponents& t_game_components,
                       VkRenderPass t_renderPass,
                       ae_ecs::AeECS& t_ecs,
                       MaterialShaderFiles& t_materialShaderFiles,
                       std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts):
                AeMaterial3DLayer(t_aeDevice,
                                  t_game_components,
                                  t_renderPass,
                                  t_ecs,
                                  t_materialShaderFiles,
                                  t_descriptorSetLayouts) {};
        ~SimpleMaterial()=default;
    };

    /// A structure to declare the materials available to the game.
    struct GameMaterials{

        GameMaterials(AeDevice &t_aeDevice,
                      GameComponents& t_game_components,
                      VkRenderPass t_renderPass,
                      ae_ecs::AeECS& t_ecs,
                      std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts)
                : m_ecs{t_ecs},
                  m_device{t_aeDevice},
                  m_game_components{t_game_components},
                  m_renderPass{t_renderPass},
                  m_descriptorSetLayouts{t_descriptorSetLayouts}{

            // Add the simple material to the material list.
            m_materials.push_back(&m_simpleMaterial);
            m_materials.push_back(&m_newMaterial);

            // Check that the number of materials does not exceed the maximum number of allowed materials. The maximum
            // can be changed.
            // TODO Figure out how to dynamically create the texture index object... or some other way of indexing into
            //  the texture SSBO.
            if(m_materials.size()>MAX_MATERIALS){
                throw std::runtime_error("Too many materials are being created! Either this is an error or the maximum "
                                         "number of allowed materials needs to be increased!");
            }
        };

        ~GameMaterials()= default;

        // Variables to easily declare new materials
        ae_ecs::AeECS& m_ecs;
        GameComponents& m_game_components;
        AeDevice& m_device;
        VkRenderPass m_renderPass;
        std::vector<VkDescriptorSetLayout>& m_descriptorSetLayouts;
        std::vector<AeMaterial3DLayerBase*> m_materials;

        // Materials
        MaterialShaderFiles simpleMaterialShaderFiles = {"engines/graphics/shaders/simple.vert.spv",
                                                         "engines/graphics/shaders/simple.frag.spv",
                                                         "Not Used",
                                                         "Not Used"};
        SimpleMaterial m_simpleMaterial{m_device,
                                        m_game_components,
                                        m_renderPass,
                                        m_ecs,
                                        simpleMaterialShaderFiles,
                                        m_descriptorSetLayouts};

        SimpleMaterial m_newMaterial{m_device,
                                        m_game_components,
                                        m_renderPass,
                                        m_ecs,
                                        simpleMaterialShaderFiles,
                                        m_descriptorSetLayouts};
    };
}