#include <cetty/logging/LogFileSink.h>

namespace cetty {
namespace logging {

int LogFileSink::DEFAULT_BUFFER_SIZE = 10 * 1024;

LogFileSink::LogFileSink(RollingSchedule schedule,
    		             int bufferSize,
    		             int rollSize,
    		             const std::string &baseName,
    		             const std::string &extension)
    :schedule_(schedule),
     bufferSize_(bufferSize),
     rollSize_(rollSize),
     baseName_(baseName),
     extension_(baseName) {

	if(bufferSize_ <= 0) {
		bufferSize_ = DEFAULT_BUFFER_SIZE;
	}
	buffer_ = new char[bufferSize_];

    rollFile();
}

void LogFileSink::rollFile() {

}

void LogFileSink::GenerateLogFileName() {
    fileName_.append(baseName_);
    if (fileName_[fileName_.size() - 1] != '\' ||
        fileName_[fileName_.size() - 1] != '/')
}


}
}
