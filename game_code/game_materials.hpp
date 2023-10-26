/*! \file GameComponents.hpp
    \brief The script that declares the components of the game.
    The game components are declared.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "ae_material3d.hpp"
#include "ae_image.hpp"

#include <memory>

namespace ae {

    class SimpleMaterial : public AeMaterial3D<std::shared_ptr<AeImage>>{
    public:
        SimpleMaterial(AeDevice &t_aeDevice,
                       VkRenderPass t_renderPass,
                       ae_ecs::AeECS& t_ecs,
                       MaterialShaderFiles& t_materialShaderFiles,
                       VkDescriptorSetLayout t_globalSetLayout,
                       VkDescriptorSetLayout t_textureSetLayout,
                       VkDescriptorSetLayout t_objectSetLayout):
                AeMaterial3D(t_aeDevice,
                             t_renderPass,
                             t_ecs,
                             t_materialShaderFiles,
                             t_globalSetLayout,
                             t_textureSetLayout,
                             t_objectSetLayout) {};
        ~SimpleMaterial()=default;
    };

    /// A structure to declare the materials available to the game.
    struct GameMaterials{

        GameMaterials(AeDevice &t_aeDevice,
                      VkRenderPass t_renderPass,
                      ae_ecs::AeECS& t_ecs,
                      VkDescriptorSetLayout t_globalSetLayout,
                      VkDescriptorSetLayout t_textureSetLayout,
                      VkDescriptorSetLayout t_objectSetLayout)
                : m_ecs{t_ecs},
                  m_device{t_aeDevice},
                  m_renderPass{t_renderPass},
                  m_globalSetLayout{t_globalSetLayout},
                  m_textureSetLayout{t_textureSetLayout},
                  m_objectSetLayout{t_objectSetLayout}{};

        ~GameMaterials(){};

        // Variables to easily declare new materials
        ae_ecs::AeECS& m_ecs;
        AeDevice& m_device;
        VkRenderPass m_renderPass;
        VkDescriptorSetLayout m_globalSetLayout;
        VkDescriptorSetLayout m_textureSetLayout;
        VkDescriptorSetLayout m_objectSetLayout;

        // Materials
        MaterialShaderFiles simpleMaterialShaderFiles = {"engines/graphics/shaders/shader.vert.spv",
                                                         "engines/graphics/shaders/shader.frag.spv",
                                                         "Not Used",
                                                         "Not Used"};
        SimpleMaterial m_simpleMaterial{m_device,
                                        m_renderPass,
                                        m_ecs,
                                        simpleMaterialShaderFiles,
                                        m_globalSetLayout,
                                        m_textureSetLayout,
                                        m_objectSetLayout};
    };
}