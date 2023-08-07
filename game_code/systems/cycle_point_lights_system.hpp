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
    class CyclePointLightsSystem : public ae_ecs::AeSystem<CyclePointLightsSystem> {
    public:
        /// Constructor of the CyclePointLightSystemClass
        /// \param t_game_components The game components available that this system may require.
        /// \param t_timingSystem The TimingSystem the CyclePointLightSystemClass will depend on executing first
        /// and require information from.
        CyclePointLightsSystem(GameComponentsStruct& t_game_components, TimingSystem& t_timingSystem);

        /// Destructor of the CyclePointLightSystemClass
        ~CyclePointLightsSystem();

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
        WorldPositionComponent& m_worldPositionComponent;
        /// The PointLightComponentOld this systems accesses to update the position of applicable point light entities.
        PointLightComponent& m_PointLightComponent;


        // Prerequisite systems for the PlayerInputSystem.
        /// The TimingSystem this system requires to operate prior to it's own operation.
        TimingSystem& m_timingSystem;

        /// Number of point lights being updated
        int m_numPointLights;
    };
}


