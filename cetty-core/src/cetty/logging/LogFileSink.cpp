#include <cetty/logging/LogFileSink.h>

#include <boost/thread.hpp>

#include <ctime>

namespace cetty {
namespace logging {

using namespace boost::this_thread;

const int LogFileSink::DEFAULT_BUFFER_SIZE = 10 * 1024;
const int LogFileSink::DAILY_CYCLE = 24 * 60 * 60;

LogFileSink::LogFileSink(bool immediateFlush,
		                 RollingSchedule schedule,
    		             int bufferSize,
    		             int rollSize,
    		             const std::string &baseName,
    		             const std::string &extension)
    :LogSink("LogFileSink"),
     fp_(NULL),
     buffer_(NULL),
     schedule_(schedule),
     bufferSize_(bufferSize),
     rollSize_(rollSize),
     baseName_(baseName),
     extension_(baseName) {

	setImmediateFlush(immediateFlush);

	if(bufferSize_ <= 0) {
		bufferSize_ = DEFAULT_BUFFER_SIZE;
	}
	buffer_ = new char[bufferSize_];

    rollFile();
}

void LogFileSink::doSink(const LogMessage& msg) {
	int64_t size = logSize();

	if(size > rollSize_) {
		rollFile();
	} else {
		// just for ROLLING_DAILY
		time_t now = ::time(NULL);
	    int days = now / DAILY_CYCLE * DAILY_CYCLE;
	    if (days != lastRollTime_) {
	    	rollFile();
	    }
	}

	const char* buf = msg.buffer();
    std::fwrite(buf, strlen(buf), 1, fp_);

    int errCode = 0;
    char errBuf[512];

    errCode = ferror(fp_);
    if(errCode) {
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
	lastRollTime_ = now / DAILY_CYCLE * DAILY_CYCLE;

    generateLogFileName();

    if (fp_) {
    	fclose(fp_);
    	fp_ = NULL;
    }

    fp_ = fopen(fileName_.c_str(), "a+");
}

void LogFileSink::generateLogFileName() {
    fileName_.append(baseName_);

    // tudo make sure file separator

    char timebuf[32];
    char pidbuf[32];
    struct tm tm;

    time_t now = time(NULL);
    gmtime_r(&now, &tm);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    fileName_.append(timebuf);

    //snprintf(pidbuf, sizeof pidbuf, ".%d", boost::lexical_cast<std::string>(get_id()));
    //fileName_.append(pidbuf);
    fileName_.append(boost::lexical_cast<std::string>(get_id()));

    fileName_.append(".log");
    fileName_.append(".");
    fileName_.append(extension_);
}

int64_t LogFileSink::logSize() const{
	int64_t size = 0x7fffffffffffffff;

	FILE *fp = fopen(fileName_.c_str(), "r");
	if (fp) {
		fseek(fp, 0L, SEEK_END);
		size = ftell(fp);
		fclose(fp);
	}

	return size;
}

void LogFileSink::setBufferSize(int bufferSize) {
	if (bufferSize_ == bufferSize) return;

	if (buffer_) {
		delete buffer_;
		buffer_ = NULL;
	}

	try {
	    buffer_ = new char[bufferSize];
	    bufferSize_ = bufferSize;
	}
	catch (std::bad_alloc &e) {
		fprintf(stderr, e.what());
	}
}




}
}
