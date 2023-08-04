/*! \file timing_system.hpp
    \brief The script defining the timing system.

    The timing system is defined.

*/
#pragma once

#include "ae_ecs.hpp"
#include <chrono>


class TimingSystemClass : public ae_ecs::AeSystem<TimingSystemClass> {
public:
    TimingSystemClass();
    ~TimingSystemClass();

    void setupSystem() override{};
    void executeSystem() override;
    void cleanupSystem() override{};

    float getDt() const{ return m_timeDelta; };

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_previousTime;
    float m_timeDelta = 0.0f;
};