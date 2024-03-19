/// \file ae_ecs.hpp
/// \brief The script exposing the entity component system (ecs) to other systems.
/// The entity component system is shared with the world.
#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_component_manager.hpp"
#include "ae_entity_manager.hpp"
#include "ae_system_manager.hpp"

#include "ae_allocator_base.hpp"
#include "ae_de_stack_allocator.hpp"

namespace ae_ecs {

    class AeECS {
        template<class T> friend class AeSystem;
        friend class AeSystemBase;
        template<class T> friend class AeEntity;
        template<class T> friend class AeComponent;
        friend class AeComponentBase;

    public:
        AeECS(ae_memory::AeDeStackAllocator& t_deStackAllocator,ae_memory::AeAllocatorBase& t_freeListAllocator) :
        m_deStackAllocator{t_deStackAllocator},
        m_freeListAllocator{t_freeListAllocator}{};

        ~AeECS()= default;

        void runSystems(){
            m_ecsSystemManager.runSystems();
        }

        void destroyEntity(ecs_id t_entityId){
            m_ecsEntityManager.destroyEntity(t_entityId);
        };

        void destroyAllEntities(){
            m_ecsEntityManager.destroyAllEntities();
        }

    private:

        ae_memory::AeDeStackAllocator& m_deStackAllocator;
        ae_memory::AeAllocatorBase& m_freeListAllocator;

        AeComponentManager m_ecsComponentManager{};
        AeSystemManager m_ecsSystemManager{m_ecsComponentManager};
        AeEntityManager m_ecsEntityManager{m_ecsComponentManager};

    protected:

    };

}