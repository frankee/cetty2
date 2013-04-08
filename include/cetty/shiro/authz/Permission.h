#if !defined(CETTY_SHIRO_AUTHZ_PERMISSION_H)
#define CETTY_SHIRO_AUTHZ_PERMISSION_H
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <string>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/shiro/authz/PermissionPtr.h>

namespace cetty {
namespace shiro {
namespace authz {

using namespace cetty::util;

/**
 * A Permission represents the ability to perform an action or access a resource.  A Permission is the most
 * granular, or atomic, unit in a system's security policy and is the cornerstone upon which fine-grained security
 * models are built.
 * <p/>
 * It is important to understand a Permission instance only represents functionality or access - it does not grant it.
 * Granting access to an application functionality or a particular resource is done by the application's security
 * configuration, typically by assigning Permissions to users, roles and/or groups.
 * <p/>
 */
class Permission : public ReferenceCounter<Permission, int> {
public:
    Permission(const std::string &strPermission)
        : strPermission(strPermission) {}

    virtual ~Permission() {}

    /**
     * Returns {@code true} if this current instance <em>implies</em> all the functionality and/or resource access
     * described by the specified {@code Permission} argument, {@code false} otherwise.
     * <p/>
     * <p>That is, this current instance must be exactly equal to or a <em>superset</em> of the functionalty
     * and/or resource access described by the given {@code Permission} argument.  Yet another way of saying this
     * would be:
     * <p/>
     * <p>If &quot;permission1 implies permission2&quot;, i.e. <code>permission1.implies(permission2)</code> ,
     * then any Subject granted {@code permission1} would have ability greater than or equal to that defined by
     * {@code permission2}.
     *
     * @param p the permission to check for behavior/functionality comparison.
     * @return {@code true} if this current instance <em>implies</em> all the functionality and/or resource access
     *         described by the specified {@code Permission} argument, {@code false} otherwise.
     */
    virtual bool implies(const PermissionPtr& p) = 0;

    virtual const std::string& stringPermission() const {
        return strPermission;
    }

    virtual std::string toString() const = 0;

protected:
    std::string strPermission;
};

}
}
}

#endif //#if !defined(CETTY_SHIRO_AUTHZ_PERMISSION_H)

// Local Variables:
// mode: c++
// End:
