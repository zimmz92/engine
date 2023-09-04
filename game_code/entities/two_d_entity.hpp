/*! \file two_d_entity.hpp
    \brief The script defining the 2D game object entity.
    The 2D game object entity is defined.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "game_components.hpp"

namespace ae {

/// Defines entities that are used for the UI.
class TwoDEntity : public ae_ecs::AeEntity<TwoDEntity> {

	public:
        /// Creates the TwoDEntity using the specified game components.
		explicit TwoDEntity(ae_ecs::AeECS& t_ecs, GameComponents& t_gameComponents);

        /// The destructor of the TwoDEntity.
		~TwoDEntity();

        /// A reference to the 2D model data of the entity.
        Model2dComponentStruct& m_model;

	private:

	protected:
	};

};