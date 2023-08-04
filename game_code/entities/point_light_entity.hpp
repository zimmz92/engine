#pragma once

#include "game_object_entity.hpp"
#include "game_components.hpp"

namespace ae {
	class PointLightEntity : public GameObjectEntity {

	public:
		// Function to create an entity
        explicit PointLightEntity(GameComponentsStruct* t_gameComponents);

		// Function to destroy an entity
		~PointLightEntity();

        PointLightComponentStruct* m_pointLightData;
        UboDataFlagsComponentStruct* m_uboDataFlags;

	private:

	protected:

	};

};