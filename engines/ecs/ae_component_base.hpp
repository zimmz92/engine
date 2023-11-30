/// \file ae_component_base.hpp
/// \brief The script defining the base component class.
/// The base component class is defined.
#pragma once

#include "ae_ecs.hpp"
#include "ae_component_manager.hpp"

#include <cstdint>

namespace ae_ecs {

    class AeComponentManager;

    /// The class providing the base framework for a component.
	class AeComponentBase {
        friend class AeComponentManager;

	public:

        /// Function to create a component, specify the specific manager for the component, and allocate memory for the
        /// component data.
        /// \param t_componentManager The component manager that will manage this component.
		explicit AeComponentBase(AeECS& t_ecs);


        /// Component destructor. Ensures that the component ID and the memory of the component are released.
		~AeComponentBase();


        /// Gets the ID of the component.
        /// \return The ID of the component.
		const ecs_id getComponentId();


        /// Alerts the component manager that a specific entity uses a component.
        /// \param t_entityId The ID of the entity using the component.
        void requiredByEntity(ecs_id t_entityId);


        /// Alerts the component manager that a specific entity no longer uses a component.
        /// \param t_entityId The ID of the entity that no longer uses this component.
		void unrequiredByEntity(ecs_id t_entityId);


        /// Checks with the component manager if an entity uses this component.
        /// \param t_entityId The ID of the entity that may use this component
        /// \return True if the entity uses this component.
        bool doesEntityUseThis(ecs_id t_entityId);


        /// Fetches the entities that use this component.
        /// \return A vector of entity IDs that use this component.
        std::vector<ecs_id> getMyEntities();


        /// Alerts the component manager that a system requires this component to operate.
        /// \param t_systemId The ID of the system that requires this component to operate.
        void requiredBySystem(ecs_id t_systemId);


        /// Alerts the component manager that a system does not require this component to operate.
        /// \param t_systemId The ID of the system that no longer requires this component to operate.
        void unrequiredBySystem(ecs_id t_systemId);

	private:

	protected:

        /// Removes an entities data from the component. This must be defined but does not actually have to
        /// delete/initialize any data.
        /// \param t_entityId
        virtual void removeEntityData(ecs_id t_entityId)=0;

        /// ID for the unique component created
        ecs_id m_componentId;

		/// Pointer to the component manager
		AeComponentManager& m_componentManager;
	};
}