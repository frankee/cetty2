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
    class RollingSchedule : public cetty::util::Enum<RollingSchedule> {
    public:
        static RollingSchedule ROLLING_NONE;
        static RollingSchedule ROLLING_FILE_SIZE;   /*basename.1*/
        static RollingSchedule ROLLING_MONTHLY;     /*basename.2013-05-07*/
        static RollingSchedule ROLLING_WEEKLY;      /*basename.2013-05-07*/
        static RollingSchedule ROLLING_TWICE_DAILY; /*basename.2013-05-07*/
        static RollingSchedule ROLLING_DAILY;       /*basename.2013-05-07*/
        static RollingSchedule ROLLING_HOURLY;      /*basename.2013-05-07.01*/
        static RollingSchedule ROLLING_MINUTELY;     /*basename.2013-05-07.01.01*/

    private:
        RollingSchedule(int value, const char* name);
    };

public:
    LogFileSink(const std::string& baseName);

    LogFileSink(const std::string& baseName,
                const std::string& extension);

    LogFileSink(const std::string& baseName,
                const std::string& extension,
                bool immediateFlush,
                RollingSchedule schedule,
                int bufferSize,
                int rollSize);

    virtual void doSink(const LogMessage& msg);
    virtual void doFlush();

    // Get the current LOG file size.
    // The returned value is approximate since some
    // logged data may not have been flushed to disk yet.
    int64_t logSize() const;

    /**
     * specifies output file base name.
     */
    const std::string& baseName() const;
    void setBaseName(const std::string& name);

    const std::string& extension() const;
    void setExtension(const std::string& extension);

    /**
     * Non-zero value of this property sets up buffering of output
     * stream using a buffer of given size.
     */
    int bufferSize() const;
    void setBufferSize(int bufferSize);

    /**
     *
     */
    const RollingSchedule& rollingSchedule() const;
    void setRollingSchedule(const RollingSchedule& schedule);

    /**
     * This property specifies maximal size of output file. The
     * value is in bytes.
     */
    int maxRollingFileSize() const;
    void setRollingMaxFileSize(int maxFileSize);

    /**
     * This property limits the number of backup output
     * files; e.g. how many <tt>log.1</tt>, <tt>log.2</tt>
     * (or <tt>log.2013-05-07.01.01</tt>) etc. files
     * will be kept.
     */
    int maxRollingBackups() const;
    void setMaxRollingBackups(int backups);

private:
    void rollFile();
    void generateLogFileName();

private:
    FILE* fp_;
    char* buffer_;
    RollingSchedule schedule_;

    int bufferSize_;
    int64_t rollSize_;
    int lastRollTime_;

    std::string baseName_;
    std::string extension_;
    std::string fileName_;
};

inline
const std::string& LogFileSink::baseName() const {
    return baseName_;
}

inline
void LogFileSink::setBaseName(const std::string& name) {
    baseName_ = name;
}

inline
const std::string& LogFileSink::extension() const {
    return extension_;
}

inline
void LogFileSink::setExtension(const std::string& extension) {
    extension_ = extension;
}

inline
int LogFileSink::bufferSize() const {
    return bufferSize_;
}

inline
const LogFileSink::RollingSchedule& LogFileSink::rollingSchedule() const {
    return schedule_;
}

inline
void LogFileSink::setRollingSchedule(const RollingSchedule& schedule) {
    schedule_ = schedule;
}

inline
int LogFileSink::maxRollingFileSize() const {
    return rollSize_;
}

inline
void LogFileSink::setRollingMaxFileSize(int maxFileSize) {
    rollSize_ = maxFileSize;
}

inline
int LogFileSink::maxRollingBackups() const {
    return 0;
}

inline
void LogFileSink::setMaxRollingBackups(int backups) {
}

}
}

#endif //#if !defined(CETTY_LOGGING_LOGFILESINK_H)

// Local Variables:
// mode: c++
// End:
