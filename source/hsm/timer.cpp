
#include <time.h>
#include <windows.h>
#include "hsm/timer.h"
#include "hsm/debug.h"

namespace utils
{
    static uint32_t s_timestamp_origin = 0;

    uint32_t Timer::getTimeStamp()
    {
        uint32_t ms = GetTickCount();

        if ( s_timestamp_origin == 0 )
        {
            s_timestamp_origin = ms;
        }

        return ( ms - s_timestamp_origin );
    }

    Timer::Timer(bool start)
    {
        m_paused = false;
        m_started = false;
        setStarted(start);
    }

    void Timer::start()
    {
        if (!m_started)
        {
            m_paused = false;
            m_started = true;
            m_pause_time_accum = 0;
            m_start_timestamp = utils::Timer::getTimeStamp();
        }
    }

    void Timer::restart()
    {
        if(m_started)
            stop();

        start();
    }

    void Timer::stop()
    {
        m_started = false;
        m_paused = false;
    }

    void Timer::pause()
    {
        if (m_started && !m_paused)
        {
            m_paused = true;
            m_pause_time_accum = 0;
            m_pause_timestamp = utils::Timer::getTimeStamp();
        }
    }
    void Timer::unpause()
    {
        if (m_started && m_paused)
        {
            m_paused = false;
            m_pause_time_accum = utils::Timer::getTimeStamp() - m_pause_timestamp;
        }
    }

    void Timer::setStarted(bool value)
    {
        if (value && !m_started)
        {
            start();
        }
        else if (!value && m_started)
        {
            stop();
        }
    }

    void Timer::setPaused(bool value)
    {
        if (m_started && value && !m_paused)
        {
            pause();
        }
        else if (m_started && !value && m_paused)
        {
            unpause();
        }
    }

    uint32_t Timer::getElapsedTimeMSecs() const
    {
        if (!m_started)
        {
            return 0;
        }
        else
        {
            uint32_t current_elapsed_time = (m_paused) ? (m_pause_timestamp - m_start_timestamp) : ( utils::Timer::getTimeStamp() - m_start_timestamp);

            //ASSERT_FATAL((current_elapsed_time > m_pause_time_accum), "current_elapsed_time(%u) > m_pause_time_accum(%u).", current_elapsed_time, m_pause_time_accum);

            uint32_t total_elapsed_time = (current_elapsed_time - m_pause_time_accum);

            return total_elapsed_time;
        }
    }

    float Timer::getElapsedTimeSecs() const
    {
        return getElapsedTimeMSecs() / 1000.0f;
    }
}
