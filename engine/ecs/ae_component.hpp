/// \file ae_component.hpp
/// \brief The script defining the template component class.
/// The template component class is defined.
#pragma once

#include "ae_ecs.hpp"
#include "ae_component_base.hpp"
#include "stl_wrappers.hpp"
#include "ae_de_stack_allocator.hpp"

#include <cstdint>
#include <unordered_map>

namespace ae_ecs {

    /// The template class for a component
	template <typename T>
	class AeComponent : public AeComponentBase {

		/// ID for the specific component
		static const ecs_id m_componentTypeId;

	public:

        enum ComponentStorageMethod{
            componentStorageMethod_maxEntityArray = 0,
            componentStorageMethod_unorderedMap
        };

        /// Function to create a component, specify the specific manager for the component, and allocate memory for the
        /// component data.
        /// \param t_componentManager The component manager that will manage this component.
		explicit AeComponent(AeECS& t_ecs,
                             std::size_t t_numInitialElements=MAX_NUM_ENTITIES,
                             ComponentStorageMethod t_componentStorageMethod=componentStorageMethod_maxEntityArray) :
                             m_ecs{t_ecs},
                             m_componentStorageMethod{t_componentStorageMethod},
                             AeComponentBase(t_ecs) {

            switch (m_componentStorageMethod) {
                case componentStorageMethod_maxEntityArray: {
                    m_bottomStackMarker = m_ecs.m_deStackAllocator.getBottomStackMarker();
                    m_componentDataArray = static_cast<T*>(m_ecs.m_deStackAllocator.allocateFromBottom(sizeof(T)*MAX_NUM_ENTITIES));
                    T templateComponentData;
                    for (ecs_id i = 1; i < MAX_NUM_ENTITIES; i++) {
                        m_componentDataArray[i] = templateComponentData;
                    };
                    break;
                }
                case componentStorageMethod_unorderedMap: {
                    m_componentDataMap = std::make_unique<ae::unordered_map<ecs_id, T, ae_memory::AeAllocatorBase>>(
                            t_numInitialElements, m_ecs.m_freeListAllocator);
                    break;
                }
            };
		};

        /// Component destructor. Ensures that the memory of the component is released.
		~AeComponent() {

            switch (m_componentStorageMethod) {
                case componentStorageMethod_maxEntityArray: {
                    m_ecs.m_deStackAllocator.deallocateToBottomMarker(m_bottomStackMarker);
                    m_componentDataArray = nullptr;
                    break;
                }
                case componentStorageMethod_unorderedMap: {
                    m_componentDataMap->clear();
                    m_componentDataMap = nullptr;
                    break;
                }
            };

		};

        /// Gets the ID of the component type.
        /// \return The type ID of the component.
        [[nodiscard]] ecs_id getComponentTypeId() const { return m_componentTypeId; }

        /// Alerts the component manager that a specific entity uses a component and returns a reference to the location
        /// allocated in memory for the storage of the entities data.
        /// \param t_entityId The ID of the entity using the component.
        T& requiredByEntityReference(ecs_id t_entityId) {
            m_componentManager.entityUsesComponent(t_entityId, m_componentId);

            switch (m_componentStorageMethod) {
                case componentStorageMethod_maxEntityArray: {
                    return getWriteableDataReference(t_entityId);
                    break;
                }
                case componentStorageMethod_unorderedMap: {
                    T templateComponentData;
                    m_componentDataMap->operator[](t_entityId) = templateComponentData;
                    return getWriteableDataReference(t_entityId);
                    break;
                }
                default:
                    throw std::runtime_error("No valid component storage method specified.");
            };
        };

        /// Removes an entities data from the component. This must be defined but does not actually have to
        /// delete/initialize any data.
        /// \param t_entityId
        void removeEntityData(ecs_id t_entityId) override {
            switch (m_componentStorageMethod) {
                case componentStorageMethod_maxEntityArray: {
                    T templateComponentData;
                    m_componentDataArray[t_entityId] = templateComponentData;
                    break;
                }
                case componentStorageMethod_unorderedMap: {
                    m_componentDataMap->erase(t_entityId);
                    break;
                }
            };
        };

        /// Get data for a specific entity.
		/// \param t_entityID The ID of the entity to return the component data for.
        virtual T& getWriteableDataReference(ecs_id t_entityId) {
            m_componentManager.entitiesComponentUpdated(t_entityId, m_componentId);

            switch (m_componentStorageMethod) {
                case componentStorageMethod_maxEntityArray: {
                    return m_componentDataArray[t_entityId];
                    break;
                }
                case componentStorageMethod_unorderedMap: {
                    return m_componentDataMap->at(t_entityId);
                    break;
                }
                default:
                    throw std::runtime_error("No valid component storage method specified.");
            };
		};

        /// Get data for a specific entity.
        /// \param t_entityID The ID of the entity to return the component data for.
        const T& getReadOnlyDataReference (ecs_id t_entityId) const {
            switch (m_componentStorageMethod) {
                case componentStorageMethod_maxEntityArray: {
                    return m_componentDataArray[t_entityId];
                    break;
                }
                case componentStorageMethod_unorderedMap: {
                    return m_componentDataMap->at(t_entityId);
                    break;
                }
                default:
                    throw std::runtime_error("No valid component storage method specified.");
            };
        };

	private:

	protected:

        /// Defines how the component data will be stored.
        ComponentStorageMethod m_componentStorageMethod;

        /// Pointer to the data the component is storing if using an array.
        T* m_componentDataArray = nullptr;
        ae_memory::AeDeStackAllocator::BottomStackMarker m_bottomStackMarker;

        /// Pointer to the component data if storing using an unordered map.
        std::unique_ptr<ae::unordered_map<ecs_id,T,ae_memory::AeAllocatorBase>> m_componentDataMap = nullptr;

        /// Reference to ECS that manages this component.
        ae_ecs::AeECS& m_ecs;
	};

	/// When a derivative of the AeComponent class is defined the type ID will be set for the derivative class
	template <class T>
	const ecs_id AeComponent<T>::m_componentTypeId = AeComponentManager::allocateComponentTypeId<T>();
}