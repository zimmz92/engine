#pragma once

#include "ae_device.hpp"

// libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace ae {
	class AeModel {
	public:
		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		AeModel(AeDevice &t_device, const AeModel::Builder &t_builder);
		~AeModel();

		// Do not allow this class to be copied (2 lines below)
		AeModel(const AeModel&) = delete;
		AeModel& operator=(const AeModel&) = delete;

		void bind(VkCommandBuffer t_commandBuffer);
		void draw(VkCommandBuffer t_commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex> &t_vertices);
		void createIndexBuffers(const std::vector<uint32_t>& t_indicies);

		AeDevice &m_aeDevice;

		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		uint32_t m_vertexCount;

		bool m_hasIndexBuffer = false;
		VkBuffer m_indexBuffer;
		VkDeviceMemory m_indexBufferMemory;
		uint32_t m_indexCount;
	};
} // namespace ae