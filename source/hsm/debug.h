#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>

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

    extern void initialise();
    extern const char* LogLevelToString(LogLevel level);

    extern std::string formatString( const char* format, va_list args );
    extern std::string formatString( const char* format, ... );
    
    extern void assertArgs(bool& skipAssert, bool condition, const char* condition_string, LogLevel level, const char* file, int line, const char* format, ...);
    
    extern void logDebugArgs( LogLevel level, const char* format, va_list args );
    extern void logDebug(LogLevel level, const char* format, ...);

    extern void logTrace(const char* format, ...);
    extern void logInfo(const char* format, ...);
    extern void logWarning(const char* format, ...);
    extern void logError(const char* format, ...);
    extern void logFatal(const char* format, ...);
    extern void logSanityCheck(const char* format, ...);
}

#if defined(_DEBUG)
    #define ASSERT_SANITY(CONDITION, FORMAT, ...) { static bool s_skipAssert = false; debug::assertArgs(s_skipAssert, CONDITION, #CONDITION, debug::LogLevel::SanityCheck, __FILE__, __LINE__, FORMAT, __VA_ARGS__); }
    #define ASSERT_FATAL(CONDITION, FORMAT, ...) { static bool s_skipAssert = false; debug::assertArgs(s_skipAssert, CONDITION, #CONDITION, debug::LogLevel::Fatal, __FILE__, __LINE__, FORMAT, __VA_ARGS__); }
    #define ASSERT_ERROR(CONDITION, FORMAT, ...) { static bool s_skipAssert = false; debug::assertArgs(s_skipAssert, CONDITION, #CONDITION, debug::LogLevel::Error, __FILE__, __LINE__, FORMAT, __VA_ARGS__); }
    #define ASSERT_WARNING(CONDITION, FORMAT, ...) { static bool s_skipAssert = false; debug::assertArgs(s_skipAssert, CONDITION, #CONDITION, debug::LogLevel::Warning, __FILE__, __LINE__, FORMAT, __VA_ARGS__); }
#else
    #define ASSERT_SANITY(CONDITION, FORMAT, ...)
    #define ASSERT_FATAL(CONDITION, FORMAT, ...)
    #define ASSERT_ERROR(CONDITION, FORMAT, ...)
    #define ASSERT_WARNING(CONDITION, FORMAT, ...)
#endif