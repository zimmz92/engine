#include "ae_3d_model.hpp"
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
    // Define a hash operator for the Vertex structure to allow for a Vertex structure to be used as the key in an
    // unordered map. This is used to determine if the vertex is unique or a repeat used to make another triangle.
	template <>
	struct hash<ae::Ae3DModel::Vertex> {
		size_t operator()(ae::Ae3DModel::Vertex const& vertex) const {
			size_t seed = 0;
			ae::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace ae {

    // Constructor, makes a model compatible with the device using a builder that loaded the model's object file.
	Ae3DModel::Ae3DModel(AeDevice &t_device, const Ae3DModel::Builder& t_builder, const ssbo_idx t_idxObbSsbo) : m_aeDevice{t_device } {

        // TODO: Look into creating a memory allocator for vulkan: https://www.youtube.com/redirect?event=video_description&redir_token=QUFFLUhqbm9VSzZadjhDZF9pYnMxV2M4c09Cc25KTUNYQXxBQ3Jtc0ttQTFaTHpWNG1nb29jS3BDY2tSX1pUeUZ5R1RzRGF2ZVdkazA5NDRmUHo4S08wV0xXR0ZMRW55bnR6X0pJZ3REYmU2UlNNNVJWdW1oVnA4Um9aY0ZLZXU3U280QWdUWmY5czBpS3BkN3dHSHF6cGJZTQ&q=http%3A%2F%2Fkylehalladay.com%2Fblog%2Ftutorial%2F2017%2F12%2F13%2FCustom-Allocators-Vulkan.html&v=mnKp501RXDc
		// Create the vertex buffer using the vertices in the specified builder.
        createVertexBuffer(t_builder.vertices);

        // Create a vertex buffer for the model's OBB.
        m_obb = t_builder.obb;
        m_idxObbSsbo = t_idxObbSsbo;
        createObbVertexBuffer(t_builder.obb);

        // Create the index buffer using the indices created in the specified builder.
        createIndexBuffer(t_builder.indices);
	}



    // Destroy the model.
	Ae3DModel::~Ae3DModel() {}



    // Create the model using the object file at the specified path.
	std::unique_ptr<Ae3DModel> Ae3DModel::createModelFromFile(AeDevice& t_device, const std::string& t_filepath) {
        // Create a new builder helper class to load the object from the specified file.
		Builder builder{};
		builder.loadModel(t_filepath);

        // Create the model using the object that was loaded by the builder.
		return std::make_unique<Ae3DModel>(t_device, builder, 0);
	}



    // Create the model's vertex buffer.
	void Ae3DModel::createVertexBuffer(const std::vector<Vertex> &t_vertices) {

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
	void Ae3DModel::createIndexBuffer(const std::vector<uint32_t>& t_indices) {
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
	void  Ae3DModel::bind(VkCommandBuffer t_commandBuffer) {
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
	void  Ae3DModel::draw(VkCommandBuffer t_commandBuffer, int t_objectBufferIndex) {
		if (m_hasIndexBuffer) {
            // Draw using the indexed vertex buffer if available.
			vkCmdDrawIndexed(t_commandBuffer, m_indexCount, 1, 0, 0, t_objectBufferIndex);
		}
		else {
            // If just using a vertex buffer draw only using vertex buffer information.
			vkCmdDraw(t_commandBuffer, m_vertexCount, 1, 0, t_objectBufferIndex);
		}
		
	}



    // Define the vertex binding descriptions used to bind the vertex buffer type to the pipeline.
	std::vector<VkVertexInputBindingDescription> Ae3DModel::Vertex::getBindingDescriptions() {

		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

        // Define the general structure of the vertex buffer for binding.
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}



    // Define the vertex attribute descriptions used to bind the vertex buffer type to the pipeline.
	std::vector<VkVertexInputAttributeDescription> Ae3DModel::Vertex::getAttributeDescriptions() {

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        // Describe the data composition of each vertex data set input.
		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT , offsetof(Vertex, position)});
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT , offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT , offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT , offsetof(Vertex, uv) });

		return attributeDescriptions;
	}



    // Load the model's vertices and indices for the object at the specified file path.
	void Ae3DModel::Builder::loadModel(const std::string& t_filepath) {

        // Declare variables to store the object data from the tiny object loader.
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

        // Attempt to load the object using tinyObjectLoader.
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, t_filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

        // Clear the structs data to ensure only the new object data is loaded.
		vertices.clear();
		indices.clear();

        // Create an unordered map that uses the Vertex struct as it's key to allow its hash to be used to identify if
        // the current vertex is unique or already exists in the map. This allows the index of non-unique vertices to
        // point to the already existing vertex data instead of replicating the data.s
		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        // Loop through all the vertex data imported by the tinyObject loader.
		for (const auto &shape : shapes) {
			for (const auto &index : shape.mesh.indices) {

                // Create a unique vertex struct for the data of the current vertex being evaluated.
				Vertex vertex{};

                // Get the vertex position and color data.
				if (index.vertex_index >= 0) {
					vertex.position = { 
						attrib.vertices[3 * index.vertex_index + 0], 
						attrib.vertices[3 * index.vertex_index + 1], 
						attrib.vertices[3 * index.vertex_index + 2],
					};

                    adjustObbForVertex(vertex.position);

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};					
				}

                // Get the vertex normal data.
				if (index.normal_index >= 0) {
					vertex.normal = { 
						attrib.normals[3 * index.normal_index + 0], 
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

                // Get the vertex texture data.
				if (index.texcoord_index >= 0) {
					vertex.uv = { 
						attrib.texcoords[2 * index.texcoord_index + 0], 
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

                // Use the hashing function of the unordered map to identify if the vertex is unique.
				if (uniqueVertices.count(vertex) == 0) {
                    // Add the unique vertex to the unique vertices unordered map.
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    // Add the vertex data to the builder's vertices data that will be used to make the model's vertex
                    // buffer later.
					vertices.push_back(vertex);
				}

                // Specify the index for the vertex data that was created from the imported object file data. The hash
                // of the vertex struct will ensure matching vertex data will get the same index.s
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

    //==================================================================================================================
    // OBB Functions
    //==================================================================================================================

    void Ae3DModel::Builder::adjustObbForVertex(glm::vec3 t_vertexPosition){
        if(t_vertexPosition.x < obb.minX){
            obb.minX = t_vertexPosition.x;
        }
        if(t_vertexPosition.y < obb.minY){
            obb.minY = t_vertexPosition.y;
        }
        if(t_vertexPosition.z < obb.minZ){
            obb.minZ = t_vertexPosition.z;
        }

        if(t_vertexPosition.x > obb.maxX){
            obb.maxX = t_vertexPosition.x;
        }
        if(t_vertexPosition.y > obb.maxY){
            obb.maxY = t_vertexPosition.y;
        }
        if(t_vertexPosition.z > obb.maxZ){
            obb.maxZ = t_vertexPosition.z;
        }
    };

    // Create the model's vertex buffer.
    void Ae3DModel::createObbVertexBuffer(const VkAabbPositionsKHR &t_obb) {

        std::vector<ObbVertex> vertices{24};

        vertices[0].position = {t_obb.minX, t_obb.maxY, t_obb.minZ};
        vertices[1].position = {t_obb.minX, t_obb.maxY, t_obb.maxZ};

        vertices[2].position = {t_obb.minX, t_obb.maxY, t_obb.minZ};
        vertices[3].position = {t_obb.maxX, t_obb.maxY, t_obb.minZ};

        vertices[4].position = {t_obb.minX, t_obb.maxY, t_obb.minZ};
        vertices[5].position = {t_obb.minX, t_obb.minY, t_obb.minZ};

        vertices[6].position = {t_obb.maxX, t_obb.maxY, t_obb.maxZ};
        vertices[7].position = {t_obb.minX, t_obb.maxY, t_obb.maxZ};

        vertices[8].position = {t_obb.maxX, t_obb.maxY, t_obb.maxZ};
        vertices[9].position = {t_obb.maxX, t_obb.maxY, t_obb.minZ};

        vertices[10].position = {t_obb.maxX, t_obb.maxY, t_obb.maxZ};
        vertices[11].position = {t_obb.maxX, t_obb.minY, t_obb.maxZ};

        vertices[12].position = {t_obb.minX, t_obb.minY, t_obb.maxZ};
        vertices[13].position = {t_obb.maxX, t_obb.minY, t_obb.maxZ};

        vertices[14].position = {t_obb.minX, t_obb.minY, t_obb.maxZ};
        vertices[15].position = {t_obb.minX, t_obb.minY, t_obb.minZ};

        vertices[16].position = {t_obb.minX, t_obb.minY, t_obb.maxZ};
        vertices[17].position = {t_obb.minX, t_obb.maxY, t_obb.maxZ};

        vertices[18].position = {t_obb.maxX, t_obb.minY, t_obb.minZ};
        vertices[19].position = {t_obb.maxX, t_obb.minY, t_obb.maxZ};

        vertices[20].position = {t_obb.maxX, t_obb.minY, t_obb.minZ};
        vertices[21].position = {t_obb.minX, t_obb.minY, t_obb.minZ};

        vertices[22].position = {t_obb.maxX, t_obb.minY, t_obb.minZ};
        vertices[23].position = {t_obb.maxX, t_obb.maxY, t_obb.minZ};

//        vertices[0].position = {t_obb.minX, t_obb.minY, t_obb.minZ};
//        vertices[1].position = {t_obb.minX, t_obb.minY, t_obb.maxZ};
//        vertices[3].position = {t_obb.minX, t_obb.maxY, t_obb.maxZ};
//        vertices[4].position = {t_obb.maxX, t_obb.minY, t_obb.minZ};
//        vertices[5].position = {t_obb.maxX, t_obb.minY, t_obb.maxZ};
//        vertices[6].position = {t_obb.maxX, t_obb.maxY, t_obb.minZ};
//        vertices[7].position = {t_obb.maxX, t_obb.maxY, t_obb.maxZ};


        // Get the size of the vertex vector and ensure that we at least have one triangle.
        auto vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");

        // Get the required size of the buffer.
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        // Get the size of an individual vertex.
        uint32_t vertexSize = sizeof(vertices[0]);

        // Create a buffer to stage the model data from the host and it into the GPU.
        // Want the host (CPU) to be able to access the GPU's buffer memory and keep the GPU buffer memory consistent
        // with the host memory.
        AeBuffer stagingBuffer{
                m_aeDevice,
                vertexSize,
                vertexCount,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        // Map the memory required for the staging buffer.
        stagingBuffer.map();

        // Write the vertex data to the staging buffer.
        stagingBuffer.writeToBuffer((void*)vertices.data());

        // Create a buffer only on the GPU that will store the vertex data for this model until it is no longer
        // required.
        m_obbVertexBuffer = std::make_unique<AeBuffer>(
                m_aeDevice,
                vertexSize,
                vertexCount,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // Copy the data from the staging buffer (host synced to GPU) to the vertex buffer (only on GPU)
        m_aeDevice.copyBuffer(stagingBuffer.getBuffer(), m_obbVertexBuffer->getBuffer(), bufferSize);

        // Remember staging buffer will be cleaned up here now that we're moving out of the staging buffer's scope.
    }



    // Define the vertex binding descriptions used to bind the vertex buffer type to the pipeline.
    std::vector<VkVertexInputBindingDescription> Ae3DModel::ObbVertex::getBindingDescriptions() {

        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

        // Define the general structure of the vertex buffer for binding.
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(ObbVertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescriptions;
    }



    // Define the vertex attribute descriptions used to bind the vertex buffer type to the pipeline.
    std::vector<VkVertexInputAttributeDescription> Ae3DModel::ObbVertex::getAttributeDescriptions() {

        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        // Describe the data composition of each vertex data set input.
        attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT , offsetof(ObbVertex, position)});

        return attributeDescriptions;
    }

    // Bind the vertex and index buffers to the specified command buffer to they are available to the command buffer
    // when executing draw commands for this model.
    void  Ae3DModel::bindAabb(VkCommandBuffer t_commandBuffer) {
        // Create buffer and offset arrays to store the vertex buffer to make them compatible with the command buffer
        // bind command.
        VkBuffer buffers[] = {m_obbVertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(t_commandBuffer, 0, 1, buffers, offsets);
    }

    // Submit the draw command for the model to the specified command buffer.
    void  Ae3DModel::drawAabb(VkCommandBuffer t_commandBuffer) {
        // If just using a vertex buffer draw only using vertex buffer information.
        vkCmdDraw(t_commandBuffer, 24, 1, 0, 0);
    }

} //namespace ae
