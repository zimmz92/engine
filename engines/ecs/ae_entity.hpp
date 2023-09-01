/// \file ae_entity.hpp
/// \brief The script defining the template entity class.
/// The template entity class is defined.
#pragma once

#include "ae_ecs.hpp"

#include <cstdint>
#include <vector>
#include <array>


namespace ae_ecs {

    /// The template class for a entity
	template <class T>
	class AeEntity {

		/// ID for the base entity class the entity was derived from.
		static const ecs_id m_entityTypeId;

	public:

		/// Function to create an entity and specify the specific manager for the entity
		/// \param t_entityManager The entity manager that will manage this entity.
        explicit AeEntity( AeECS& t_ecs) : m_entityManager{ t_ecs.m_ecsEntityManager } {
			m_entityId = m_entityManager.registerEntity();
		};

		/// Function to destroy an entity. Only destroys this class, entity data must be destroyed through the ECS
		/// class.
		~AeEntity() {};

		/// Do not allow this class to be copied (2 lines below)
		AeEntity(const AeEntity&) = delete;
		AeEntity& operator=(const AeEntity&) = delete;

		/// Do not allow this class to be moved (2 lines below)
		AeEntity(AeEntity&&) = delete;
		AeEntity& operator=(AeEntity&&) = delete;

        /// Gets the ID of the entity.
        /// \return The ID of the entity.
		ecs_id getEntityId() const { return m_entityId; }

        /// Gets the ID of the entity type.
        /// \return The type ID of the entity.
		ecs_id getEntityTypeId() const { return m_entityTypeId; }

        /// Enables entity allowing it to be acted upon by systems
        void enableEntity() {
            m_entityManager.enableEntity(m_entityId);
        };

	private:

		
	protected:

		/// ID for the unique entity created
		ecs_id m_entityId;

		/// Pointer to the entity manager
		AeEntityManager& m_entityManager;

	};

	/// When a derivative of the AeEntity class is defined the type ID will be set for the derivative class
	template <class T>
	const ecs_id AeEntity<T>::m_entityTypeId = AeEntityManager::allocateEntityTypeId<T>();

}