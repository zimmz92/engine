#pragma once

#include "ae_camera.hpp"
#include "ae_game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace ae {

	#define MAX_LIGHTS 10

	struct PointLight {
		glm::vec4 position{}; // ignore w
		glm::vec4 color{}; // w is intensity
	};

	struct GlobalUbo {
		// Remember alignment always either use alignment or use only 4d!!!
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f };
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo {
		int m_frameIndex;
		float m_frameTime;
		VkCommandBuffer m_commandBuffer;
		AeCamera& m_camera;
		VkDescriptorSet m_globalDescriptorSet;
		AeGameObject::Map &gameObjects;
	};
} // namespace ae