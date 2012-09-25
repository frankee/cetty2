/*
 * WildcardPermission.cpp
 *
 *  Created on: 2012-9-20
 *      Author: chenhl
 */

#include <cetty/shiro/authz/WildcardPermission.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace shiro {
namespace authz {

const std::string WildcardPermission::WILDCARD_TOKEN = "*";
const std::string WildcardPermission::PART_DIVIDER_TOKEN = ":";
const bool DEFAULT_CASE_SENSITIVE = false;

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

void WildcardPermission::setParts(std::string& wildcardString, bool caseSensitive) {
    if (wildcardString.empty()) {
        LOG_TRACE << "Wildcard string cannot be null or empty. "
                  "Make sure permission strings are properly formatted.";
        return;
    }

    while (wildcardString.at(wildcardString.size() - 1) == ' ') {
        wildcardString.erase(wildcardString.size() - 1, 1);
    }

    size_t pre = 0, pos = 0;

    while ((pos = wildcardString.find(':', pre)) != std::string::npos) {
        parts.push_back(wildcardString.substr(pre, pos - pre));
        pre = pos + 1;
    }

    parts.push_back(wildcardString.substr(pre, wildcardString.size() - pre));

    if (!caseSensitive) {
        std::vector<std::string>::iterator iter = parts.begin();

        for (; iter != parts.end(); iter ++) {
            lowercase(&(*iter));
        }
    }
}

void WildcardPermission::lowercase(std::string* part) {
    if (part == NULL || part->empty()) { return; }

    int diff = 'a' - 'A';
    unsigned int i = 0;

    for (; i < part->size(); ++i)
        if (part->at(i) >= 'A' && part->at(i) <= 'Z') {
            part->at(i) += diff;
        }
}

std::string WildcardPermission::toString() const {
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
