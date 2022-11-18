#pragma once

#include "ae_component.hpp"
#include "ae_component_manager.hpp"

#include <cstdint>
#include <vector>
#include <array>
#include <bitset>

#define MAX_ENTITIES 5000
#define MAX_COMPONENTS 32

namespace ae {

	class AeEntity {
	public:

		// Function to create an entity
		AeEntity(AeComponentManager& t_componentManager, std::uint64_t t_entityId, std::uint64_t t_entityTypeId);

		// Function to destroy an entity
		~AeEntity();

		// Do not allow this class to be copied (2 lines below)
		AeEntity(const AeEntity&) = delete;
		AeEntity& operator=(const AeEntity&) = delete;

		// Do not allow this class to be moved (2 lines below)
		AeEntity(AeEntity&&) = delete;
		AeEntity& operator=(AeEntity&&) = delete;

		std::uint64_t getEntityId() const { return m_entityId; }
		std::uint64_t getEntityTypeId() const { return m_entityTypeId; }
		
		template <typename T> void useComponent(std::uint64_t t_componentId, T t_entityComponentData);

		bool componentUsed(std::uint64_t t_componentId);

	private:

		// ID for the unique entity created
		std::uint64_t m_entityId;

		// ID for the entity class that the entity was based on
		std::uint64_t m_entityTypeId;

		// vector storing the typeids
		std::bitset<MAX_COMPONENTS> m_componentSignature;

		// Pointer to the component manager
		AeComponentManager& m_componentManager;
		

	protected:
		
	};
}