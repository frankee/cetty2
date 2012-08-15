#if !defined(CETTY_SHIRO_UTIL_EMPTYOBJ_H)
#define CETTY_SHIRO_UTIL_EMPTYOBJ_H

#include <boost/any.hpp>
#include <string>
#include <boost/date_time/posix_time/ptime.hpp>

namespace cetty {
namespace shiro {
namespace util {

using namespace boost::posix_time;

extern const boost::any emptyAny;
extern const std::string emptyString;
extern const ptime emptyPtime;
}
}
}


#endif /* CETTY_SHIRO_UTIL_EMPTYOBJ_H */
