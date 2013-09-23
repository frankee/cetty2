#include <cetty/logging/LogFileSink.h>

#include <ctime>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace cetty {
namespace logging {

using namespace boost::this_thread;
using namespace boost::posix_time;

static const int DEFAULT_BUFFER_SIZE = 10 * 1024;
static const int DEFAULT_ROLL_SIZE = 1024 * 1024 * 128;
static const int DAILY_CYCLE = 24 * 60 * 60;

LogFileSink::RollingSchedule LogFileSink::RollingSchedule::ROLLING_NONE(0, "NONE");
LogFileSink::RollingSchedule LogFileSink::RollingSchedule::ROLLING_FILE_SIZE(1, "FILE_SIZE");
LogFileSink::RollingSchedule LogFileSink::RollingSchedule::ROLLING_MONTHLY(2, "MONTHLY");
LogFileSink::RollingSchedule LogFileSink::RollingSchedule::ROLLING_WEEKLY(3, "WEEKLY");
LogFileSink::RollingSchedule LogFileSink::RollingSchedule::ROLLING_TWICE_DAILY(4, "TWICE_DAILY");
LogFileSink::RollingSchedule LogFileSink::RollingSchedule::ROLLING_DAILY(5, "DAILY");
LogFileSink::RollingSchedule LogFileSink::RollingSchedule::ROLLING_HOURLY(6, "HOURLY");
LogFileSink::RollingSchedule LogFileSink::RollingSchedule::ROLLING_MINUTELY(7, "MINUTELY");

LogFileSink::RollingSchedule::RollingSchedule(int value, const char* name)
    : cetty::util::Enum<RollingSchedule>(value, name) {
    if (needSetDefaultEnum()) {
        setDefaultEnum(new RollingSchedule(-1, "UNKNOWN"));
    }
}

LogFileSink::LogFileSink(const std::string& baseName)
    :LogSink("LogFileSink"),
     fp_(NULL),
     buffer_(NULL),
     schedule_(RollingSchedule::ROLLING_FILE_SIZE),
     bufferSize_(DEFAULT_BUFFER_SIZE),
     rollSize_(DEFAULT_ROLL_SIZE),
     baseName_(baseName),
     extension_("log") {
    buffer_ = new char[bufferSize_];
    rollFile();
}

LogFileSink::LogFileSink(const std::string& baseName,
                         const std::string& extension)
    :LogSink("LogFileSink"),
     fp_(NULL),
     buffer_(NULL),
     schedule_(RollingSchedule::ROLLING_FILE_SIZE),
     bufferSize_(DEFAULT_BUFFER_SIZE),
     rollSize_(DEFAULT_ROLL_SIZE),
     baseName_(baseName),
     extension_(extension) {
    buffer_ = new char[bufferSize_];
    rollFile();
}

LogFileSink::LogFileSink(const std::string& baseName,
                         const std::string& extension,
                         bool immediateFlush,
                         RollingSchedule schedule,
                         int bufferSize,
                         int rollSize)
    :LogSink("LogFileSink"),
     fp_(NULL),
     buffer_(NULL),
     schedule_(schedule),
     bufferSize_(bufferSize),
     rollSize_(rollSize),
     baseName_(baseName),
     extension_(extension) {

    setImmediateFlush(immediateFlush);

    if (bufferSize_ <= 0) {
        bufferSize_ = DEFAULT_BUFFER_SIZE;
    }

    buffer_ = new char[bufferSize_];

    rollFile();
}

void LogFileSink::doSink(const LogMessage& msg) {
    int64_t size = logSize();

    if (size > rollSize_) {
        rollFile();
    }
    else {
        // just for ROLLING_DAILY
        time_t now = ::time(NULL);
        int days = static_cast<int>(now / DAILY_CYCLE * DAILY_CYCLE);

        if (days != lastRollTime_) {
            rollFile();
        }
    }

    const char* buf = msg.buffer();
    std::fwrite(buf, strlen(buf), 1, fp_);

    int errCode = 0;
    errCode = ferror(fp_);

    if (errCode) {
        fprintf(stderr, "write to file error: %s.", strerror(errCode));
    }
}

void LogFileSink::doFlush() {
    if (fp_) {
        std::fflush(fp_);
    }
}

void LogFileSink::rollFile() {
    // Just for daily cycle
    time_t now = ::time(NULL);
    lastRollTime_ = static_cast<int>(now / DAILY_CYCLE * DAILY_CYCLE);

    generateLogFileName();

    if (fp_) {
        fclose(fp_);
        fp_ = NULL;
    }

    fp_ = fopen(fileName_.c_str(), "a+");
}

void LogFileSink::generateLogFileName() {
    fileName_.clear();
    fileName_.append(baseName_);

    // TODO make sure file separator
    char timebuf[32];
    //char pidbuf[32];

    time_t now = time(NULL);
    struct tm* tm = gmtime(&now);
    strftime(timebuf, sizeof timebuf, "%Y%m%d-%H%M%S", tm);
    fileName_.append(timebuf);

    ptime epoch(boost::gregorian::date(1970,1,1));
    ptime pnow = microsec_clock::local_time();
    time_duration::tick_type x = (pnow - epoch).total_nanoseconds();

    sprintf(timebuf, ":%d", x);
    fileName_.append(timebuf);

    // fileName_.append(boost::lexical_cast<std::string>(get_id()));
    fileName_.append(".log");
    fileName_.append(".");
    fileName_.append(extension_);
}

int64_t LogFileSink::logSize() const {
    int64_t size = 0x7fffffffffffffff;

    FILE* fp = fopen(fileName_.c_str(), "r");

    if (fp) {
        fseek(fp, 0L, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        fclose(fp);
    }

    return size;
}

void LogFileSink::setBufferSize(int bufferSize) {
    if (bufferSize_ == bufferSize) {
        return;
    }

    try {
        char* buffer = new char[bufferSize];

        if (buffer_) {
            delete buffer_;
        }

        buffer_ = buffer;
        bufferSize_ = bufferSize;
    }
    catch (std::bad_alloc& e) {
        fprintf(stderr, e.what());
    }
}

}
}
