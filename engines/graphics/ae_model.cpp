#include "ae_model.hpp"
#include "ae_utils.hpp"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std {
	template <>
	struct hash<ae::AeModel::Vertex> {
		size_t operator()(ae::AeModel::Vertex const& vertex) const {
			size_t seed = 0;
			ae::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

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

	std::unique_ptr<AeModel> AeModel::createModelFromFile(AeDevice& t_device, const std::string& t_filepath) {
		Builder builder{};
		builder.loadModel(t_filepath);
		return std::make_unique<AeModel>(t_device, builder);
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
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT , offsetof(Vertex, position)});
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT , offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT , offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT , offsetof(Vertex, uv) });

		return attributeDescriptions;
	}


	void AeModel::Builder::loadModel(const std::string& t_filepath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, t_filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto &shape : shapes) {
			for (const auto &index : shape.mesh.indices) {
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = { 
						attrib.vertices[3 * index.vertex_index + 0], 
						attrib.vertices[3 * index.vertex_index + 1], 
						attrib.vertices[3 * index.vertex_index + 2],
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};					
				}

				if (index.normal_index >= 0) {
					vertex.normal = { 
						attrib.normals[3 * index.normal_index + 0], 
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = { 
						attrib.texcoords[2 * index.texcoord_index + 0], 
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

} //namespace ae
