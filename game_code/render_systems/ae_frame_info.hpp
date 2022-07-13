#pragma once

#include "ae_camera.hpp"
#include "ae_game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace ae {
	struct FrameInfo {
		int m_frameIndex;
		float m_frameTime;
		VkCommandBuffer m_commandBuffer;
		AeCamera& m_camera;
		VkDescriptorSet m_globalDescriptorSet;
		AeGameObject::Map &gameObjects;
	};
} // namespace ae