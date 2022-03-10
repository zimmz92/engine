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
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
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

		void bind(VkCommandBuffer t_commandBuffer);
		void draw(VkCommandBuffer t_commandBuffer);

	private:
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