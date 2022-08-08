#include "ae_entity.hpp"
#include "ae_component_manager.hpp"

#include <stdexcept>
#include <algorithm>
#include <string>

namespace ae {
	AeEntity::AeEntity() {};
	AeEntity::~AeEntity() {};

	template <typename T>
	void AeEntity::useComponent(std::uint64_t t_componentId, T t_entityComponentData) {

		// Make sure that the entity does not already use the component
		if (componentUsed(t_componentId)) {
			throw std::runtime_error(
				"Cannot log the same component twice for the same entity. Why would you to this... why? EntityID: " + std::to_string(m_entityId)
				+ ", EntityTypeID: " + std::to_string(m_entityTypeId)
				+ ", ComponentID: " + std::to_string(t_componentId)
			);
		};

		m_usedComponents.push_back(t_componentId);
		m_componentManager.addEntityComponentData(m_entityId, t_componentId, t_entityComponentData);
	};


	bool AeEntity::componentUsed(std::uint64_t t_componentId) {

		return m_usedComponents.end() != std::find(m_usedComponents.begin(), m_usedComponents.end(), t_componentId);

	};
}