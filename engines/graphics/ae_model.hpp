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
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

            /// Get the binding descriptions for the model.
            /// \return
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

            /// Get the attribute descriptions for the model.
            /// \return
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& t_other) const {
				return position == t_other.position && color == t_other.color && normal == t_other.normal && uv == t_other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& t_filepath);
		};

		AeModel(AeDevice &t_device, const AeModel::Builder &t_builder);
		~AeModel();

		// Do not allow this class to be copied (2 lines below)
		AeModel(const AeModel&) = delete;
		AeModel& operator=(const AeModel&) = delete;

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
        /// Creates the vertex buffers from the provided vertices.
        /// \param t_vertices The vertices to create the vertex buffer using.
		void createVertexBuffers(const std::vector<Vertex> &t_vertices);


		void createIndexBuffers(const std::vector<uint32_t>& t_indicies);

		AeDevice &m_aeDevice;

		std::unique_ptr<AeBuffer> m_vertexBuffer;
		uint32_t m_vertexCount;

		bool m_hasIndexBuffer = false;
		std::unique_ptr<AeBuffer> m_indexBuffer;
		uint32_t m_indexCount;
	};
} // namespace ae