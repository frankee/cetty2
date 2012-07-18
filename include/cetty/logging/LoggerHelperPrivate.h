#if !defined(LOGGER_HELPER_PRIVATE_H)
#define LOGGER_HELPER_PRIVATE_H

#include <assert.h>
#include <string>

#include <cetty/logging/InternalLogger.h>
#include <cetty/logging/InternalLogLevel.h>

#ifdef _MSC_VER
# pragma warning(disable: 4996)
#endif

namespace cetty {
namespace logging {
namespace internal {
using namespace cetty::logging;

class Timer;
class LoggerWrapper {
public:
    LoggerWrapper(InternalLogger* logger) : logger(logger), timer(NULL) {}
    ~LoggerWrapper();

    void log(InternalLogLevel level, const char* file, int line, const char* format, ...);

    void startTimer(const char* file, int line);
    void checkTimer(const char* file, int line, const char* format, ...);

private:
    InternalLogger* logger;
    Timer* timer;
};

struct LogMessageMarker {
    LogMessageMarker(InternalLogger* logger, const char* file, int line, const char* funname)
        : loggerWapper(logger), file(file), line(line), funname(funname) {
        if (funname) {
            loggerWapper.log(InternalLogLevel::INFO, file, line, "-->> Entering function:%s", funname);
        }
    }
    ~LogMessageMarker() {
        if (funname) {
            loggerWapper.log(InternalLogLevel::INFO, file, line, "<<-- Leaving function:%s", funname);
        }
    }

private:
    LoggerWrapper loggerWapper;

    int         line;
    const char* file;
    const char* funname;
};
}
}
}

#define INTERNAL_LOG_CONDITION_EVERY_N(n) \
    static int LOG_OCCURRENCES = 0, LOG_OCCURRENCES_MOD_N = 0; \
    ++LOG_OCCURRENCES; \
    if (++LOG_OCCURRENCES_MOD_N > n) LOG_OCCURRENCES_MOD_N -= n; \
    if (LOG_OCCURRENCES_MOD_N == 1)

#define INTERNAL_LOG_CONDITION_IF_EVERY_N(condition, n) \
    static int LOG_OCCURRENCES = 0, LOG_OCCURRENCES_MOD_N = 0; \
    ++LOG_OCCURRENCES; \
    if (condition && \
            ((LOG_OCCURRENCES_MOD_N=(LOG_OCCURRENCES_MOD_N + 1) % n) == (1 % n)))

#define INTERNAL_LOG_CONDITION_FIRST_N(n) \
    static int LOG_OCCURRENCES = 0; \
    if (LOG_OCCURRENCES <= n) \
        ++LOG_OCCURRENCES; \
    if (LOG_OCCURRENCES <= n)

#endif //#if !defined(LOGGER_HELPER_PRIVATE_H)

// Local Variables:
// mode: c++
// End:
