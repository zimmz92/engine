#pragma once

#include "ae_model.hpp"

// std
#include <memory>

namespace ae {

	struct Transform2dComponent {
		glm::vec2 translation{}; // (position offset)
		glm::vec2 scale{ 1.0f, 1.0f };
		float rotation;

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{{c, s}, {-s, c}};

			glm::mat2 scaleMat{ {scale.x, 0.0f}, {0.0f, scale.y} };

			return rotMatrix*scaleMat;
		}
	};


	class AeGameObject {
	public:
		using id_t = unsigned int;

		static AeGameObject createGameObject() {
			static id_t currentId = 0;
			return AeGameObject{currentId++};
		}

		// Do not allow this class to be copied (2 lines below)
		AeGameObject(const AeGameObject&) = delete;
		AeGameObject& operator=(const AeGameObject&) = delete;

		// Allow the move and assignment operators
		AeGameObject(AeGameObject&&) = default;
		AeGameObject& operator=(AeGameObject&&) = default;

		const id_t getID() { return m_id; }

		std::shared_ptr<AeModel> m_model{};
		glm::vec3 m_color{};
		Transform2dComponent m_transform2d{};

	private:
		AeGameObject(id_t t_objId) : m_id{ t_objId } {}

		id_t m_id;
	};
}; // namespace ae