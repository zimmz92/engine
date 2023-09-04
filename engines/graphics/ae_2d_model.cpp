/// \file ae_2d_model.cpp
/// \brief Implements the model for 2-D objects that will be rendered in the game.
/// Implements the model for 2-D game objects such as user interface elements or billboards.

#include "ae_2d_model.hpp"
#include "ae_utils.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std {
    // Define a hash operator for the Vertex structure to allow for a Vertex structure to be used as the key in an
    // unordered map. This is used to determine if the vertex is unique or a repeat used to make another triangle.
	template <>
	struct hash<ae::Ae2DModel::Vertex> {
		size_t operator()(ae::Ae2DModel::Vertex const& vertex) const {
			size_t seed = 0;
			ae::hashCombine(seed, vertex.position, vertex.color, vertex.uv);
			return seed;
		}
	};
}

namespace ae {

    // Constructor, makes a model compatible with the device using a builder that loaded the model's object file.
    Ae2DModel::Ae2DModel(AeDevice &t_device, const Ae2DModel::Builder& t_builder) : m_aeDevice{ t_device } {

        // TODO: Look into creating a memory allocator for vulkan: https://www.youtube.com/redirect?event=video_description&redir_token=QUFFLUhqbm9VSzZadjhDZF9pYnMxV2M4c09Cc25KTUNYQXxBQ3Jtc0ttQTFaTHpWNG1nb29jS3BDY2tSX1pUeUZ5R1RzRGF2ZVdkazA5NDRmUHo4S08wV0xXR0ZMRW55bnR6X0pJZ3REYmU2UlNNNVJWdW1oVnA4Um9aY0ZLZXU3U280QWdUWmY5czBpS3BkN3dHSHF6cGJZTQ&q=http%3A%2F%2Fkylehalladay.com%2Fblog%2Ftutorial%2F2017%2F12%2F13%2FCustom-Allocators-Vulkan.html&v=mnKp501RXDc
		// Create the vertex buffer using the vertices in the specified builder.
        createVertexBuffers(t_builder.vertices);

        // Create the index buffer using the indices created in the specified builder.
		createIndexBuffers(t_builder.indices);
	}



    // Destroy the model.
    Ae2DModel::~Ae2DModel() {}


    // Create the 2D model using the specified superset of vertices.
    std::unique_ptr<Ae2DModel> Ae2DModel::createModelFromFile(AeDevice& t_device, const std::vector<Vertex> &t_vertices) {
        // Create a new builder helper class to load the vertices and identify unique vertices.
        Builder builder{};
        builder.loadModel(t_vertices);

        // Create the model using the object that was loaded by the builder.
        return std::make_unique<Ae2DModel>(t_device, builder);
    }




