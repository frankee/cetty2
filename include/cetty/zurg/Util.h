#include <cetty/util/StringPiece.h>

#include <map>
#include <dirent.h>
#include <boost/date_time/posix_time/ptime.hpp>

namespace cetty {
namespace zurg {

using namespace boost::posix_time;
using namespace cetty::util;

extern int kMicroSecondsPerSecond;
extern int g_tempFileCount;
extern int t_numOpenedFiles;
extern ptime g_startTime;

typedef int (*filter)(const struct dirent *);

bool setFl(int fd, int flag);
bool detFl(int fd, int flag);

bool setFd(int fd, int flag);
bool detFd(int fd, int flag);

// returns file name
std::string writeTempFile(
    const StringPiece &prefix,
    const StringPiece &content
);

void parseMd5sum(
    const std::string& lines,
    std::map<StringPiece, StringPiece>* md5sums
);

void setupWorkingDir(const std::string& cwd);

void setNonBlockAndCloseOnExec(int fd);

int64_t now();

int numThreads();
std::string procStatus();

int scanDir(const char *dirpath, int (*filter)(const struct dirent *));

int openedFiles();
int maxOpenFiles();

int fdDirFilter(const struct dirent* d);

int64_t getMicroSecs(const ptime &p);
std::string hostname();

}
}
