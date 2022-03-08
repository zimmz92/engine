#include "Arundos.hpp"

// libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace ae {

    struct SimplePushConstantData {
        glm::mat2 transform{ 1.0f };
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    Arundos::Arundos() {
        loadGameObjects();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    Arundos::~Arundos() {
        vkDestroyPipelineLayout(m_aeDevice.device(), m_pipelineLayout, nullptr);
    }

    void Arundos::run() {
        while (!m_aeWindow.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(m_aeDevice.device());
    }

    void Arundos::loadGameObjects() {
        std::vector<AeModel::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };
        auto aeModel = std::make_shared<AeModel>(m_aeDevice, vertices);

        auto triangle = AeGameObject::createGameObject();
        triangle.m_model = aeModel;
        triangle.m_color = { 0.1f, 0.8f, 0.1f };
        triangle.m_transform2d.translation.x = 0.2f;
        triangle.m_transform2d.scale = {2.0f, 0.5f};
        triangle.m_transform2d.rotation = 0.25f * glm::two_pi<float>(); // vulkan has -x is left on screen -y is up on screen

        m_gameObjects.push_back(std::move(triangle));
    }

    void Arundos::createPipelineLayout() {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Send a small amount of data to shader program
        if (vkCreatePipelineLayout(m_aeDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

    };
    void Arundos::createPipeline() {
        assert(m_aeSwapChain != nullptr && "Cannot create pipeline before swap chain!");
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

        PipelineConfigInfo pipelineConfig{};
        AePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = m_aeSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_aePipeline = std::make_unique<AePipeline>(
            m_aeDevice,
            "engines/graphics/shaders/vert.spv",
            "engines/graphics/shaders/frag.spv",
            pipelineConfig);
    };

    void Arundos::recreateSwapChain() {
        auto extent = m_aeWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = m_aeWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_aeDevice.device());

        if (m_aeSwapChain == nullptr) {
            m_aeSwapChain = std::make_unique<AeSwapChain>(m_aeDevice, extent);
        }
        else {
            m_aeSwapChain = std::make_unique<AeSwapChain>(m_aeDevice, extent, std::move(m_aeSwapChain));
            if (m_aeSwapChain->imageCount() != m_commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        // TODO: If render pass compatible remove following line
        createPipeline();
    };

    void Arundos::createCommandBuffers() {
        m_commandBuffers.resize(m_aeSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_aeDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        if (vkAllocateCommandBuffers(m_aeDevice.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    };

    void Arundos::freeCommandBuffers() {
        vkFreeCommandBuffers(m_aeDevice.device(), m_aeDevice.getCommandPool(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
        m_commandBuffers.clear();
    }

    void Arundos::recordCommandBuffer(int t_imageIndex) {

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(m_commandBuffers[t_imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_aeSwapChain->getRenderPass();
        renderPassInfo.framebuffer = m_aeSwapChain->getFrameBuffer(t_imageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_aeSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_commandBuffers[t_imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_aeSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(m_aeSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_aeSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(m_commandBuffers[t_imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(m_commandBuffers[t_imageIndex], 0, 1, &scissor);

        renderGameObjects(m_commandBuffers[t_imageIndex]);

        vkCmdEndRenderPass(m_commandBuffers[t_imageIndex]);
        if (vkEndCommandBuffer(m_commandBuffers[t_imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer.");
        }
    };

    void Arundos::renderGameObjects(VkCommandBuffer t_commandBuffer) {
        m_aePipeline->bind(t_commandBuffer);

        for (auto& obj : m_gameObjects) {
            SimplePushConstantData push{};
            push.offset = obj.m_transform2d.translation;
            push.color = obj.m_color;
            push.transform = obj.m_transform2d.mat2();

            vkCmdPushConstants(t_commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

            obj.m_model->bind(t_commandBuffer);
            obj.m_model->draw(t_commandBuffer);
        }
    }

    void Arundos::drawFrame() {
        uint32_t imageIndex;
        auto result = m_aeSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to aquire swap chain image!");
        }

        recordCommandBuffer(imageIndex);
        result = m_aeSwapChain->submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_aeWindow.wasWindowResized()) {
            m_aeWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image!");
        }

    };

}  // namespace ae