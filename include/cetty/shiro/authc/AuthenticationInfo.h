#if !defined(CETTY_SHIRO_AUTHENTICATION_INFO_H)
#define CETTY_SHIRO_AUTHENTICATION_INFO_H
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

#include <cetty/shiro/subject/PrincipalCollection.h>
#include <cetty/shiro/util/ByteSource.h>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::shiro::subject;
using namespace cetty::shiro::util;

/**
 * Simple AuthenticationInfo that holds the principals and credentials.
 */
class AuthenticationInfo {

public:
    AuthenticationInfo() {}

    /**
     * Constructor that takes in a single 'primary' principal of the account and its corresponding credentials,
     * associated with the specified realm.
     * <p/>
     * This is a convenience constructor and will construct a {@link PrincipalCollection PrincipalCollection} based
     * on the {@code principal} and {@code realmName} argument.
     *
     * @param principal   the 'primary' principal associated with the specified realm.
     * @param credentials the credentials that verify the given principal.
     * @param realmName   the realm from where the principal and credentials were acquired.
     */
    AuthenticationInfo(const std::string &principal, const std::string &credentials, const std::string &realmName);

    /**
     * Constructor that takes in a single 'primary' principal of the account, its corresponding hashed credentials,
     * the salt used to hash the credentials, and the name of the realm to associate with the principals.
     * <p/>
     * This is a convenience constructor and will construct a {@link PrincipalCollection PrincipalCollection} based
     * on the <code>principal</code> and <code>realmName</code> argument.
     *
     * @param principal         the 'primary' principal associated with the specified realm.
     * @param hashedCredentials the hashed credentials that verify the given principal.
     * @param credentialsSalt   the salt used when hashing the given hashedCredentials
     * @param realmName         the realm from where the principal and credentials were acquired.
     */
    AuthenticationInfo(const std::string &principal, const std::string &hashedCredentials, const ByteSource &credentialsSalt, const std::string &realmName);

    /**
     * Constructor that takes in an account's identifying principal(s) and its corresponding credentials that verify
     * the principals.
     *
     * @param principals  a Realm's account's identifying principal(s)
     * @param credentials the accounts corresponding principals that verify the principals.
     */
    AuthenticationInfo(const PrincipalCollection &principals, const std::string &credentials);

    /**
     * Constructor that takes in an account's identifying principal(s), hashed credentials used to verify the
     * principals, and the salt used when hashing the credentials.
     *
     * @param principals        a Realm's account's identifying principal(s)
     * @param hashedCredentials the hashed credentials that verify the principals.
     * @param credentialsSalt   the salt used when hashing the hashedCredentials.
     */
    AuthenticationInfo(const PrincipalCollection &principals, std::string &hashedCredentials, const ByteSource &credentialsSalt);


    const PrincipalCollection &getPrincipals() const{
        return principals;
    }

    /**
     * Sets the identifying principal(s) represented by this instance.
     *
     * @param principals the indentifying attributes of the corresponding Realm account.
     */
    void setPrincipals(const PrincipalCollection &principals);

    const std::string &getCredentials() const{
        return credentials;
    }

    /**
     * Sets the credentials that verify the principals/identity of the associated Realm account.
     *
     * @param credentials attribute(s) that verify the account's identity/principals, such as a password or private key.
     */
    void setCredentials(const std::string &credentials) {
        this->credentials = credentials;
    }

    /**
     * Returns the salt used to hash the credentials, or {@code null} if no salt was used or credentials were not
     * hashed at all.
     * <p/>
     * Note that this attribute is <em>NOT</em> handled in the
     * {@link #merge(AuthenticationInfo) merge} method - a hash salt is only useful within a single realm (as each
     * realm will perform it's own Credentials Matching logic), and once finished in that realm, Shiro has no further
     * use for salts.  Therefore it doesn't make sense to 'merge' salts in a multi-realm scenario.
     *
     * @return the salt used to hash the credentials, or {@code null} if no salt was used or credentials were not
     *         hashed at all.
     * @since 1.1
     */
    const ByteSource &getCredentialsSalt();

    /**
     * Sets the salt used to hash the credentials, or {@code null} if no salt was used or credentials were not
     * hashed at all.
     * <p/>
     * Note that this attribute is <em>NOT</em> handled in the
     * {@link #merge(AuthenticationInfo) merge} method - a hash salt is only useful within a single realm (as each
     * realm will perform it's own Credentials Matching logic), and once finished in that realm, Shiro has no further
     * use for salts.  Therefore it doesn't make sense to 'merge' salts in a multi-realm scenario.
     *
     * @param salt the salt used to hash the credentials, or {@code null} if no salt was used or credentials were not
     *             hashed at all.
     * @since 1.1
     */
    void setCredentialsSalt(const ByteSource &salt);

    /**
     * Takes the specified <code>info</code> argument and adds its principals and credentials into this instance.
     *
     * @param info the <code>AuthenticationInfo</code> to add into this instance.
     */
    void merge(const AuthenticationInfo &info);

    /**
     * Returns <code>true</code> if the Object argument is an <code>instanceof SimpleAuthenticationInfo</code> and
     * its {@link #getPrincipals() principals} are equal to this instance's principals, <code>false</code> otherwise.
     *
     * @param o the object to compare for equality.
     * @return <code>true</code> if the Object argument is an <code>instanceof SimpleAuthenticationInfo</code> and
     *         its {@link #getPrincipals() principals} are equal to this instance's principals, <code>false</code> otherwise.
     */
    bool equals(const std::string &o);

    /**
     * Returns the hashcode of the internal {@link #getPrincipals() principals} instance.
     *
     * @return the hashcode of the internal {@link #getPrincipals() principals} instance.
     */
    int hashCode();

    /**
     * Simple implementation that merely returns <code>{@link #getPrincipals() principals}.toString()</code>
     *
     * @return <code>{@link #getPrincipals() principals}.toString()</code>
     */
    std::string toString();

private:
    /**
     * The principals identifying the account associated with this AuthenticationInfo instance.
     */
    PrincipalCollection principals;
    /**
     * The credentials verifying the account principals.
     */
    std::string credentials;

    /* Any salt used in hashing the credentials.*/
    ByteSource credentialsSalt;
};

}
}
}
#endif // #if !defined(CETTY_SHIRO_AUTHENTICATION_INFO_H)
