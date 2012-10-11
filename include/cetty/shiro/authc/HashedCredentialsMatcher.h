#if !defined(CETTY_SHIRO_AUTHC_HASHEDCREDENTIALSMATCHER_H)
#define CETTY_SHIRO_AUTHC_HASHEDCREDENTIALSMATCHER_H
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

#include <cetty/shiro/authc/CredentialsMatcher.h>
#include <cetty/shiro/authc/HashedCredentialsMatcherConfig.h>


namespace cetty {
namespace shiro {
namespace crypt {
    class DigestEngine;
}
}
}

namespace cetty {
namespace shiro {
namespace authc {

class AuthenticationInfo;
class AuthenticationToken;

using namespace cetty::shiro::crypt;

/**
 * A {@code HashedCredentialMatcher} provides support for hashing of supplied {@code AuthenticationToken} credentials
 * before being compared to those in the {@code AuthenticationInfo} from the data store.
 * <p/>
 * Credential hashing is one of the most common security techniques when safeguarding a user's private credentials
 * (passwords, keys, etc).  Most developers never want to store their users' credentials in plain form, viewable by
 * anyone, so they often hash the users' credentials before they are saved in the data store.
 * <p/>
 * This class (and its subclasses) function as follows:
 * <ol>
 * <li>Hash the {@code AuthenticationToken} credentials supplied by the user during their login.</li>
 * <li>Compare this hashed value directly with the {@code AuthenticationInfo} credentials stored in the system
 * (the stored account credentials are expected to already be in hashed form).</li>
 * <li>If these two values are {@link #equals(Object, Object) equal}, the submitted credentials match, otherwise
 * they do not.</li>
 * </ol>
 * <h2>Salting and Multiple Hash Iterations</h2>
 * Because simple hashing is usually not good enough for secure applications, this class also supports 'salting'
 * and multiple hash iterations.  Please read this excellent
 * <a href="http://www.owasp.org/index.php/Hashing_Java" _target="blank">Hashing Java article</a> to learn about
 * salting and multiple iterations and why you might want to use them. (Note of sections 5
 * &quot;Why add salt?&quot; and 6 "Hardening against the attacker's attack").   We should also note here that all of
 * Shiro's Hash implementations (for example, {@link org.apache.shiro.crypto.hash.Md5Hash Md5Hash},
 * {@link org.apache.shiro.crypto.hash.Sha1Hash Sha1Hash}, etc) support salting and multiple hash iterations via
 * overloaded constructors.
 * <h4>Real World Case Study</h4>
 * In April 2010, some public Atlassian Jira and Confluence
 * installations (Apache Software Foundation, Codehaus, etc) were the target of account attacks and user accounts
 * were compromised.  The reason?  Jira and Confluence at the time did not salt user passwords and attackers were
 * able to use dictionary attacks to compromise user accounts (Atlassian has since
 * <a href="http://blogs.atlassian.com/news/2010/04/oh_man_what_a_day_an_update_on_our_security_breach.html">
 * fixed the problem</a> of course).
 * <p/>
 * The lesson?
 * <p/>
 * <b>ALWAYS, ALWAYS, ALWAYS SALT USER PASSWORDS!</b>
 * <p/>
 * <h3>Salting</h3>
 * Prior to Shiro 1.1, salts could be obtained based on the end-user submitted
 * {@link AuthenticationToken AuthenticationToken} via the now-deprecated
 * {@link #getSalt(org.apache.shiro.authc.AuthenticationToken) getSalt(AuthenticationToken)} method.  This however
 * could constitute a security hole since ideally salts should never be obtained based on what a user can submit.
 * User-submitted salt mechanisms are <em>much</em> more susceptible to dictionary attacks and <b>SHOULD NOT</b> be
 * used in secure systems.  Instead salts should ideally be a secure randomly-generated number that is generated when
 * the user account is created.  The secure number should never be disseminated to the user and always kept private
 * by the application.
 * <h4>Shiro 1.1</h4>
 * As of Shiro 1.1, it is expected that any salt used to hash the submitted credentials will be obtained from the
 * stored account information (represented as an {@link AuthenticationInfo AuthenticationInfo} instance).  This is much
 * more secure because the salt value remains private to the application (Shiro will never store this value).
 * <p/>
 * To enable this, {@code Realm}s should return {@link SaltedAuthenticationInfo SaltedAuthenticationInfo} instances
 * during authentication.  {@code HashedCredentialsMatcher} implementations will then use the provided
 * {@link org.apache.shiro.authc.SaltedAuthenticationInfo#getCredentialsSalt credentialsSalt} for hashing.  To avoid
 * security risks,
 * <b>it is highly recommended that any existing {@code Realm} implementations that support hashed credentials are
 * updated to return {@link SaltedAuthenticationInfo SaltedAuthenticationInfo} instances as soon as possible</b>.
 * <h4>Shiro 1.0 Backwards Compatibility</h4>
 * Because of the identified security risk, {@code Realm} implementations that support credentials hashing should
 * be updated to return {@link SaltedAuthenticationInfo SaltedAuthenticationInfo} instances as
 * soon as possible.
 * <p/>
 * If this is not possible for some reason, this class will retain 1.0 backwards-compatible behavior of obtaining
 * the salt via the now-deprecated {@link #getSalt(AuthenticationToken) getSalt(AuthenticationToken)} method.  This
 * method will only be invoked if a {@code Realm} <em>does not</em> return
 * {@link SaltedAuthenticationInfo SaltedAutenticationInfo} instances and {@link #isHashSalted() hashSalted} is
 * {@code true}.
 * But please note that the {@link #isHashSalted() hashSalted} property and the
 * {@link #getSalt(AuthenticationToken) getSalt(AuthenticationToken)} methods will be removed before the Shiro 2.0
 * release.
 * <h3>Multiple Hash Iterations</h3>
 * If you hash your users' credentials multiple times before persisting to the data store, you will also need to
 * set this class's {@link #setHashIterations(int) hashIterations} property.  See the
 * <a href="http://www.owasp.org/index.php/Hashing_Java" _target="blank">Hashing Java article</a>'s
 * <a href="http://www.owasp.org/index.php/Hashing_Java#Hardening_against_the_attacker.27s_attack">
 * &quot;Hardening against the attacker's attack&quot;</a> section to learn more about why you might want to use
 * multiple hash iterations.
 * <h2>MD5 &amp; SHA-1 Notice</h2>
 * <a href="http://en.wikipedia.org/wiki/MD5">MD5</a> and
 * <a href="http://en.wikipedia.org/wiki/SHA_hash_functions">SHA-1</a> algorithms are now known to be vulnerable to
 * compromise and/or collisions (read the linked pages for more).  While most applications are ok with either of these
 * two, if your application mandates high security, use the SHA-256 (or higher) hashing algorithms and their
 * supporting {@code CredentialsMatcher} implementations.
 *
 * @see org.apache.shiro.crypto.hash.Md5Hash
 * @see org.apache.shiro.crypto.hash.Sha1Hash
 * @see org.apache.shiro.crypto.hash.Sha256Hash
 * @since 0.9
 */
class HashedCredentialsMatcher : public CredentialsMatcher {
public:
    /**
     * JavaBeans-compatibile no-arg constructor intended for use in IoC/Dependency Injection environments.  If you
     * use this constructor, you <em>MUST</em> also additionally set the
     * {@link #setHashAlgorithmName(String) hashAlgorithmName} property.
     */
    HashedCredentialsMatcher();

