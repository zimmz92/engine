/// \file timing_system.cpp
/// \brief The script implementing the timing system class.
/// The timing system class is implemented.

#include "timing_system.hpp"

namespace ae {

    // Constructor implementation
    TimingSystem::TimingSystem() : ae_ecs::AeSystem<TimingSystem>() {
        m_previousTime = std::chrono::high_resolution_clock::now();
        this->enableSystem();
    };



    // Destructor implementation
    TimingSystem::~TimingSystem() {};



    // Set up the system prior to execution. Currently not used.
    void TimingSystem::setupSystem() {};



    // Update the time difference between the current execution and the previous.
    void TimingSystem::executeSystem() {
        // Get the current time.
        auto currentTime = std::chrono::high_resolution_clock::now();

        // Calculate the difference between the current time and the last time this system executed.
        m_timeDelta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_previousTime).count();

        // Store the current execution time for reference during next execution.
        m_previousTime = currentTime;
    };



    // Clean up the system after execution. Currently not used.
    void TimingSystem::cleanupSystem() {};

}