#if !defined(CETTY_SHIRO_UTIL_EMPTYOBJ_H)
#define CETTY_SHIRO_UTIL_EMPTYOBJ_H

#include <string>

#include <boost/any.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

//#include <cetty/shiro/session/SessionPtr.h>

namespace cetty {
namespace shiro {
namespace util {

using namespace boost::posix_time;

extern boost::any emptyAny;
extern std::string emptyString;
extern ptime emptyPtime;
//extern SessionPtr emptySessionPtr;

}
}
}


#endif /* CETTY_SHIRO_UTIL_EMPTYOBJ_H */
