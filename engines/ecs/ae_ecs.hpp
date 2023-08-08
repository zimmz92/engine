/// \file ae_ecs.hpp
/// \brief The script exposing the entity component system (ecs) to other systems.
/// The entity component system is shared with the world.
#pragma once

#include "ae_ecs_constants.hpp"
#include "ae_component_manager.hpp"
#include "ae_entity_manager.hpp"
#include "ae_system_manager.hpp"

namespace ae_ecs {

    class AeECS {
        template<class T> friend class AeSystem;
        friend class AeSystemBase;
        template<class T> friend class AeEntity;
        template<class T> friend class AeComponent;

    public:
        AeECS(){
        };


        ~AeECS(){
        };

        void runSystems(){
            m_ecsSystemManager.runSystems();
        }

        void destroyEntity(ecs_id t_entityId){
            m_ecsEntityManager.destroyEntity(t_entityId);
        };

    private:
        AeComponentManager m_ecsComponentManager;
        AeSystemManager m_ecsSystemManager{m_ecsComponentManager};
        AeEntityManager m_ecsEntityManager{m_ecsComponentManager};

    protected:

    };

}