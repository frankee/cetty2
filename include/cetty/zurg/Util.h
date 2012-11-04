#include <cetty/util/StringPiece.h>

#include <map>
#include <dirent.h>

namespace cetty {
namespace zurg {

using namespace cetty::util;

extern int kMicroSecondsPerSecond;
extern int g_tempFileCount;
extern int t_numOpenedFiles;

typedef int (*filter)(const struct dirent *);

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

}
}
