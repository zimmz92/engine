/*! \file cycle_point_lights_system.hpp
    \brief The script defining the system that cycles the point lights.

    The cycle point light system is defined.

*/
#pragma once

#include "ae_ecs.hpp"

#include "game_components.hpp"

#include "timing_system.hpp"


namespace ae {

    /// A temporary system that cycles the point lights in the world for fun.
    class CyclePointLightsSystemClass : public ae_ecs::AeSystem<CyclePointLightsSystemClass> {
    public:
        /// Constructor of the CyclePointLightSystemClass
        /// \param t_game_components The game components available that this system may require.
        /// \param t_timingSystem The TimingSystemClass the CyclePointLightSystemClass will depend on executing first
        /// and require information from.
        CyclePointLightsSystemClass(GameComponentsStruct& t_game_components, TimingSystemClass& t_timingSystem);

        /// Destructor of the CyclePointLightSystemClass
        ~CyclePointLightsSystemClass();

        /// Setup the CyclePointLightSystemClass, this is handled by the ECS.
        void setupSystem() override;

        /// Execute the CyclePointLightSystemClass, this is handled by the ECS.
        void executeSystem() override;

        /// Clean up the CyclePointLightSystemClass, this is handled by the ECS.
        void cleanupSystem() override;

        /// Returns the number of point lights being handled by this system
        int getNumPointLights() const {return m_numPointLights; };

    private:

        // Components this system utilizes.
        /// The WorldPositionComponent this systems accesses to update the position of applicable point light entities.
        WorldPositionComponentClass& m_worldPositionComponent;
        /// The PointLightComponent this systems accesses to update the position of applicable point light entities.
        PointLightComponentClass& m_PointLightComponent;


        // Prerequisite systems for the PlayerInputSystemClass.
        /// The TimingSystemClass this system requires to operate prior to it's own operation.
        TimingSystemClass& m_timingSystem;

        /// Number of point lights being updated
        int m_numPointLights;
    };
}


