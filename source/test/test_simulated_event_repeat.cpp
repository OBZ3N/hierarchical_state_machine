
#include "test/test_simulated_event.h"

namespace test
{
    SimulatedEvent::SimulatedEvent(const std::string& event, float time_min, float time_max, float probability_min, float probability_max, bool automatic_retry)
        : m_event(event)
        , m_time_min(time_min)
        , m_time_max(time_max)
        , m_probability_min(probability_min)
        , m_probability_max(probability_max)
        , m_time_for_event(std::numeric_limits<float>::max())
        , m_probability_for_event(0.0f)
        , m_probability_roll(std::numeric_limits<float>::max())
        , m_automatic_retry(automatic_retry)
        , m_start_count(0)
        , m_stop_count( 0 )
    {}
    
    void SimulatedEvent::stop()
    {
        if (m_timer.isStarted())
        {
            m_timer.stop();
        }
        m_stop_count++;
    }

    void SimulatedEvent::start()
    {
        m_time_for_event = debug::randomFloat(m_time_min, m_time_max);

        m_probability_for_event = debug::randomFloat(m_probability_min, m_probability_max);

        m_probability_roll = debug::randomFloat(0.0f, 1.0f);

        if (!m_timer.isStarted())
        {
            m_timer.start();
        }
        else
        {
            m_timer.restart();
        }
        m_start_count++;
    }

    bool SimulatedEvent::update()
    {
        if ( !m_timer.isStarted() )
            return false;

        // haven't rolled the dice
        if (m_probability_roll > m_probability_for_event)
            return false;

        if(m_timer.getElapsedTimeSecs() > m_time_for_event)
        {
            debug::logInfo("simulated event '%s' triggered.", m_event.c_str());
            
            m_timer.stop();

            return false;
        }
        else
        {
            return true;
        }
    }

}