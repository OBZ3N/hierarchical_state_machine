
#pragma once

#include <string>
#include "hsm/debug.h"
#include "hsm/timer.h"
#include "hsm/random.h"

namespace test
{
    class SimulatedEvent
    {
    public:
        SimulatedEvent( const std::string& event, float time_min, float time_max, float probability_min = 1.0f, float probability_max = 1.0f, bool automatic_retry = false);

        void start();
        std::string update();
        void stop();
        bool isStarted() const { return m_timer.isStarted();  }

        const std::string& getEvent() const { return m_event; }
        int getStartCount() const { return m_start_count; }
        int getStopCount() const { return m_stop_count; }

    private:
        std::string m_event;
        
        float m_time_min;
        float m_time_max;
        float m_time_for_event;
 
        float m_probability_min;
        float m_probability_max;
        float m_probability_for_event;
        float m_probability_roll;

        bool m_automatic_retry;
        int m_start_count;
        int m_stop_count;

        utils::Timer m_timer;
    };
}