#if !defined(LOGGER_HELPER_H)
#define LOGGER_HELPER_H

#include <cetty/logging/Logger.h>

#define LOG(level) \
    ::cetty::logging::Logger(__FILE__, __LINE__, level).stream()

#define LOG_FATAL \
    ::cetty::logging::Logger(__FILE__, __LINE__, ::cetty::logging::LogLevel::ERROR).stream()

#define LOG_SYSFATAL \
    ::cetty::logging::Logger(__FILE__, __LINE__, true).stream()

#define LOG_ERROR \
    ::cetty::logging::Logger(__FILE__, __LINE__, ::cetty::logging::LogLevel::ERROR).stream()

#define LOG_WARN \
    ::cetty::logging::Logger(__FILE__, __LINE__, ::cetty::logging::LogLevel::WARN).stream()

#define LOG_INFO \
    ::cetty::logging::Logger(__FILE__, __LINE__, ::cetty::logging::LogLevel::INFO).stream()

#if defined(_MSC_VER)

#define LOG_DEBUG \
    ::cetty::logging::Logger(__FILE__, __LINE__, __FUNCTION__, ::cetty::logging::LogLevel::DEBUG).stream()

#define LOG_TRACE \
    ::cetty::logging::Logger(__FILE__, __LINE__, __FUNCTION__, ::cetty::logging::LogLevel::TRACE).stream()

#else

#define LOG_DEBUG \
    ::cetty::logging::Logger(__FILE__, __LINE__, __func__, ::cetty::logging::LogLevel::DEBUG).stream()

#define LOG_TRACE \
    ::cetty::logging::Logger(__FILE__, __LINE__, __func__, ::cetty::logging::LogLevel::TRACE).stream()

#endif

#define LOG_IF(level, condition) \
    !(condition) ? (void) 0 : ::cetty::logging::LogMessageVoidify() & LOG(level)

// Plus some debug-logging macros that get compiled to nothing for production
#if !defined(NDEBUG) || defined(_DEBUG)

#define DLOG(level) LOG(level)
#define DLOG_DEBUG LOG_DEBUG
#define DLOG_TRACE LOG_TRACE
#define DLOG_IF(level, condition) LOG_IF(level, condition)

#else  // NDEBUG

#define DLOG(level) \
    true ? (void) 0 : ::cetty::logging::LogMessageVoidify() & LOG(level)

#define DLOG_DEBUG \
    true ? (void) 0 : ::cetty::logging::LogMessageVoidify() & LOG_DEBUG

#define DLOG_TRACE \
    true ? (void) 0 : ::cetty::logging::LogMessageVoidify() & LOG_TRACE

#define DLOG_IF(level, condition) \
    (true || !(condition)) ? (void) 0 : ::cetty::logging::LogMessageVoidify() & LOG_IF(level, condition)

#endif  // NDEBUG


// Local Variables:
// mode: c++
// End:


#endif //#if !defined(LOGGER_HELPER_H)

// Local Variables:
// mode: c++
// End:
