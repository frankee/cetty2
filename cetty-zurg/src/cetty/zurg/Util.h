
#include <map>
#include <cetty/util/StringPiece.h>

namespace cetty {
namespace zurg
{

// returns file name
std::string writeTempFile(cetty::util::StringPiece prefix,
    cetty::util::StringPiece content);

void parseMd5sum(const std::string& lines,
    std::map<cetty::util::StringPiece, cetty::util::StringPiece>* md5sums);

void setupWorkingDir(const std::string& cwd);

void setNonBlockAndCloseOnExec(int fd);

}
}
