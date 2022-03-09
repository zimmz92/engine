#include "ae_model.hpp"

#include <cassert>
#include <cstring>

namespace ae {

	AeModel::AeModel(AeDevice &t_device, const AeModel::Builder& t_builder) : m_aeDevice{ t_device } {
		createVertexBuffers(t_builder.vertices);
		createIndexBuffers(t_builder.indices);
	}

	AeModel::~AeModel() {
		vkDestroyBuffer(m_aeDevice.device(), m_vertexBuffer, nullptr);
		vkFreeMemory(m_aeDevice.device(), m_vertexBufferMemory, nullptr);

		if (m_hasIndexBuffer) {
			vkDestroyBuffer(m_aeDevice.device(), m_indexBuffer, nullptr);
			vkFreeMemory(m_aeDevice.device(), m_indexBufferMemory, nullptr);
		}
		
	}

	void AeModel::createVertexBuffers(const std::vector<Vertex> &t_vertices) {
		m_vertexCount = static_cast<uint32_t>(t_vertices.size());
		assert(m_vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(t_vertices[0]) * m_vertexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		m_aeDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void *data;
		vkMapMemory(m_aeDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, t_vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_aeDevice.device(), stagingBufferMemory);

		m_aeDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_vertexBuffer,
			m_vertexBufferMemory);

		m_aeDevice.copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

		vkDestroyBuffer(m_aeDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(m_aeDevice.device(), stagingBufferMemory, nullptr);
	}

	void AeModel::createIndexBuffers(const std::vector<uint32_t>& t_indicies) {
		m_indexCount = static_cast<uint32_t>(t_indicies.size());
		m_hasIndexBuffer = m_indexCount > 0;

		if (!m_hasIndexBuffer) {
			return;
		}
		VkDeviceSize bufferSize = sizeof(t_indicies[0]) * m_indexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		m_aeDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		vkMapMemory(m_aeDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, t_indicies.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_aeDevice.device(), stagingBufferMemory);

		m_aeDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_indexBuffer,
			m_indexBufferMemory);

		m_aeDevice.copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

		vkDestroyBuffer(m_aeDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(m_aeDevice.device(), stagingBufferMemory, nullptr);
	}

	void  AeModel::bind(VkCommandBuffer t_commandBuffer) {
		VkBuffer buffers[] = {m_vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(t_commandBuffer, 0, 1, buffers, offsets);

		// TODO: UINT32 could be reduced if using more simple models
		if (m_hasIndexBuffer) {
			vkCmdBindIndexBuffer(t_commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void  AeModel::draw(VkCommandBuffer t_commandBuffer) {
		if (m_hasIndexBuffer) {
			vkCmdDrawIndexed(t_commandBuffer, m_indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(t_commandBuffer, m_vertexCount, 1, 0, 0);
		}
		
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
