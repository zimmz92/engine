#include "ae_rs_point_light.hpp"

// libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <cassert>
#include <stdexcept>
#include <array>
#include <map>

namespace ae {

    struct PointLightPushConstants {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

    AeRsPointLight::AeRsPointLight(AeDevice& t_device, VkRenderPass t_renderPass, VkDescriptorSetLayout t_globalSetLayout) : m_aeDevice{ t_device } {
        createPipelineLayout(t_globalSetLayout);
        createPipeline(t_renderPass);
    }

    AeRsPointLight::~AeRsPointLight() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    }

    void AeRsPointLight::createPipelineLayout(VkDescriptorSetLayout t_globalSetLayout) {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstants);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ t_globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Send a small amount of data to shader program
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

    };

    void AeRsPointLight::createPipeline(VkRenderPass t_renderPass) {
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

        PipelineConfigInfo pipelineConfig{};
        AePipeline::defaultPipelineConfigInfo(pipelineConfig);
        AePipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.renderPass = t_renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_aePipeline = std::make_unique<AePipeline>(
            m_aeDevice,
            "engines/graphics/shaders/point_light_vert.spv",
            "engines/graphics/shaders/point_light_frag.spv",
            pipelineConfig);
    };

    void AeRsPointLight::update(FrameInfo& t_frameInfo, GlobalUbo& t_ubo) {
        auto rotateLight = glm::rotate(
            glm::mat4(1.0f),
            t_frameInfo.m_frameTime,
            { 0.0f, -1.0f, 0.0f });
        int lightIndex = 0;
        for (auto& kv : t_frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.m_pointLight == nullptr) continue;

            assert(lightIndex < MAX_LIGHTS && "Number of point lights exceed MAX_LIGHTS!");

            // update light position
            obj.m_transform.translation = glm::vec3(rotateLight * glm::vec4(obj.m_transform.translation, 1.0f));

            //copy light to ubo
            t_ubo.pointLights[lightIndex].position = glm::vec4(obj.m_transform.translation, 1.0f);
            t_ubo.pointLights[lightIndex].color = glm::vec4(obj.m_color, obj.m_pointLight->lightIntensity);
            lightIndex += 1;
        }
        t_ubo.numLights = lightIndex;
    }

    void AeRsPointLight::render(FrameInfo& t_frameInfo) {
        //sort lights
        std::map<float, AeGameObject::id_t> sorted;
        for (auto& kv : t_frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.m_pointLight == nullptr) continue;

            //calculate distance
            auto offset = t_frameInfo.m_camera.getPosition() - obj.m_transform.translation;
            float disSquared = glm::dot(offset, offset);
            sorted[disSquared] = obj.getID();
        }

        m_aePipeline->bind(t_frameInfo.m_commandBuffer);

        vkCmdBindDescriptorSets(
            t_frameInfo.m_commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipelineLayout,
            0,
            1,
            &t_frameInfo.m_globalDescriptorSet,
            0,
            nullptr);

        // iterate through sorted lights in reverse order
        for (auto& it = sorted.rbegin(); it != sorted.rend(); ++it) {
            // use game obj id to find light object
            auto& obj = t_frameInfo.gameObjects.at(it->second);

            PointLightPushConstants push{};
            push.position = glm::vec4(obj.m_transform.translation, 1.0f);
            push.color = glm::vec4(obj.m_color, obj.m_pointLight->lightIntensity);
            push.radius = obj.m_transform.scale.x;

            vkCmdPushConstants(
                t_frameInfo.m_commandBuffer,
                m_pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PointLightPushConstants),
                &push);

            vkCmdDraw(t_frameInfo.m_commandBuffer, 6, 1, 0, 0);
        } 
    }
}  // namespace ae