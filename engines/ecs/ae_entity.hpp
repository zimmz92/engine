#pragma once

#include "ae_component.hpp"
#include "ae_component_manager.hpp"
#include "ae_id_counters.hpp"

#include <cstdint>
#include <vector>
#include <array>
#include <bitset>

#define MAX_ENTITIES 5000
#define MAX_COMPONENTS 32

namespace ae {

	template <class T>
	class AeEntity {

		// ID for the entity class that the entity was based on
		static const std::int64_t m_entityTypeId;

	public:

		// Function to create an entity
		AeEntity(AeComponentManager& t_componentManager) : m_componentManager{ t_componentManager } {
			m_entityId = 1;
			m_componentSignature = 0;
		};

		// Function to destroy an entity
		~AeEntity() {};

		// Do not allow this class to be copied (2 lines below)
		AeEntity(const AeEntity&) = delete;
		AeEntity& operator=(const AeEntity&) = delete;

		// Do not allow this class to be moved (2 lines below)
		AeEntity(AeEntity&&) = delete;
		AeEntity& operator=(AeEntity&&) = delete;

		std::int64_t getEntityId() const { return m_entityId; }
		std::int64_t getEntityTypeId() const { return m_entityTypeId; }
		
		template <typename U> void useComponent(std::int64_t t_componentId, U t_entityComponentData) {

			// Make sure that the entity does not already use the component
			if (componentUsed(t_componentId)) {
				throw std::runtime_error(
					"Cannot log the same component twice for the same entity. Why would you to this... why? EntityID: " + std::to_string(m_entityId)
					+ ", EntityTypeID: " + std::to_string(m_entityTypeId)
					+ ", ComponentID: " + std::to_string(t_componentId)
				);
			};

			m_componentSignature[t_componentId] = 1;
			m_componentManager.addEntityComponentData<U>(m_entityId, t_componentId, t_entityComponentData);
		};

		bool componentUsed(std::int64_t t_componentId) {

			return m_componentSignature.test(t_componentId);

		};

	private:

		// ID for the unique entity created
		std::int64_t m_entityId;

		// vector storing the typeids
		std::bitset<MAX_COMPONENTS> m_componentSignature;

		// Pointer to the component manager
		AeComponentManager& m_componentManager;
		

	protected:
		
	};

	template <class T>
	const std::int64_t AeEntity<T>::m_entityTypeId = AeIdCounters::allocateEntityTypeId<T>();

}