#include "ae_ecs.hpp"
#include "game_object_entity.hpp"
#include "position_component.hpp"

namespace ae {

	GameObjectEntity::GameObjectEntity(modelComponentStructure t_modelComponentDefinition, worldPosition t_worldPosition) : AeEntity() {
        worldPositionComponent.requiredByEntity(this->m_entityId, t_worldPosition);
        modelComponent.requiredByEntity(this->m_entityId, t_modelComponentDefinition);

		this->enableEntity();
	};

    GameObjectEntity::~GameObjectEntity() {

    };
}