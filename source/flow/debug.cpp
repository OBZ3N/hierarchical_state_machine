#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <windows.h>
#include <wincon.h>
#include "flow/debug.h"

namespace debug
{
    void randomSeed(unsigned int seed)
    {
        srand(seed);
    }

    extern float randomFloat()
    {
        float rnd = rand() / (float)RAND_MAX;
        return rnd;
    }

    float randomFloat( float min, float max )
    {
        return min + randomFloat() * ( max - min );
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
            m_start_timestamp = GetTimeStamp();
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
            m_pause_timestamp = GetTimeStamp();
        }
    }
    void Timer::unpause()
    {
        if (m_started && m_paused)
        {
            m_paused = false;
            m_pause_time_accum = GetTimeStamp() - m_pause_timestamp;
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
            uint32_t current_elapsed_time = (m_paused) ? (m_pause_timestamp - m_start_timestamp) : (GetTimeStamp() - m_start_timestamp);

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

namespace debug
{
    static bool s_initialised = false;
    static int s_timestamp_origin = 0;
    static HANDLE s_hConsole = 0;

    #define LOG_WIN32_API_CALL(EXP) logWin32ApiCall(#EXP, (EXP) != 0);

    void logWin32ApiCall(const char* call_descriptor, bool call_result)
    {
        if (!call_result)
        {
            logError("%s failed (GetLastError() = 0x%08x).", call_descriptor, ::GetLastError());
        }
    }

    void initialise()
    {
        if (s_initialised)
            return;

        s_initialised = true;
        s_timestamp_origin = GetTimeStamp();

        s_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        COORD coords;
        coords.X = 300;
        coords.Y = 300;

        char buffer[512];
        snprintf(buffer, sizeof(buffer), "%s (PID %d)", "State Machine", ::GetCurrentProcessId());

        // initialise screen buffer to a large value.
        LOG_WIN32_API_CALL(::SetConsoleScreenBufferSize(s_hConsole, coords));
        LOG_WIN32_API_CALL(::SetConsoleTitle(buffer));

        // Get maximum console window size allowed. 
        CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
        LOG_WIN32_API_CALL(::GetConsoleScreenBufferInfo(s_hConsole, &screenBufferInfo));

        // set screen buffer to x4 screen size.
        coords.X = screenBufferInfo.dwMaximumWindowSize.X;
        coords.Y = screenBufferInfo.dwMaximumWindowSize.Y * 4;
        LOG_WIN32_API_CALL(::SetConsoleScreenBufferSize(s_hConsole, coords));

        // Set buffer window size to maximum allowed.
        SMALL_RECT rect;
        rect.Top = 0;
        rect.Left = 0;
        rect.Right = screenBufferInfo.dwMaximumWindowSize.X - 1;
        rect.Bottom = screenBufferInfo.dwMaximumWindowSize.Y - 20;
        LOG_WIN32_API_CALL(::SetConsoleWindowInfo(s_hConsole, true, &rect));

        // Change console window font.
        CONSOLE_FONT_INFOEX cfi;
        cfi.cbSize = sizeof(cfi);
        cfi.nFont = 0;
        cfi.dwFontSize.X = 0;       // Width of each character in the font
        cfi.dwFontSize.Y = 14;      // Height
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_NORMAL;
        wcscpy_s(cfi.FaceName, sizeof(cfi.FaceName) / sizeof(*cfi.FaceName), L"Consolas"); // Choose your font
        LOG_WIN32_API_CALL(::SetCurrentConsoleFontEx(s_hConsole, FALSE, &cfi));
    }

    const char* LogLevelToString(LogLevel level)
    {
        switch (level)
        {
        default:                    return "?????";
        case LogLevel::Trace:       return "Trace";
        case LogLevel::Info:        return "Info";
        case LogLevel::Warning:     return "Warning";
        case LogLevel::Error:       return "Error";
        case LogLevel::Fatal:       return "Fatal";
        case LogLevel::SanityCheck: return "Sanity";
        }
    }

    int LogLevelToTextAttribute(LogLevel level)
    {
        switch (level)
        {
            default:                        return                        FOREGROUND_RED   | FOREGROUND_BLUE  | FOREGROUND_GREEN;
            case LogLevel::Trace:           return FOREGROUND_INTENSITY | FOREGROUND_BLUE  ;
            case LogLevel::Info:            return FOREGROUND_INTENSITY | FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE ;
            case LogLevel::Warning:         return FOREGROUND_INTENSITY | FOREGROUND_RED   | FOREGROUND_GREEN;
            case LogLevel::Error:           return FOREGROUND_INTENSITY | FOREGROUND_RED   | FOREGROUND_BLUE ;
            case LogLevel::Fatal:           return FOREGROUND_INTENSITY | FOREGROUND_RED   ;
            case LogLevel::SanityCheck:     return FOREGROUND_INTENSITY | FOREGROUND_RED   ;
        }
    }

    void SetTextAttribute(LogLevel level)
    {
        initialise();

        int attrib = LogLevelToTextAttribute(level);

        SetConsoleTextAttribute(s_hConsole, attrib);
    }

    int GetTimeStamp()
    {
        int ms = GetTickCount();

        return (ms - s_timestamp_origin);
    }

    void assertArgs( bool condition, const char* condition_string, LogLevel level, const char* file, int line, const char* format, ... )
    {
        va_list args;
        va_start( args, format );
        assertArgs( condition, condition_string, level, file, line, format, args );
        va_end( args );
    }

    void assertArgs(bool condition, const char* condition_string, LogLevel level, const char* file, int line, const char* format, va_list args)
    {
        if (condition)
            return;

        char message[2048];
        snprintf(message, sizeof(message), 
            "***ASSERT***\n"
            "level     : %s\n"
            "condition : %s\n"
            "file      : %s\n"
            "line      : %d\n"
            "message   : %s\n",
            LogLevelToString(level),
            condition_string,
            file,
            line,
            format);

        logDebug(level, message, args);
    }

    void logDebug(LogLevel level, const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logDebugArgs(level, format, args);
        va_end(args);
    }

    void logDebugArgs(LogLevel level, const char* format, va_list args)
    {
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);

        // text color.
        SetTextAttribute(level);

        // timestamp column
        int timestamp = GetTimeStamp();
        int ms = GetTimeStamp() % 1000;
        int sec = (GetTimeStamp() / 1000) % 60;
        int min = (GetTimeStamp() / 60000) % 60;
        int hour = (GetTimeStamp() / 3600000) % 24;

        printf("%02u:%02u:%02u.%03u   ", hour, min, sec, ms);

        // level
        printf("%7s   ", LogLevelToString(level));

        // message.
        printf(buffer);

        // next-line, if required.
        if (buffer[strlen(buffer) - 1] != '\n')
            printf("\n");

        // back to default text color.
        SetTextAttribute(LogLevel::Info);
    }

    void logTrace(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logDebugArgs(LogLevel::Trace, format, args);
        va_end(args);
    }

    void logInfo(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logDebugArgs(LogLevel::Info, format, args);
        va_end(args);
    }

    void logWarning(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logDebugArgs(LogLevel::Warning, format, args);
        va_end(args);
    }

    void logError(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logDebugArgs(LogLevel::Error, format, args);
        va_end(args);
    }

    void logFatal(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logDebugArgs(LogLevel::Fatal, format, args);
        va_end(args);
    }

    void logSanityCheck(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logDebugArgs(LogLevel::SanityCheck, format, args);
        va_end(args);
    }
}