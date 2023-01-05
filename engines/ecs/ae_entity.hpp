#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_component.hpp"
#include "ae_component_manager.hpp"
#include "ae_entity_manager.hpp"
#include "ae_id_counters.hpp"

#include <cstdint>
#include <vector>
#include <array>
#include <bitset>

namespace ae {

	template <class T>
	class AeEntity {

		// ID for the entity class that the entity was based on
		static const std::int64_t m_entityTypeId;

	public:

		// Function to create an entity
		AeEntity(AeComponentManager& t_componentManager, AeEntityManager& t_entityManager) : m_componentManager{ t_componentManager }, m_entityManager{ t_entityManager } {
			m_entityId = m_entityManager.allocateEntityId();
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
		
		void useComponent(std::int64_t m_componentId) {

		};

		template <class U, typename T> void useComponentTest(U &t_component, T t_componentData) {

		};

		bool componentUsed(std::int64_t t_componentId) {

			return m_componentSignature.test(t_componentId);

		};

	private:

		
	protected:

		// ID for the unique entity created
		std::int64_t m_entityId;

		// vector storing the typeids
		std::bitset<MAX_COMPONENTS> m_componentSignature = 0;

		// Pointer to the component manager
		AeComponentManager& m_componentManager;

		AeEntityManager& m_entityManager;
		
	};

	template <class T>
	const std::int64_t AeEntity<T>::m_entityTypeId = AeIdCounters::allocateEntityTypeId<T>();

}