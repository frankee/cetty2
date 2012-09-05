/*
 * Authorizer.cpp
 *
 *  Created on: 2012-9-4
 *      Author: chenhl
 */

#include <cetty/shiro/authz/Authorizer.h>

namespace cetty {
namespace shiro {
namespace authz {

bool Authorizer::authoriseUser(const SessionPtr &sission,
                               const std::string &operation)
{
    return true;
}

bool Authorizer::authoriseApp(const std::string &appKey,
                              const std::string &operation)
{
    if(realms.empty()) return false;

    std::vector<AuthorisingRealm>::iterator it = realms.begin();
    for(; it != realms.end(); ++it)
        if(it->authoriseApp(appKey, operation))
            return true;

    return false;
}

}
}
}


