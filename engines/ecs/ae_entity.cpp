#include "ae_entity.hpp"
#include "ae_component_manager.hpp"

#include <stdexcept>
#include <algorithm>
#include <string>

namespace ae {

	AeEntity::AeEntity(AeComponentManager& t_componentManager, std::uint64_t t_entityId, std::uint64_t t_entityTypeId) : 
		m_componentManager{ t_componentManager }, m_entityId{ t_entityId }, m_entityTypeId{ t_entityTypeId } {}
	AeEntity::~AeEntity() {}

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

		m_componentSignature[t_componentId] = 1;
		m_componentManager.addEntityComponentData<T>(m_entityId, t_componentId, t_entityComponentData);
	};


	bool AeEntity::componentUsed(std::uint64_t t_componentId) {

		return m_componentSignature.test(t_componentId);

	};
}