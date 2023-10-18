/// \file timing_system.cpp
/// \brief The script implementing the timing system class.
/// The timing system class is implemented.

#include "timing_system.hpp"
#include <iostream>

namespace ae {

    // Constructor implementation
    TimingSystem::TimingSystem(ae_ecs::AeECS& t_ecs) : ae_ecs::AeSystem<TimingSystem>(t_ecs) {
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


#ifdef FPS_DEBUG
        // Time delta simple moving average
        if(m_numSamples < m_timeDeltaSamples){
            m_timeDeltaSMA = (m_timeDelta+m_numSamples*m_timeDeltaSMA)/(m_numSamples+1);
            m_timeDeltaFIFO.push(m_timeDelta);
            m_numSamples++;
        }else{
            m_timeDeltaSMA = m_timeDeltaSMA + (1.0/m_timeDeltaSamples)*(m_timeDelta-m_timeDeltaFIFO.front());
            m_timeDeltaFIFO.pop();
            m_timeDeltaFIFO.push(m_timeDelta);

            // Check if it is the min.
            if(m_timeDelta<m_timeDeltaMin){
                m_timeDeltaMin=m_timeDelta;
            }

            // Check if it is the max.
            if(m_timeDelta>m_timeDeltaMax){
                m_timeDeltaMax=m_timeDelta;
            }
        }

        std::cout << "fps avg: " << 1.0/m_timeDeltaSMA << std::endl;
        std::cout << "fps min: " << 1.0/m_timeDeltaMax << std::endl;
        std::cout << "fps max: " << 1.0/m_timeDeltaMin << std::endl;
#endif
    };



    // Clean up the system after execution. Currently not used.
    void TimingSystem::cleanupSystem() {};

}