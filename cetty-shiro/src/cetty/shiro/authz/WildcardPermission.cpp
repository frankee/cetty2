/*
 * WildcardPermission.cpp
 *
 *  Created on: 2012-9-20
 *      Author: chenhl
 */

#include <cetty/shiro/authz/WildcardPermission.h>
#include <cetty/logging/LoggerHelper.h>

#include <boost/algorithm/string.hpp>

namespace cetty {
namespace shiro {
namespace authz {

const std::string WildcardPermission::WILDCARD_TOKEN = "*";
const std::string WildcardPermission::PART_DIVIDER_TOKEN = ":";
const bool WildcardPermission::DEFAULT_CASE_SENSITIVE = false;

bool WildcardPermission::implies(const PermissionPtr& p) {
    WildcardPermissionPtr permission = boost::dynamic_pointer_cast<WildcardPermission>(p);

    if (!permission) {
        return false;
    }

    const std::vector<std::string>& otherParts= permission->getParts();
    unsigned int i = 0;

    for (; i < otherParts.size() && i < parts.size(); ++i) {
        if ("*" != parts.at(i) && parts.at(i) != otherParts.at(i)) {
            return false;
        }
    }

    if (i == parts.size()) { return true; }

    for (; i < parts.size(); ++i) {
        if ("*" != parts.at(i)) {
            return false;
        }
    }

    return true;
}

void WildcardPermission::setParts(const std::string& strPermission, bool caseSensitive) {
    std::string wildcardString = strPermission;
    if (wildcardString.empty()) {
        LOG_TRACE << "Wildcard string cannot be null or empty. "
                  "Make sure permission strings are properly formatted.";
        return;
    }

    boost::trim(wildcardString);
    if(!caseSensitive) boost::to_lower(wildcardString);

    size_t pre = 0, pos = 0;
    while ((pos = wildcardString.find(':', pre)) != std::string::npos) {
        parts.push_back(wildcardString.substr(pre, pos - pre));
        pre = pos + 1;
    }
    parts.push_back(wildcardString.substr(pre, wildcardString.size() - pre));
}

std::string WildcardPermission::toString() const{
    std::string permission;
    bool flag = false;

    std::vector<std::string>::const_iterator it = parts.begin();

    for (; it != parts.end(); ++ it) {
        if (flag) { permission.append(":"); }

        permission.append(*it);
        flag = true;
    }

    return permission;
}

}
}
}
