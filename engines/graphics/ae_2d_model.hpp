/// \file ae_2d_model.hpp
/// \brief Defines the model for 2-D game objects.
/// Defines the model for 2-D game objects such as user interface elements or billboards.
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
	class Ae2DModel {
	public:
		struct Vertex2D {
            /// The position of the vertex.
			glm::vec2 position{};

            /// The color of the vertex.
			glm::vec3 color{};

            /// Texture coordinate of the vertex.
			glm::vec2 uv{};

            /// Get the binding descriptions for the model.
            /// \return The 2-D object's vertex buffer binding description.
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

            /// Get the attribute descriptions for the model.
            /// \return The 2-D object's vertex buffer binding attributes.
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            /// Overload the comparison function to allow the Vertex struct to be used as a key in an unordered map
			bool operator==(const Vertex2D& t_other) const {
				return position == t_other.position && color == t_other.color && uv == t_other.uv;
			}
		};

		struct Builder {
            /// Vector to store the vertex data until a vertex buffer is created for it.
			std::vector<Vertex2D> vertices{};

            /// Vector to store the index data until an index buffer is created for it.
			std::vector<uint32_t> indices{};

            /// Loads a model from the file at the specified path and populates the Builder's struct it is being called
            /// for.
            /// \param t_filepath The path to the model file to be loaded.
			void loadModel(const std::vector<Vertex2D> &t_vertices);
		};

        /// Construct a new AeModel and creates the model buffers ready for the specified device using the model built
        /// by the supplied builder.
        /// \param t_device The GPU the model buffers should be prepared for and maybe loaded onto.
        /// \param t_builder The builder that contains the model information that buffers should be created for ready
        /// for the specified device.
        Ae2DModel(AeDevice &t_device, const Ae2DModel::Builder &t_builder);

        /// Destroy the AeModel object.
		~Ae2DModel();

		/// Do not allow this class to be copied (2 lines below)
        Ae2DModel(const Ae2DModel&) = delete;
        Ae2DModel& operator=(const Ae2DModel&) = delete;

        /// Creates an Ae2DModel using the specified vertices which is compatible with the specified GPU.
        /// \param t_device The GPU the created model will be compatible with and will have buffer created for.
        /// \param t_vertices The superset of vertices specifying the 2D object.
        static std::unique_ptr<Ae2DModel> createModelFromFile(AeDevice& t_device, const std::vector<Vertex2D> &t_vertices);

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
		void createVertexBuffers(const std::vector<Vertex2D> &t_vertices);

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