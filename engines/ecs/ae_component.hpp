/// \file ae_component.hpp
/// \brief The script defining the template component class.
/// The template component class is defined.
#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_component_manager.hpp"

#include <cstdint>

namespace ae_ecs {

    /// The template class for a component
	template <typename T>
	class AeComponent {

		/// ID for the specific component
		static const ecs_id m_componentTypeId;

	public:

        /// Function to create the entity using the default entity, component, and system managers of the ecs engine
        AeComponent() : AeComponent(ecsComponentManager) {};

        /// Function to create a component, specify the specific manager for the component, and allocate memory for the
        /// component data.
        /// \param t_componentManager The component manager that will manage this component.
		explicit AeComponent(AeComponentManager& t_componentManager) : m_componentManager{ t_componentManager } {
            // Get an ID for the component from the component manager
			m_componentId = m_componentManager.allocateComponentId();

			// TODO: Allow the use of different memory architectures
            // TODO: Allow for a stack instead of allocating memory for every entity even if every entity will never
            //  have a component, for instance there will not be a ton of cameras.
			m_componentData = new T[MAX_NUM_ENTITIES];
            for(int i = 1; i<MAX_NUM_ENTITIES; i++){
                T templateComponentData;
                m_componentData[i] = templateComponentData;
            };
		};

        /// Component destructor. Ensures that the component ID and the memory of the component are released.
		~AeComponent() {
            m_componentManager.releaseComponentId(m_componentId);
			delete[] m_componentData;
            m_componentData = nullptr;
		};

        /// Gets the ID of the component.
        /// \return The ID of the component.
		ecs_id getComponentId() const { return m_componentId; }

        /// Gets the ID of the component type.
        /// \return The type ID of the component.
        ecs_id getComponentTypeId() const { return m_componentTypeId; }

        /// Alerts the component manager that a specific entity uses a component and returns a reference to the location
        /// allocated in memory for the storage of the entities data.
        /// \param t_entityId The ID of the entity using the component.
        void requiredByEntity(ecs_id t_entityId) {
            m_componentManager.setEntityComponentSignature(t_entityId, m_componentId);
            // TODO: If stack type component allocate additional memory for the entity on the stack.
        };


        /// Alerts the component manager that a specific entity no longer uses a component.
        /// \param t_entityId The ID of the entity that no longer uses this component.
		void unrequiredByEntity(ecs_id t_entityId) {
            m_componentManager.unsetEntityComponentSignature(t_entityId, m_componentId);
			// TODO: Alert system manager that this entity no longer uses this component.
		};

        /// Checks with the component manager if an entity uses this component.
        /// \param t_entityId The ID of the entity that may use this component
        /// \return True if the entity uses this component.
        bool doesEntityUseThis(ecs_id t_entityId){
            return m_componentManager.isComponentUsed(t_entityId, this->m_componentId);
        };

        /// Fetches the entities that use this component.
        /// \return A vector of entity IDs that use this component.
        std::vector<ecs_id> getMyEntities(){
            return m_componentManager.getComponentEntities(this->m_componentId);
        };

        /// Alerts the component manager that a system requires this component to operate.
        /// \param t_systemId The ID of the system that requires this component to operate.
        void requiredBySystem(ecs_id t_systemId) {
            m_componentManager.setSystemComponentSignature(t_systemId, m_componentId);
            // TODO: Alert system manager that this system uses this component.
        };

        /// Alerts the component manager that a system does not require this component to operate.
        /// \param t_systemId The ID of the system that no longer requires this component to operate.
        void unrequiredBySystem(ecs_id t_systemId) {
            m_componentManager.unsetSystemComponentSignature(t_systemId, m_componentId);
            // TODO: Alert system manager that this system uses this component.
        };

		/// Update component data for a specific entity.
		/// \param t_entityId The ID of the entity the data is being stored in the component for.
		/// \param t_entityComponentData The data to be stored for the entity in the component.
		void updateData(ecs_id t_entityId, T t_entityComponentData) {
			// Check that an entity has actually registered that it uses the component before 
			// updating its data to ensure data will not collide
			m_componentManager.isComponentUsed(t_entityId, this->m_componentId);
			m_componentData[t_entityId] = t_entityComponentData;
		};

		/// Get data for a specific entity.
		/// \param t_entityID The ID of the entity to return the component data for.
		T& getDataReference(ecs_id t_entityID) {
			return m_componentData[t_entityID];
		};

	private:

        /// Pointer to the data the component is storing.
		T* m_componentData;

	protected:
        /// ID for the unique component created
        ecs_id m_componentId;

		/// Pointer to the component manager
		AeComponentManager& m_componentManager;
	};

	/// When a derivative of the AeComponent class is defined the type ID will be set for the derivative class
	template <class T>
	const ecs_id AeComponent<T>::m_componentTypeId = AeComponentManager::allocateComponentTypeId<T>();
}