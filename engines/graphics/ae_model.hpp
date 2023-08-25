#pragma once

#include "ae_device.hpp"
#include "ae_buffer.hpp"

// libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <memory>
#include <vector>

namespace ae {
	class AeModel {
	public:
		struct Vertex {
            /// The position of the vertex.
			glm::vec3 position{};
            /// The color of the vertex.
			glm::vec3 color{};

            /// The vertex normal.
			glm::vec3 normal{};

            /// Texture coordinate of the vertex.
			glm::vec2 uv{};

            /// Get the binding descriptions for the model.
            /// \return
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

            /// Get the attribute descriptions for the model.
            /// \return
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            /// Overload the comparison function to allow the Vertex struct to be used as a key in an unordered map
			bool operator==(const Vertex& t_other) const {
				return position == t_other.position && color == t_other.color && normal == t_other.normal && uv == t_other.uv;
			}
		};

		struct Builder {
            /// Vector to store the vertex data until a vertex buffer is created for it.
			std::vector<Vertex> vertices{};

            /// Vector to store the index data until an index buffer is created for it.
			std::vector<uint32_t> indices{};

            /// Loads a model from the file at the specified path and populates the Builder's struct it is being called
            /// for.
            /// \param t_filepath The path to the model file to be loaded.
			void loadModel(const std::string& t_filepath);
		};

        /// Construct a new AeModel and creates the model buffers ready for the specified device using the model built
        /// by the supplied builder.
        /// \param t_device The GPU the model buffers should be prepared for and maybe loaded onto.
        /// \param t_builder The builder that contains the model information that buffers should be created for ready
        /// for the specified device.
		AeModel(AeDevice &t_device, const AeModel::Builder &t_builder);

        /// Destroy the AeModel object.
		~AeModel();

		/// Do not allow this class to be copied (2 lines below)
		AeModel(const AeModel&) = delete;
		AeModel& operator=(const AeModel&) = delete;

        /// Creates an AeModel using the specified object data stored at the specified file path compatible with the
        /// specified GPU.
        /// \param t_device The GPU the created model will be compatible with and will have buffer created for.
        /// \param t_filepath The location of the file defining the model to be created.
		static std::unique_ptr<AeModel> createModelFromFile(AeDevice& t_device, const std::string& t_filepath);

        /// Binds the model's vertex buffer, and if available index buffer, to the specified command buffer.
        /// \param t_commandBuffer The command buffer that this model's vertex and index buffer(s) shall be bound to.
		void bind(VkCommandBuffer t_commandBuffer);

        /// Will draw the model using the model's index buffer if available, otherwise will use the vertex buffer
        /// information.
        /// \param t_commandBuffer The command buffer that the model's buffers were bound to and will actually execute
        /// the draw call.
		void draw(VkCommandBuffer t_commandBuffer);

	private:
        /// Creates a vertex buffer from the provided vertices.
        /// \param t_vertices The vertices to create the vertex buffer using.
		void createVertexBuffers(const std::vector<Vertex> &t_vertices);

        /// Create a index buffer from the provided indices
        /// \param t_indicies The indices to create the index buffer using.
		void createIndexBuffers(const std::vector<uint32_t>& t_indices);

        /// The device the model is compatible with and buffers are created for submitting command to.
		AeDevice &m_aeDevice;

        /// The vertex buffer data for the model.
		std::unique_ptr<AeBuffer> m_vertexBuffer;
        /// The number of vertices of the model.
		uint32_t m_vertexCount;

        /// Flag to indicate if the specific model has an index buffer available.
		bool m_hasIndexBuffer = false;
        /// The index buffer data for the model.
		std::unique_ptr<AeBuffer> m_indexBuffer;
        /// The number of indices of the model.
		uint32_t m_indexCount;
	};
} // namespace ae