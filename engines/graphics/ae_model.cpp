#include "ae_model.hpp"

#include <cassert>
#include <cstring>

namespace ae {

	AeModel::AeModel(AeDevice &t_device, const std::vector<Vertex> &t_vertices) : m_aeDevice{ t_device } {
		createVertexBuffers(t_vertices);
	}

	AeModel::~AeModel() {
		vkDestroyBuffer(m_aeDevice.device(), m_vertexBuffer, nullptr);
		vkFreeMemory(m_aeDevice.device(), m_vertexBufferMemory, nullptr);
	}

	void AeModel::createVertexBuffers(const std::vector<Vertex> &t_vertices) {
		m_vertexCount = static_cast<uint32_t>(t_vertices.size());
		assert(m_vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(t_vertices[0]) * m_vertexCount;
		m_aeDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_vertexBuffer,
			m_vertexBufferMemory);

		void *data;
		vkMapMemory(m_aeDevice.device(), m_vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, t_vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_aeDevice.device(), m_vertexBufferMemory);
	}

	void  AeModel::bind(VkCommandBuffer t_commandBuffer) {
		VkBuffer buffers[] = {m_vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(t_commandBuffer, 0, 1, buffers, offsets);
	}

	void  AeModel::draw(VkCommandBuffer t_commandBuffer) {
		vkCmdDraw(t_commandBuffer, m_vertexCount, 1, 0, 0);
	}

	std::vector<VkVertexInputBindingDescription> AeModel::Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> AeModel::Vertex::getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}

} //namespace ae
