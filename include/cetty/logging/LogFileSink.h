#if !defined(CETTY_LOGGING_LOGFILESINK_H)
#define CETTY_LOGGING_LOGFILESINK_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/Types.h>
#include <cetty/logging/LogSink.h>

namespace cetty {
namespace logging {

class LogFileSink : public LogSink {
public:
    enum RollingSchedule {
        ROLLING_NONE,
        ROLLING_FILE_SIZE,   /*basename.1*/
        ROLLING_MONTHLY,     /*basename.2013-05-07*/
        ROLLING_WEEKLY,      /*basename.2013-05-07*/
        ROLLING_TWICE_DAILY, /*basename.2013-05-07*/
        ROLLING_DAILY,       /*basename.2013-05-07*/
        ROLLING_HOURLY,      /*basename.2013-05-07.01*/
        ROLLING_MINUTELY     /*basename.2013-05-07.01.01*/
    };

public:

    LogFileSink(bool immediateFlush,
    		    RollingSchedule schedule,
    		    int bufferSize,
    		    int rollSize,
    		    const std::string &baseName,
    		    const std::string &extension);


    virtual void doSink(const LogMessage& msg);
    virtual void doFlush();

    // Get the current LOG file size.
    // The returned value is approximate since some
    // logged data may not have been flushed to disk yet.
    int64_t logSize() const;

    /**
     * specifies output file base name.
     */
    const std::string& baseName() const { return baseName_; }
    void setBaseName(const std::string& name) { baseName_ = name; }

    const std::string& extension() const { return extension_; }
    void setExtension(const std::string& extension) { extension_ = extension; }

    /**
     * Non-zero value of this property sets up buffering of output
     * stream using a buffer of given size.
     */
    int bufferSize() const { return bufferSize_; }
    void setBufferSize(int bufferSize);

    /**
     *
     */
    const RollingSchedule& rollingSchedule() const { return schedule_; }
    void setRollingSchedule(const RollingSchedule& schedule) {
    	schedule_ = schedule;
    }

    /**
     * This property specifies maximal size of output file. The
     * value is in bytes.
     */
    int maxRollingFileSize() const { return rollSize_; }
    void setRollingMaxFileSize(int maxFileSize) { rollSize_ = maxFileSize; }

    /**
     * This property limits the number of backup output
     * files; e.g. how many <tt>log.1</tt>, <tt>log.2</tt>
     * (or <tt>log.2013-05-07.01.01</tt>) etc. files
     * will be kept.
     */
    int maxRollingBackups() const { return 0; }
    void setMaxRollingBackups(int backups){}

private:
    void rollFile();
    void generateLogFileName();

private:
    const static int DEFAULT_BUFFER_SIZE;
    const static int DAILY_CYCLE;

    FILE* fp_;
    char *buffer_;
    RollingSchedule schedule_;

    int bufferSize_;
    int64_t rollSize_;
    int lastRollTime_;

    std::string baseName_;
    std::string extension_;
    std::string fileName_;
};

}
}

#endif //#if !defined(CETTY_LOGGING_LOGFILESINK_H)

// Local Variables:
// mode: c++
// End:
