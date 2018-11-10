#pragma once

#include <stdarg.h>
#include <stdint.h>

// basic debugging utils.
namespace utils
{
    // millisecond timer / stopwatch.
    struct Timer
    {
    public:
        static uint32_t getTimeStamp();

        Timer( bool start = false );

        void start();
        void stop();
        void restart();

        void pause();
        void unpause();

        bool isStarted() const { return m_started; }
        bool isPaused() const { return m_paused; }

        void setStarted( bool value );
        void setPaused( bool value );

        uint32_t getElapsedTimeMSecs() const;
        float getElapsedTimeSecs() const;

    private:
        uint32_t m_start_timestamp;
        uint32_t m_pause_timestamp;
        uint32_t m_pause_time_accum;

        bool m_paused;
        bool m_started;
    };
}