    /**
     * Returns {@code true} if the provided token credentials match the stored account credentials,
     * {@code false} otherwise.
     *
     * @param token   the {@code AuthenticationToken} submitted during the authentication attempt
     * @param info the {@code AuthenticationInfo} stored in the system.
     * @return {@code true} if the provided token credentials match the stored account credentials,
     *         {@code false} otherwise.
     */
    virtual bool match(const AuthenticationToken& token,
                       const AuthenticationInfo& info);

    /**
     * Returns the {@code Hash} {@link org.apache.shiro.crypto.hash.Hash#getAlgorithmName() algorithmName} to use
     * when performing hashes for credentials matching.
     *
     * @return the {@code Hash} {@link org.apache.shiro.crypto.hash.Hash#getAlgorithmName() algorithmName} to use
     *         when performing hashes for credentials matching.
     * @since 1.1
     */
    const std::string getHashAlgorithmName() const {
        return config.hashAlgorithm;
    }

    /**
     * Returns {@code true} if the system's stored credential hash is Hex encoded, {@code false} if it
     * is Base64 encoded.
     * <p/>
     * Default value is {@code true} for convenience - all of Shiro's {@link Hash Hash#toString()}
     * implementations return Hex encoded values by default, making this class's use with those implementations
     * easier.
     *
     * @return {@code true} if the system's stored credential hash is Hex encoded, {@code false} if it
     *         is Base64 encoded.  Default is {@code true}
     */
    bool isStoredCredentialsHexEncoded() const {
        return config.storedCredentialsHexEncoded;
    }

