/// \file ae_component.hpp
/// \brief The script defining the template component class.
/// The template component class is defined.
#pragma once

#include "ae_ecs.hpp"
#include "ae_component_base.hpp"
#include "ae_allocator_stl_adapter.hpp"
#include "ae_allocator_base.hpp"

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
            componentStorageMethod_fixedSizeArray = 0,
            componentStorageMethod_unorderedMap,
            componentStorageMethod_dynamicArray
        };

        /// Function to create a component, specify the specific manager for the component, and allocate memory for the
        /// component data.
        /// \param t_componentManager The component manager that will manage this component.
		explicit AeComponent(AeECS& t_ecs, ae_memory::AeAllocatorBase& t_allocator, std::size_t t_numInitialElements=MAX_NUM_ENTITIES,ComponentStorageMethod t_componentStorageMethod=componentStorageMethod_fixedSizeArray) : m_allocator{t_allocator}, m_componentStorageMethod{t_componentStorageMethod}, AeComponentBase(t_ecs) {

            switch (m_componentStorageMethod) {
                case componentStorageMethod_fixedSizeArray:{
                    m_componentDataPtr = new T[t_numInitialElements];
                    for(ecs_id i = 1; i<t_numInitialElements; i++){
                        T templateComponentData;
                        ((T*)m_componentDataPtr)[i] = templateComponentData;
                    };
                };
                case componentStorageMethod_unorderedMap:{
                    m_componentDataPtr = unorderedMapData(t_numInitialElements,t_allocator);
                };
                case componentStorageMethod_dynamicArray:{

                };
            }
			// TODO: Allow the use of different memory architectures
            // TODO: Allow for a stack instead of allocating memory for every entity even if every entity will never
            //  have a component, for instance there will not be a ton of cameras.

		};

        /// Component destructor. Ensures that the memory of the component is released.
		~AeComponent() {

			delete[] ((T*)m_componentDataPtr);
            m_componentDataPtr = nullptr;
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
        void removeEntityData(ecs_id t_entityId) override {
            T templateComponentData;
            ((T*)m_componentDataPtr)[t_entityId] = templateComponentData;
        };

        /// Get data for a specific entity.
		/// \param t_entityID The ID of the entity to return the component data for.
        virtual T& getWriteableDataReference(ecs_id t_entityId) {
            m_componentManager.entitiesComponentUpdated(t_entityId, m_componentId);
			return ((T*)m_componentDataPtr)[t_entityId];
		};

        /// Get data for a specific entity.
        /// \param t_entityID The ID of the entity to return the component data for.
        const T& getReadOnlyDataReference (ecs_id t_entityID) const {
            return ((T*)m_componentDataPtr)[t_entityID];
        };

	private:

        struct  unorderedMapData{
            unorderedMapData(std::size_t t_numOfElements,ae_memory::AeAllocatorBase& t_allocator) : m_allocator{t_allocator}{
                m_componentData = std::unordered_map<ecs_id,T,ae_memory::AeAllocatorStlAdaptor<std::pair<ecs_id,T>,ae_memory::AeAllocatorBase>>(t_numOfElements, m_allocator);
            };

            ~unorderedMapData(){
                m_componentData->clear();
                m_allocator.deallocate(m_componentData);
            };

            ae_memory::AeAllocatorBase& m_allocator;
            std::unordered_map<ecs_id,T>* m_componentData;
        };

//        struct  dynamicArrayData{
//            dynamicArrayData(std::size_t t_numOfElements,ae_memory::AeAllocatorBase& t_allocator) : m_allocator{t_allocator}{
//                m_componentData = std::vector<T,ae_memory::AeAllocatorStlAdaptor<std::pair<ecs_id,T>,ae_memory::AeAllocatorBase>>(t_numOfElements, m_allocator);
//            };
//
//            ~dynamicArrayData(){
//                m_componentData->clear();
//                m_allocator.deallocate(m_componentData);
//            };
//
//            ae_memory::AeAllocatorBase& m_allocator;
//            std::unordered_map<ecs_id,T>* m_componentData;
//        };

	protected:

        /// Defines how the component data will be stored.
        ComponentStorageMethod m_componentStorageMethod;

        /// Pointer to the data the component is storing.
        void* m_componentDataPtr;

        /// Reference to the allocator used for this component.
        ae_memory::AeAllocatorBase& m_allocator;
	};

	/// When a derivative of the AeComponent class is defined the type ID will be set for the derivative class
	template <class T>
	const ecs_id AeComponent<T>::m_componentTypeId = AeComponentManager::allocateComponentTypeId<T>();
}