#pragma once

#include "ae_engine_constants.hpp"
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
	class Ae3DModel {
	public:
        struct ObbVertex{
            /// The position of the aabb vertex.
            glm::vec3 position{};

            /// Get the binding descriptions for the model.
            /// \return The model's vertex buffer binding description.
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

            /// Get the attribute descriptions for the model.
            /// \return The model's vertex buffer binding attributes.
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

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
            /// \return The model's vertex buffer binding description.
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

            /// Get the attribute descriptions for the model.
            /// \return The model's vertex buffer binding attributes.
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

            /// The OBB for the loaded model.
            VkAabbPositionsKHR obb{};

            /// Loads a model from the file at the specified path and populates the Builder's struct it is being called
            /// for.
            /// \param t_filepath The path to the model file to be loaded.
			void loadModel(const std::string& t_filepath);

            void adjustObbForVertex(glm::vec3 t_vertexPosition);
		};

        /// Construct a new AeModel and creates the model buffers ready for the specified device using the model built
        /// by the supplied builder.
        /// \param t_device The GPU the model buffers should be prepared for and maybe loaded onto.
        /// \param t_builder The builder that contains the model information that buffers should be created for ready
        /// for the specified device.
		Ae3DModel(AeDevice &t_device, const Ae3DModel::Builder &t_builder, const ssbo_idx t_idxObbSsbo);

        /// Destroy the AeModel object.
		~Ae3DModel();

		/// Do not allow this class to be copied (2 lines below)
		Ae3DModel(const Ae3DModel&) = delete;
		Ae3DModel& operator=(const Ae3DModel&) = delete;

        /// Creates an AeModel using the specified object data stored at the specified file path compatible with the
        /// specified GPU.
        /// \param t_device The GPU the created model will be compatible with and will have buffer created for.
        /// \param t_filepath The location of the file defining the model to be created.
		static std::unique_ptr<Ae3DModel> createModelFromFile(AeDevice& t_device, const std::string& t_filepath);

        /// Binds the model's vertex buffer, and if available index buffer, to the specified command buffer.
        /// \param t_commandBuffer The command buffer that this model's vertex and index buffer(s) shall be bound to.
		void bind(VkCommandBuffer t_commandBuffer);

        /// Will draw the model using the model's index buffer if available, otherwise will use the vertex buffer
        /// information.
        /// \param t_commandBuffer The command buffer that the model's buffers were bound to and will actually execute
        /// the draw call.
        /// \param t_objectBufferIndex The index into the SSBO that stores the object's model matrix and texture index.
		void draw(VkCommandBuffer t_commandBuffer,int t_objectBufferIndex = 0);

        uint32_t getIndexCount() const {return m_indexCount;};


        /// Binds the model's vertex buffer, and if available index buffer, to the specified command buffer.
        /// \param t_commandBuffer The command buffer that this model's vertex and index buffer(s) shall be bound to.
        void bindAabb(VkCommandBuffer t_commandBuffer);

        /// Will draw the model using the model's index buffer if available, otherwise will use the vertex buffer
        /// information.
        /// \param t_commandBuffer The command buffer that the model's buffers were bound to and will actually execute
        /// the draw call.
        void drawAabb(VkCommandBuffer t_commandBuffer);

        VkAabbPositionsKHR getobb() {return m_obb;};

	private:
        /// Creates a vertex buffer from the provided vertices.
        /// \param t_vertices The vertices to create the vertex buffer using.
		void createVertexBuffer(const std::vector<Vertex> &t_vertices);

        /// Create a index buffer from the provided indices
        /// \param t_indicies The indices to create the index buffer using.
		void createIndexBuffer(const std::vector<uint32_t>& t_indices);


        /// Creates a vertex buffer from the provided vertices.
        /// \param t_vertices The vertices to create the vertex buffer using.
        void createObbVertexBuffer(const VkAabbPositionsKHR &t_obb);

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

        /// The vertex buffer data for the model's OBB.
        std::unique_ptr<AeBuffer> m_obbVertexBuffer;

        /// The OBB of this model
        VkAabbPositionsKHR m_obb;

        /// The index of the model in the OBB SSBO
        ssbo_idx m_idxObbSsbo;

	};
} // namespace ae