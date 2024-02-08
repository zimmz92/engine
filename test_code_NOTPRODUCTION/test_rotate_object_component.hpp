/*! \file test_rotate_object_component.hpp
    \brief The script defining the world voxel component.
    The world voxel component is defined. This component defines relevant properties to objects that are world voxels.
*/
#pragma once

#include "ae_ecs_include.hpp"
#include "world_position_component.hpp"

namespace ae {

    /// This structure defines the data stored for each world voxel.
    struct TestRotationComponentStruct {
        // Defines what the angular velocity is of the object for each of the Euler Angles.
        glm::vec3 m_angularVelocity{};
    };


    /// The WorldVoxelComponent is derived from the AeComponent template class using the WorldVoxelStruct.
    class TestRotationComponent : public ae_ecs::AeComponent<TestRotationComponentStruct> {
    public:
        /// The WorldVoxelComponent constructor uses the AeComponent constructor with no additions.
        /// \param t_ecs The entity component system this component will be handled by.
        TestRotationComponent(ae_ecs::AeECS& t_ecs) : AeComponent(t_ecs,1,componentStorageMethod_unorderedMap) {};

        /// The destructor of the WorldVoxelComponent class. The WorldVoxelComponent destructor
        /// uses the AeComponent constructor with no additions.
        ~TestRotationComponent() {};

    private:

    protected:

    };
}