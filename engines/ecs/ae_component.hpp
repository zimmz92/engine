/// \file ae_component.hpp
/// \brief The script defining the template component class.
/// The template component class is defined.
#pragma once

#include "ae_ecs.hpp"
#include "ae_component_base.hpp"

#include <cstdint>

namespace ae_ecs {

    /// The template class for a component
	template <typename T>
	class AeComponent : public AeComponentBase {

		/// ID for the specific component
		static const ecs_id m_componentTypeId;

	public:

        /// Function to create a component, specify the specific manager for the component, and allocate memory for the
        /// component data.
        /// \param t_componentManager The component manager that will manage this component.
		explicit AeComponent(AeECS& t_ecs) : AeComponentBase(t_ecs) {

			// TODO: Allow the use of different memory architectures
            // TODO: Allow for a stack instead of allocating memory for every entity even if every entity will never
            //  have a component, for instance there will not be a ton of cameras.
			m_componentData = new T[MAX_NUM_ENTITIES];
            for(ecs_id i = 1; i<MAX_NUM_ENTITIES; i++){
                T templateComponentData;
                m_componentData[i] = templateComponentData;
            };
		};

        /// Component destructor. Ensures that the memory of the component is released.
		~AeComponent() {

			delete[] m_componentData;
            m_componentData = nullptr;
		};

        /// Gets the ID of the component type.
        /// \return The type ID of the component.
        [[nodiscard]] ecs_id getComponentTypeId() const { return m_componentTypeId; }

        /// Alerts the component manager that a specific entity uses a component and returns a reference to the location
        /// allocated in memory for the storage of the entities data.
        /// \param t_entityId The ID of the entity using the component.
        T& requiredByEntityReference(ecs_id t_entityId) {
            m_componentManager.entityUsesComponent(t_entityId, m_componentId);

            return getWriteableDataReference(t_entityId);
            // TODO: If stack type component allocate additional memory for the entity on the stack.
        };

        /// Removes an entities data from the component. This must be defined but does not actually have to
        /// delete/initialize any data.
        /// \param t_entityId
        virtual void removeEntityData(ecs_id t_entityId) override {
            T templateComponentData;
            m_componentData[t_entityId] = templateComponentData;
        };

        /// Get data for a specific entity.
		/// \param t_entityID The ID of the entity to return the component data for.
        virtual T& getWriteableDataReference(ecs_id t_entityId) {
            m_componentManager.entitiesComponentUpdated(t_entityId, m_componentId);
			return m_componentData[t_entityId];
		};

        /// Get data for a specific entity.
        /// \param t_entityID The ID of the entity to return the component data for.
        const T& getReadOnlyDataReference (ecs_id t_entityID) const {
            return m_componentData[t_entityID];
        };

	private:

	protected:

        /// Pointer to the data the component is storing.
        T* m_componentData;
	};

	/// When a derivative of the AeComponent class is defined the type ID will be set for the derivative class
	template <class T>
	const ecs_id AeComponent<T>::m_componentTypeId = AeComponentManager::allocateComponentTypeId<T>();
}