/*
 * AuthorisingRealm.cpp
 *
 *  Created on: 2012-9-4
 *      Author: chenhl
 */



#include <cetty/shiro/realm/AuthorisingRealm.h>

namespace cetty {
namespace shiro {
namespace realm {

AuthorisingRealm::AuthorisingRealm(bool isCached,
                                   std::string name
    ): Realm(isCached, name)
{

}

bool AuthorisingRealm::authoriseApp(const std::string &appKey,
                                   const std::string &operation)
{
    return true;
}

}
}
}
