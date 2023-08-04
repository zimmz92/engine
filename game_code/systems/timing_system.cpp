/// \file timing_system.cpp
/// \brief The script implementing the timing system class.
/// The timing system class is implemented.

#include "timing_system.hpp"

TimingSystemClass::TimingSystemClass() : ae_ecs::AeSystem<TimingSystemClass>()  {
    m_previousTime = std::chrono::high_resolution_clock::now();
    this->enableSystem();
};

TimingSystemClass::~TimingSystemClass(){};

void TimingSystemClass::executeSystem(){
auto currentTime = std::chrono::high_resolution_clock::now();
m_timeDelta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_previousTime).count();
m_previousTime = currentTime;
};

