/*! \file timing_system.hpp
    \brief The script defining the timing system.
    The timing system is defined.
*/
#pragma once

#include "ae_ecs.hpp"
#include <chrono>

namespace ae {

/// Tracks the time between runs.
    class TimingSystem : public ae_ecs::AeSystem<TimingSystem> {
    public:
        /// Constructor of the TimingSystem
        TimingSystem();

        /// Destructor of the TimingSystem
        ~TimingSystem();

        /// Setup the TimingSystem, this is handled by the ECS.
        void setupSystem() override;

        /// Execute the TimingSystem, this is handled by the ECS.
        void executeSystem() override;

        /// Clean up the TimingSystem, this is handled by the ECS.
        void cleanupSystem() override;

        /// Get the change in time since the system last ran.
        /// \return Returns the time delta between when this system ran last and the current run of the system.
        float getDt() const { return m_timeDelta; };

    private:

        /// Keeps track of the what time it was the last time this system executed.
        std::chrono::time_point<std::chrono::high_resolution_clock> m_previousTime;

        /// Stores the amount of time that has passed between the current and previous executions of this system.
        float m_timeDelta = 0.0f;
    };
}