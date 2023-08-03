#pragma once

#include "game_object_entity.hpp"
#include "game_components.hpp"

namespace ae {
	class PointLightEntity : public GameObjectEntity {

	public:
		// Function to create an entity
        explicit PointLightEntity(GameComponents* t_gameComponents);

		// Function to destroy an entity
		~PointLightEntity();

        PointLightComponentStructure* m_pointLightData;
        uboDataFlagsComponentStructure* m_uboDataFlags;

	private:

	protected:

	};

};