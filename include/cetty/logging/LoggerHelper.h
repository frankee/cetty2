#if !defined(LOGGER_HELPER_H)
#define LOGGER_HELPER_H

#include <cetty/logging/LoggerHelperPrivate.h>

#define LOG(logger, priority, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(priority, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define LOG_ERROR(logger, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(InternalLogLevel::ERROR, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define LOG_WARN(logger, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(InternalLogLevel::WARN, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define LOG_INFO(logger, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(InternalLogLevel::INFO, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define LOG_DEBUG(logger, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(InternalLogLevel::DEBUG, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define LOG_MARK_FUNCTION(logger, funname) \
    ::cetty::logging::internal::LogMessageMarker(logger, __FILE__, __LINE__, funname)

#define LOG_IF(logger, priority, condition, msg, ...) \
    !(condition) ? (void) 0 : ::cetty::logging::internal::LoggerWrapper(logger).log(priority, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define LOG_EVERY_N(logger, priority, n, msg, ...) \
    INTERNAL_LOG_CONDITION_EVERY_N(n) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(priority, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define LOG_IF_EVERY_N(logger, priority, condition, n, msg, ...) \
    INTERNAL_LOG_CONDITION_IF_EVERY_N(condition, n)\
    ::cetty::logging::internal::LoggerWrapper(logger).log(priority, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define LOG_FIRST_N(logger, priority, n, msg, ...) \
    INTERNAL_LOG_CONDITION_FIRST_N(n) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(priority, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define LOG_START_TIMER(logger) \
    ::cetty::logging::internal::LoggerWrapper(logger).startTimer(__FILE__, __LINE__)

#define LOG_CHECK_TIMER(logger, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).checkTimer(__FILE__, __LINE__, msg, ##__VA_ARGS__)


// Plus some debug-logging macros that get compiled to nothing for production
#if !defined(NDEBUG) || defined(_DEBUG)

#define DLOG(logger, priority, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(priority, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define DLOG(logger, priority, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(priority, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define DLOG_ERROR(logger, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(InternalLogLevel::ERROR, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define DLOG_WARN(logger, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(InternalLogLevel::WARN, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define DLOG_INFO(logger, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(InternalLogLevel::INFO, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define DLOG_DEBUG(logger, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(InternalLogLevel::DEBUG, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define DLOG_IF(logger, priority, condition, msg, ...) \
    !(condition) ? (void) 0 : ::cetty::logging::internal::LoggerWrapper(logger).log(priority, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define DLOG_EVERY_N(logger, priority, n, msg, ...) \
    INTERNAL_LOG_CONDITION_EVERY_N(n) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(priority, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define DLOG_IF_EVERY_N(logger, priority, condition, n, msg, ...) \
    INTERNAL_LOG_CONDITION_IF_EVERY_N(condition, n)\
    ::cetty::logging::internal::LoggerWrapper(logger).log(priority, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define DLOG_FIRST_N(logger, priority, n, msg, ...) \
    INTERNAL_LOG_CONDITION_FIRST_N(n) \
    ::cetty::logging::internal::LoggerWrapper(logger).log(priority, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define DLOG_START_TIMER(logger) \
    ::cetty::logging::internal::LoggerWrapper(logger).startTimer(__FILE__, __LINE__)

#define DLOG_CHECK_TIMER(logger, msg, ...) \
    ::cetty::logging::internal::LoggerWrapper(logger).checkTimer(__FILE__, __LINE__, msg, ##__VA_ARGS__)

#else  // NDEBUG

#define DLOG(logger, priority, msg, ...)
#define DLOG_ERROR(logger, msg, ...)
#define DLOG_WARN(logger, msg, ...)
#define DLOG_INFO(logger, msg, ...)
#define DLOG_DEBUG(logger, msg, ...)
#define DLOG_IF(logger, priority, condition, msg, ...)
#define DLOG_EVERY_N(logger, priority, n, msg, ...)
#define DLOG_IF_EVERY_N(logger, priority, condition, n, msg, ...)
#define DLOG_FIRST_N(logger, priority, n, msg, ...)

#define DLOG_START_TIMER(logger)
#define DLOG_CHECK_TIMER(logger, msg, ...)

#endif  // NDEBUG

// Local Variables:
// mode: c++
// End:


#endif //#if !defined(LOGGER_HELPER_H)

// Local Variables:
// mode: c++
// End:

