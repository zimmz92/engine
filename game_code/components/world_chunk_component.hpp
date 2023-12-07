/*! \file world_chunk_component.hpp
    \brief The script defining the world chunk component.
    The world chunk component is defined. This component defines relevant properties to objects that are world chunks.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "game_constants.hpp"
#include "world_position_component.hpp"

namespace ae {

    /// This structure defines the data stored for each world chunk.
    struct WorldChunkComponentStruct {
        ecs_id m_chunkVoxelEntityIds[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {0};
    };


    /// The WorldVoxelComponent is derived from the AeComponent template class using the WorldVoxelStruct.
    class WorldChunkComponent : public ae_ecs::AeComponent<WorldChunkComponentStruct> {
    public:
        /// The WorldVoxelComponent constructor uses the AeComponent constructor with no additions.
        /// \param t_ecs The entity component system this component will be handled by.
        WorldChunkComponent(ae_ecs::AeECS& t_ecs) : AeComponent(t_ecs) {};

        /// The destructor of the WorldVoxelComponent class. The WorldVoxelComponent destructor
        /// uses the AeComponent constructor with no additions.
        ~WorldChunkComponent() {};

    private:

    protected:

    };
}