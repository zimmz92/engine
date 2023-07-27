/// \file ae_component_manager.cpp
/// \brief The script implementing the component manager class.
/// The component manager class is implemented.
#include "timing_system.hpp"

TimingSystem::TimingSystem() : ae_ecs::AeSystem<TimingSystem>()  {
    m_previousTime = std::chrono::high_resolution_clock::now();
    this->enableSystem();
};

TimingSystem::~TimingSystem(){};

void TimingSystem::executeSystem(){
auto currentTime = std::chrono::high_resolution_clock::now();
m_timeDelta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_previousTime).count();
m_previousTime = currentTime;
};

