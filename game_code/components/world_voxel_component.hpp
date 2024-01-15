/*! \file world_voxel_component.hpp
    \brief The script defining the world voxel component.
    The world voxel component is defined. This component defines relevant properties to objects that are world voxels.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "world_position_component.hpp"

namespace ae {

    enum WorldVoxelType{
        WorldVoxelType_Void = 0,
        WorldVoxelType_Air,
        WorldVoxelType_Dirt,
        WorldVoxelType_Sand,
        WorldVoxelType_Water,
        WorldVoxelType_Stone
    };

    /// This structure defines the data stored for each world voxel.
    struct WorldVoxelComponentStruct {
        // This is to flag if the voxel should be rendered. This would be probably be set to false for an air or void
        // type voxel.
        bool m_render = false;

        // Defines what type of voxel this is.
        WorldVoxelType m_voxelType;
    };


    /// The WorldVoxelComponent is derived from the AeComponent template class using the WorldVoxelStruct.
    class WorldVoxelComponent : public ae_ecs::AeComponent<WorldVoxelComponentStruct> {
    public:
        /// The WorldVoxelComponent constructor uses the AeComponent constructor with no additions.
        /// \param t_ecs The entity component system this component will be handled by.
        WorldVoxelComponent(ae_ecs::AeECS& t_ecs) : AeComponent(t_ecs,1,componentStorageMethod_unorderedMap) {};

        /// The destructor of the WorldVoxelComponent class. The WorldVoxelComponent destructor
        /// uses the AeComponent constructor with no additions.
        ~WorldVoxelComponent() {};

    private:

    protected:

    };
}