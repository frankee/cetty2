#include <cetty/zurg/Util.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/util/SmallFile.h>
#include <cetty/util/Integer.h>

#include <string>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/resource.h>

namespace cetty {
namespace zurg {

using namespace cetty::util;

int kMicroSecondsPerSecond = 1000 * 1000;
int g_tempFileCount = 0;
int t_numOpenedFiles = 0;
ptime g_startTime = microsec_clock::local_time();

int fdDirFilter(const struct dirent* d){
    if (::isdigit(d->d_name[0])){
        ++t_numOpenedFiles;
    }
    return 0;
}

std::string writeTempFile(const StringPiece prefix, const StringPiece content) {
    char buf[256];
    ::snprintf(buf, sizeof buf, "/tmp/%s-runScript-%s-%d-%05d-XXXXXX",
               prefix.data(),
               Integer::toString(now()).c_str(),
               ::getpid(),
               ++g_tempFileCount);

    int tempfd = ::mkostemp(buf, O_CLOEXEC);
    ssize_t n = ::pwrite(tempfd, content.data(), content.size(), 0);
    ::fchmod(tempfd, 0755);
    ::close(tempfd);

    return n == content.size() ? buf : "";
}

void parseMd5sum(const std::string& lines, std::map<StringPiece, StringPiece>* md5sums) {
    const char* p = lines.c_str();
    size_t nl = 0;

    while (*p) {
        StringPiece md5(p, 32);
        nl = lines.find('\n', nl);
        assert(nl != std::string::npos);

        StringPiece file(p + 34, static_cast<int>(lines.c_str() + nl - p - 34));
        (*md5sums)[file] = md5;

        p = lines.c_str() + nl + 1;
        ++nl;
    }
}

void setupWorkingDir(const std::string& cwd) {
    ::mkdir(cwd.c_str(), 0755); // don't check return value
    char buf[256];
    ::snprintf(buf, sizeof buf, "/%s/XXXXXX", cwd.c_str());
    int fd = ::mkstemp(buf);

    if (fd < 0) {
        LOG_FATAL << '/' << cwd << " is not accessible.";
    }

    ::close(fd);
    ::unlink(buf);

    ::snprintf(buf, sizeof buf, "/%s/pid", cwd.c_str());
    fd = ::open(buf, O_WRONLY | O_CREAT | O_CLOEXEC, 0644);

    if (fd < 0) {
        LOG_FATAL << "Failed to create pid file at " << buf;
    }

    if (::flock(fd, LOCK_EX | LOCK_NB)) {
        LOG_SYSFATAL << "Failed to lock pid file at " << buf
                     << ", another instance running ?";
    }

    if (::ftruncate(fd, 0)) {
        LOG_ERROR << "::ftruncate " << buf;
    }

    char pid[32];
    int len = ::snprintf(pid, sizeof pid, "%d\n", ::getpid());
    ssize_t n = ::write(fd, pid, len);
    assert(n == len);
    (void)n;
    // leave fd open, to keep the lock
}

void setNonBlockAndCloseOnExec(int fd) {
    int flags = ::fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(fd, F_SETFL, flags);

    flags = ::fcntl(fd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(fd, F_SETFD, flags);

    (void)ret;
}

int64_t now(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return seconds * kMicroSecondsPerSecond + tv.tv_usec;
}

int numThreads(){
    int result = 0;
    std::string status = procStatus();
    size_t pos = status.find("Threads:");
    if (pos != std::string::npos){
      result = ::atoi(status.c_str() + pos + 8);
    }
    return result;
}

std::string procStatus(){
    std::string result;
    SmallFile::readFile("/proc/self/status", 65536, &result);

    return result;
}

int scanDir(const char *dirpath, filter fil){
    struct dirent** namelist = NULL;
    int result = ::scandir(dirpath, &namelist, fil, alphasort);
    assert(namelist == NULL);
    return result;
}

int openedFiles(){
    t_numOpenedFiles = 0;
    scanDir("/proc/self/fd", fdDirFilter);
    return t_numOpenedFiles;
}

int maxOpenFiles(){
    struct rlimit rl;
    int ret = ::getrlimit(RLIMIT_NOFILE, &rl);
    if (ret == -1) return openedFiles();
    else return static_cast<int>(rl.rlim_cur);
}

int64_t getMicroSecs(const ptime &p){
    ptime epoch(boost::gregorian::date(1970,1,1));
    time_duration::tick_type x = (p - epoch).total_nanoseconds();
    return static_cast<int64_t>(x);
}

std::string hostname(){
    char buf[64] = "unknownhost";
    ::gethostname(buf, sizeof buf);
    return buf;
}

}
}