    /**
     * Returns {@code true} if a submitted {@code AuthenticationToken}'s credentials should be salted when hashing,
     * {@code false} if it should not be salted.
     * <p/>
     * If enabled, the salt used will be obtained via the {@link #getSalt(AuthenticationToken) getSalt} method.
     * <p/>
     * The default value is {@code false}.
     *
     * @return {@code true} if a submitted {@code AuthenticationToken}'s credentials should be salted when hashing,
     *         {@code false} if it should not be salted.
     * @deprecated since Shiro 1.1.  Hash salting is now expected to be based on if the {@link AuthenticationInfo}
     *             returned from the {@code Realm} is a {@link SaltedAuthenticationInfo} instance and its
     *             {@link org.apache.shiro.authc.SaltedAuthenticationInfo#getCredentialsSalt() getCredentialsSalt()} method returns a non-null value.
     *             This method and the 1.0 behavior still exists for backwards compatibility if the {@code Realm} does not return
     *             {@code SaltedAuthenticationInfo} instances, but <b>it is highly recommended that {@code Realm} implementations
     *             that support hashed credentials start returning {@link SaltedAuthenticationInfo SaltedAuthenticationInfo}
     *             instances as soon as possible</b>.
     *             <p/>
     *             This is because salts should always be obtained from the stored account information and
     *             never be interpreted based on user/Subject-entered data.  User-entered data is easier to compromise for
     *             attackers, whereas account-unique (and secure randomly-generated) salts never disseminated to the end-user
     *             are almost impossible to break.  This method will be removed in Shiro 2.0.
     */
    bool isHashSalted() const {
        return config.hashSalted;
    }

    /**
     * Returns the number of times a submitted {@code AuthenticationToken}'s credentials will be hashed before
     * comparing to the credentials stored in the system.
     * <p/>
     * Unless overridden, the default value is {@code 1}, meaning a normal hash execution will occur.
     *
     * @return the number of times a submitted {@code AuthenticationToken}'s credentials will be hashed before
     *         comparing to the credentials stored in the system.
     */
    int getHashIterations() const {
        return config.hashIterations;
    }

    ~HashedCredentialsMatcher();

public:
    static const std::string MD5ENGINE;
    static const std::string SHA1ENGINE;

private:
    void init();

    /**
     * Returns the {@code account}'s credentials.
     * <p/>
     * <p>This default implementation merely returns
     * {@link AuthenticationInfo#getCredentials() account.getCredentials()} and exists as a template hook if subclasses
     * wish to obtain the credentials in a different way or convert them to a different format before
     * returning.
     *
     * @param info the {@code AuthenticationInfo} stored in the data store to be compared against the submitted authentication
     *             token's credentials.
     * @return the {@code account}'s associated credentials.
     */
    void getCredentials(const AuthenticationInfo& info, std::string* credentials);

    /**
     * Returns the {@code token}'s credentials.
     * <p/>
     * <p>This default implementation merely returns
     * {@link AuthenticationToken#getCredentials() authenticationToken.getCredentials()} and exists as a template hook
     * if subclasses wish to obtain the credentials in a different way or convert them to a different format before
     * returning.
     *
     * @param token the {@code AuthenticationToken} submitted during the authentication attempt.
     * @return the {@code token}'s associated credentials.
     */
    void getCredentials(const AuthenticationToken& token, std::string* credentials);

    /**
     * Hash the provided {@code token}'s credentials using the salt stored with the account if the
     * {@code info} instance is an {@code instanceof} {@link SaltedAuthenticationInfo SaltedAuthenticationInfo} (see
     * the class-level JavaDoc for why this is the preferred approach).
     * <p/>
     * If the {@code info} instance is <em>not</em>
     * an {@code instanceof} {@code SaltedAuthenticationInfo}, the logic will fall back to Shiro 1.0
     * backwards-compatible logic:  it will first check to see {@link #isHashSalted() isHashSalted} and if so, will try
     * to acquire the salt from {@link #getSalt(AuthenticationToken) getSalt(AuthenticationToken)}.  See the class-level
     * JavaDoc for why this is not recommended.  This 'fallback' logic exists only for backwards-compatibility.
     * {@code Realm}s should be updated as soon as possible to return {@code SaltedAuthenticationInfo} instances
     * if account credentials salting is enabled (highly recommended for password-based systems).
     *
     * @param token the submitted authentication token from which its credentials will be hashed
     * @param info  the stored account data, potentially used to acquire a salt
     * @return the token credentials hash
     * @since 1.1
     */
    void hashProvidedCredentials(const AuthenticationToken& token,
                                 const AuthenticationInfo& info,
                                 std::string* credentials);

    DigestEngine* getDigestEngine();

private:
    HashedCredentialsMatcherConfig config;

    DigestEngine* digestEngine;
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_AUTHC_HASHEDCREDENTIALSMATCHER_H)