    // Create the model's vertex buffer.
	void Ae2DModel::createVertexBuffers(const std::vector<Vertex> &t_vertices) {

        // Get the size of the vertex vector and ensure that we at least have one triangle.
		m_vertexCount = static_cast<uint32_t>(t_vertices.size());
		assert(m_vertexCount >= 3 && "Vertex count must be at least 3");

        // Get the required size of the buffer.
		VkDeviceSize bufferSize = sizeof(t_vertices[0]) * m_vertexCount;

        // Get the size of an individual vertex.
		uint32_t vertexSize = sizeof(t_vertices[0]);

        // Create a buffer to stage the model data from the host and it into the GPU.
        // Want the host (CPU) to be able to access the GPU's buffer memory and keep the GPU buffer memory consistent
        // with the host memory.
		AeBuffer stagingBuffer{
			m_aeDevice,
			vertexSize,
			m_vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

        // Map the memory required for the staging buffer.
		stagingBuffer.map();

        // Write the vertex data to the staging buffer.
		stagingBuffer.writeToBuffer((void*)t_vertices.data());

        // Create a buffer only on the GPU that will store the vertex data for this model until it is no longer
        // required.
		m_vertexBuffer = std::make_unique<AeBuffer>(
			m_aeDevice,
			vertexSize,
			m_vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // Copy the data from the staging buffer (host synced to GPU) to the vertex buffer (only on GPU)
		m_aeDevice.copyBuffer(stagingBuffer.getBuffer(), m_vertexBuffer->getBuffer(), bufferSize);

        // Remember staging buffer will be cleaned up here now that we're moving out of the staging buffer's scope.
	}



    // Create the model's index buffer.
	void Ae2DModel::createIndexBuffers(const std::vector<uint32_t>& t_indices) {
		m_indexCount = static_cast<uint32_t>(t_indices.size());
		m_hasIndexBuffer = m_indexCount > 0;

		if (!m_hasIndexBuffer) {
			return;
		}
		VkDeviceSize bufferSize = sizeof(t_indices[0]) * m_indexCount;
		uint32_t indexSize = sizeof(t_indices[0]);

		AeBuffer stagingBuffer{
			m_aeDevice,
			indexSize,
			m_indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)t_indices.data());

		m_indexBuffer = std::make_unique<AeBuffer>(
			m_aeDevice,
			indexSize,
			m_indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_aeDevice.copyBuffer(stagingBuffer.getBuffer(), m_indexBuffer->getBuffer(), bufferSize);
	}



    // Bind the vertex and index buffers to the specified command buffer to they are available to the command buffer
    // when executing draw commands for this model.
	void  Ae2DModel::bind(VkCommandBuffer t_commandBuffer) {
        // Create buffer and offset arrays to store the vertex buffer to make them compatible with the command buffer
        // bind command.
		VkBuffer buffers[] = {m_vertexBuffer->getBuffer()};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(t_commandBuffer, 0, 1, buffers, offsets);

		// TODO: UINT32 could be reduced if using more simple models
		if (m_hasIndexBuffer) {
            // Bind the index buffer to the command buffer if it is available.
			vkCmdBindIndexBuffer(t_commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}



    // Submit the draw command for the model to the specified command buffer.
	void  Ae2DModel::draw(VkCommandBuffer t_commandBuffer) {
		if (m_hasIndexBuffer) {
            // Draw using the indexed vertex buffer if available.
			vkCmdDrawIndexed(t_commandBuffer, m_indexCount, 1, 0, 0, 0);
		}
		else {
            // If just using a vertex buffer draw only using vertex buffer information.
			vkCmdDraw(t_commandBuffer, m_vertexCount, 1, 0, 0);
		}
		
	}



    // Define the vertex binding descriptions used to bind the vertex buffer type to the pipeline.
	std::vector<VkVertexInputBindingDescription> Ae2DModel::Vertex::getBindingDescriptions() {

		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

        // Define the general structure of the vertex buffer for binding.
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}



    // Define the vertex attribute descriptions used to bind the vertex buffer type to the pipeline.
	std::vector<VkVertexInputAttributeDescription> Ae2DModel::Vertex::getAttributeDescriptions() {

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        // Describe the data composition of each vertex data set input.
		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32_SFLOAT , offsetof(Vertex, position)});
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT , offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32_SFLOAT , offsetof(Vertex, uv) });

		return attributeDescriptions;
	}



    // Load the 2-D model's vertices and indices for the object specified by its superset of vertices.
    void Ae2DModel::Builder::loadModel(const std::vector<Vertex> &t_vertices) {

        // Assume that the input vertex data is unoptimized and intended to be used with an index buffer.
        uint32_t vertexCount = static_cast<uint32_t>(t_vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");

        // Clear the structs data to ensure only the new object data is loaded.
		vertices.clear();
		indices.clear();

        // Create an unordered map that uses the Vertex struct as it's key to allow its hash to be used to identify if
        // the current vertex is unique or already exists in the map. This allows the index of non-unique vertices to
        // point to the already existing vertex data instead of replicating the data.
		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        // Loop through all the input vertex data.
        for (const auto &in_vertex : t_vertices) {

            // Use the hashing function of the unordered map to identify if the vertex is unique.
            if (uniqueVertices.count(in_vertex) == 0) {
                // Add the unique vertex to the unique vertices unordered map.
                uniqueVertices[in_vertex] = static_cast<uint32_t>(vertices.size());
                // Add the vertex data to the builder's vertices data that will be used to make the model's vertex
                // buffer later.
                vertices.push_back(in_vertex);
            }

            // Specify the index for the vertex data that was created from the imported object file data. The hash
            // of the vertex struct will ensure matching vertex data will get the same index.s
            indices.push_back(uniqueVertices[in_vertex]);
        }
	}

} //namespace ae
