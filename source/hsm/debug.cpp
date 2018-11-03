#pragma once

#include <cstdarg>
#include <string>
#include <vector>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <windows.h>
#include <wincon.h>
#include "hsm/debug.h"

namespace debug
{
    std::string formatString( const char* format, va_list args )
    {
        // reliably acquire the size from a copy of
        // the variable argument array
        // and a functionally reliable call
        // to mock the formatting
        va_list args_copy;
        va_copy( args_copy, args );
        const int iLen = std::vsnprintf( NULL, 0, format, args_copy );
        va_end( args_copy );

        // return a formatted string without
        // risking memory mismanagement
        // and without assuming any compiler
        // or platform specific behavior
        std::vector<char> zc( iLen + 1 );
        std::vsnprintf( zc.data(), zc.size(), format, args );
        
        return std::string( zc.data() );
    }

    void getLines( const std::string& message, std::vector<std::string>& lines )
    {
        std::stringstream stream( message );

        std::string item;

        while ( std::getline ( stream, item, '\n' ) ) 
        {
            lines.push_back ( item );
        }
    }


    // requires at least C++11
    std::string formatString( const char* format, ... )
    {
        // initialize use of the variable argument array
        va_list args;
        va_start( args, format );
        std::string result = formatString( format, args );
        va_end( args );

        return result;
    }
    
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

        std::string header = formatString( "%s (PID %d)", "State Machine", ::GetCurrentProcessId() );

        // initialise screen buffer to a large value.
        LOG_WIN32_API_CALL( ::SetConsoleScreenBufferSize( s_hConsole, coords ) );
        LOG_WIN32_API_CALL( ::SetConsoleTitle( header.c_str() ) );

        // Get maximum console window size allowed. 
        CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
        LOG_WIN32_API_CALL( ::GetConsoleScreenBufferInfo( s_hConsole, &screenBufferInfo ) );

        // set screen buffer to x4 screen size.
        coords.X = screenBufferInfo.dwMaximumWindowSize.X;
        coords.Y = screenBufferInfo.dwMaximumWindowSize.Y * 4;
        LOG_WIN32_API_CALL( ::SetConsoleScreenBufferSize( s_hConsole, coords ) );

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

    bool showErrorDialog( LogLevel level, const std::string& message )
    {
        UINT style = 
            ( level == LogLevel::SanityCheck || level == LogLevel::Fatal ) ?  ( MB_OK | MB_ICONERROR | MB_APPLMODAL ) :
            ( MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION | MB_APPLMODAL );

        std::string header = LogLevelToString( level );

        int result = MessageBox( GetConsoleWindow(), message.c_str(), header.c_str(), style );

        bool skippError = false;

        switch ( result )
        {
        default:
            {
                break;
            }
        case IDOK:
            {
                abort();
                break;
            }
        case IDABORT:
            {
                DebugBreak();
                skippError = false;
                break;
            }
        case IDRETRY:
            {
                skippError = false;
                break;
            }
        case  IDIGNORE:
            {
                skippError = true;
                break;
            }
        }
        return skippError;
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
        case LogLevel::SanityCheck: return "SanityCheck";
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

    void assertArgs( bool& skipAssert, bool condition, const char* condition_string, LogLevel level, const char* file, int line, const char* format, ... )
    {
        if ( skipAssert )
            return;

        if ( condition )
            return;

        va_list args;
        va_start( args, format );
        std::string message = formatString( format, args );
        va_end( args );

        std::ostringstream stream;
        stream  << "***ASSERT***" << std::endl
                << "level     : " << LogLevelToString( level ) << std::endl
                << "condition : " << condition_string << std::endl
                << "file      : " << file << std::endl
                << "message   : " << message << std::endl;

        logDebug( level, stream.str().c_str() );

        skipAssert = showErrorDialog( level, stream.str() );
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
        // text color.
        SetTextAttribute( level );

        // timestamp column
        int timestamp = GetTimeStamp();
        int ms = GetTimeStamp() % 1000;
        int sec = (GetTimeStamp() / 1000) % 60;
        int min = (GetTimeStamp() / 60000) % 60;
        int hour = (GetTimeStamp() / 3600000) % 24;

        std::string message_string = formatString( format, args );

        std::string timestamp_string = formatString( "%02u:%02u:%02u.%03u   ", hour, min, sec, ms );

        std::string level_string = formatString( "%7s   ", LogLevelToString( level ) );

        std::vector<std::string> lines;

        getLines( message_string, lines );
        
        for ( size_t i = 0; i < lines.size(); ++i )
        {
            const std::string& line = lines[ i ];

            if ( i == 0 )
            {
                std::cout << timestamp_string << level_string << line << std::endl;
            }
            else
            {
                std::cout << "                         " << line << std::endl;
            }
        }

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