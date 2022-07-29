#pragma once

#include <cstdint>
#include <vector>

namespace ae {

	class AeEntity {
	public:

		// Function to create an entity
		AeEntity();

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
		

	protected:
		// ID for the unique entity created
		std::uint64_t m_entityId;

		// ID for the entity class that the entity was based on
		std::uint64_t m_entityTypeId;

		// vector storing the typeids
		std::vector<uint64_t> m_usedComponents;

		// Pointer to the component manager
		AeComponentManager& m_componentManager;
	};
}