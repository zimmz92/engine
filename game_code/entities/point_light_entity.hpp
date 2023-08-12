/*! \file point_light_entity.hpp
    \brief The script defining the point light entity.
    The point light entity is defined.
*/
#pragma once

#include "game_object_entity.hpp"
#include "game_components.hpp"

namespace ae {

    /// Defines entities that are point lights which are derivatives of game object entities.
	class PointLightEntity : public GameObjectEntity {

	public:
        /// Creates the PointLightEntity using the specified game components.
        explicit PointLightEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents);

        /// The destructor of the PointLightEntity.
		~PointLightEntity();

        /// A reference to the point light data of the entity.
        PointLightComponentStruct& m_pointLightData;

        /// Specifies if the point light entity has data for the ubo.
        UboDataFlagsComponentStruct& m_uboDataFlags;

	private:

	protected:

	};

};