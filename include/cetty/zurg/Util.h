
#include <map>
#include <cetty/util/StringPiece.h>

namespace cetty {
namespace zurg {

using namespace cetty::util;

extern int kMicroSecondsPerSecond;
extern int g_tempFileCount;

// returns file name
std::string writeTempFile(
    StringPiece prefix,
    StringPiece content
);

void parseMd5sum(
    const std::string& lines,
    std::map<StringPiece, StringPiece>* md5sums
);

void setupWorkingDir(const std::string& cwd);

void setNonBlockAndCloseOnExec(int fd);

int64_t now();

}
}
