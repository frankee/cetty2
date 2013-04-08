#if !defined(CETTY_SHIRO_REALM_REALM_H)
#define CETTY_SHIRO_REALM_REALM_H

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

namespace cetty {
namespace shiro {
namespace realm {

using namespace cetty::util;

/**
 * A <tt>Realm</tt> is a security component that can access application-specific security entities
 * such as users, roles, and permissions to determine authentication and authorization operations.
 *
 * <p><tt>Realm</tt>s usually have a 1-to-1 correspondance with a datasource such as a relational database,
 * file sysetem, or other similar resource.  As such, implementations of this interface use datasource-specific APIs to
 * determine authorization data (roles, permissions, etc), such as JDBC, File IO, Hibernate or JPA, or any other
 * Data Access API.  They are essentially security-specific
 * <a href="http://en.wikipedia.org/wiki/Data_Access_Object" target="_blank">DAO</a>s.
 *
 * <p>Because most of these datasources usually contain Subject (a.k.a. User) information such as usernames and
 * passwords, a Realm can act as a pluggable authentication module in a
 * <a href="http://en.wikipedia.org/wiki/Pluggable_Authentication_Modules">PAM</a> configuration.  This allows a Realm to
 * perform <i>both</i> authentication and authorization duties for a single datasource, which caters to the large
 * majority of applications.  If for some reason you don't want your Realm implementation to perform authentication
 * duties, you should override the {@link #supports(org.apache.shiro.authc.AuthenticationToken)} method to always
 * return <tt>false</tt>.
 *
 * <p>Because every application is different, security data such as users and roles can be
 * represented in any number of ways.  Shiro tries to maintain a non-intrusive development philosophy whenever
 * possible - it does not require you to implement or extend any <tt>User</tt>, <tt>Group</tt> or <tt>Role</tt>
 * interfaces or classes.
 *
 * <p>Instead, Shiro allows applications to implement this interface to access environment-specific datasources
 * and data model objects.  The implementation can then be plugged in to the application's Shiro configuration.
 * This modular technique abstracts away any environment/modeling details and allows Shiro to be deployed in
 * practically any application environment.
 *
 * <p>Most users will not implement the <tt>Realm</tt> interface directly, but will extend one of the subclasses,
 * {@link org.apache.shiro.realm.AuthenticatingRealm AuthenticatingRealm} or {@AuthorizingRealm}, greatly reducing the effort requird
 * to implement a <tt>Realm</tt> from scratch.</p>
 *
 * @see org.apache.shiro.realm.AuthenticatingRealm AuthenticatingRealm
 * @see org.apache.shiro.realm.AuthorizingRealm AuthorizingRealm
 * @see org.apache.shiro.authc.pam.ModularRealmAuthenticator ModularRealmAuthenticator
 * @since 0.1
 */
class Realm : public ReferenceCounter<Realm, int> {
public:
    Realm() { name = autoName(); }
    Realm(std::string name): name(name){}

    virtual ~Realm(){}

    const std::string &getName() const { return name; }
    void setName(const std::string &name) { this->name = name; }

    virtual void onLogout(const std::string &principal){/*NOOP*/}

private:
    /**
     * Generator name based on the current time.
     */
    std::string autoName();

private:
    std::string name;
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_REALM_REALM_H)
