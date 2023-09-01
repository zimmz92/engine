#pragma once

#include "ae_model.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace ae {

	struct TransformComponent {
		glm::vec3 translation{}; // (position offset)
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation;

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct PointLightComponentOld {
		float lightIntensity = 1.0f;
	};

	class AeGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, AeGameObject>;

		static AeGameObject createGameObject() {
			static id_t currentId = 0;
			return AeGameObject{currentId++};
		}

		static AeGameObject makePointLight(float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

		// Do not allow this class to be copied (2 lines below)
		AeGameObject(const AeGameObject&) = delete;
		AeGameObject& operator=(const AeGameObject&) = delete;

		// Allow the move and assignment operators
		AeGameObject(AeGameObject&&) = default;
		AeGameObject& operator=(AeGameObject&&) = default;

		const id_t getID() { return m_id; }

		glm::vec3 m_color{};
		TransformComponent m_transform{};

		// Optional pointer components
		std::shared_ptr<AeModel> m_model{};
		std::unique_ptr<PointLightComponentOld> m_pointLight = nullptr;

	private:
		AeGameObject(id_t t_objId) : m_id{ t_objId } {}

		id_t m_id;
	};
}; // namespace ae