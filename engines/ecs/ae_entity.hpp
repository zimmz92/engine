#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_component.hpp"
#include "ae_component_manager.hpp"
#include "ae_entity_manager.hpp"

#include <cstdint>
#include <vector>
#include <array>


namespace ae_ecs {

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

		bool isComponentUsed(std::int64_t t_componentId) {

			return m_componentManager.m_componentSignatures[m_entityId].test(t_componentId);

		};

	private:

		
	protected:

		// ID for the unique entity created
		std::int64_t m_entityId;

		// Pointer to the entity manager
		AeEntityManager& m_entityManager;

		// Pointer to the component manager
		AeComponentManager& m_componentManager;
	};

	// When a derivitive of the AeEntity class is defined the type ID will be set for the derivative class
	template <class T>
	const std::int64_t AeEntity<T>::m_entityTypeId = AeEntityManager::allocateEntityTypeId<T>();

}