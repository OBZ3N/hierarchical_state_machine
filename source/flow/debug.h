#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

// basic debugging utils.
namespace debug
{
    enum class LogLevel
    {
        Trace,          // spam trace.
        Info,           // debug info.
        Warning,        // warning, recoverable.
        Error,          // error, recoverable.
        Fatal,          // error, not recoverable.
        SanityCheck,    // programmatical error. not recoverable.
        Count
    };

    extern void randomSeed(unsigned int seed);

    extern float randomFloat();

    extern float randomFloat(float min, float max);

    template<typename type> type random(int a, int b) 
    { 
        float rnd = rand() / (float)RAND_MAX;
        return (type) (a + rnd * (b - a));
    }

    struct Timer
    {
    public:
        Timer(bool start=false);

        void start();
        void stop();
        void restart();
        
        void pause();
        void unpause();

        bool isStarted() const { return m_started; }
        bool isPaused() const { return m_paused; }

        void setStarted(bool value);
        void setPaused(bool value);
        
        uint32_t getElapsedTimeMSecs() const;
        float getElapsedTimeSecs() const;
    
    private:
        uint32_t m_start_timestamp;
        uint32_t m_pause_timestamp;
        uint32_t m_pause_time_accum;
        
        bool m_paused;
        bool m_started;
    };
    
    extern void initialise();
    extern int GetTimeStamp();
    extern const char* LogLevelToString(LogLevel level);

    extern void assertArgs(bool condition, const char* condition_string, LogLevel level, const char* file, int line, const char* format, ...);
    extern void assertArgs( bool condition, const char* condition_string, LogLevel level, const char* file, int line, const char* format, va_list args );

    extern void logDebug(LogLevel level, const char* format, ...);
    extern void logDebugArgs(LogLevel level, const char* format, va_list args);

    extern void logTrace(const char* format, ...);
    extern void logInfo(const char* format, ...);
    extern void logWarning(const char* format, ...);
    extern void logError(const char* format, ...);
    extern void logFatal(const char* format, ...);
    extern void logSanityCheck(const char* format, ...);
}

#if defined(_DEBUG)
    #define ASSERT_SANITY(CONDITION, FORMAT, ...) debug::assertArgs(CONDITION, #CONDITION, debug::LogLevel::SanityCheck, __FILE__, __LINE__, FORMAT, __VA_ARGS__)
    #define ASSERT_FATAL(CONDITION, FORMAT, ...) debug::assertArgs(CONDITION, #CONDITION, debug::LogLevel::Fatal, __FILE__, __LINE__, FORMAT, __VA_ARGS__)
    #define ASSERT_WARNING(CONDITION, FORMAT, ...) debug::assertArgs(CONDITION, #CONDITION, debug::LogLevel::Warning, __FILE__, __LINE__, FORMAT, __VA_ARGS__)
#else
    #define ASSERT_SANITY(CONDITION, FORMAT, ...)
    #define ASSERT_FATAL(CONDITION, FORMAT, ...)
    #define ASSERT_WARNING(CONDITION, FORMAT, ...)
#endif