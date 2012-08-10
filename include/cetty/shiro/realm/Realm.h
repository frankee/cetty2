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

class CacheManager;

namespace cetty {
namespace shiro {
namespace realm {

/**
 * A simple implementation of the {@link Realm Realm} interface that
 * uses a set of configured user accounts and roles to support authentication and authorization.  Each account entry
 * specifies the username, password, and roles for a user.  Roles can also be mapped
 * to permissions and associated with users.
 * <p/>
 * User accounts and roles are stored in two {@code Map}s in memory, so it is expected that the total number of either
 * is not sufficiently large.
 *
 * @since 0.1
 */
class Realm{
public:
    Realm() {
        this->cachingEnabled = false;
        this->name = autoName();
    }

    /**
     * Returns the <tt>CacheManager</tt> used for data caching to reduce EIS round trips, or <tt>null</tt> if
     * caching is disabled.
     *
     * @return the <tt>CacheManager</tt> used for data caching to reduce EIS round trips, or <tt>null</tt> if
     *         caching is disabled.
     */
    const CacheManager &getCacheManager() const{
        return this->cacheManager;
    }

    /**
     * Sets the <tt>CacheManager</tt> to be used for data caching to reduce EIS round trips.
     * <p/>
     * <p>This property is <tt>null</tt> by default, indicating that caching is turned off.
     *
     * @param cacheManager the <tt>CacheManager</tt> to use for data caching, or <tt>null</tt> to disable caching.
     */
    void setCacheManager(const CacheManager &cacheManager) {
        this->cacheManager = cacheManager;
        afterCacheManagerSet();
    }

    /**
     * Returns {@code true} if caching should be used if a {@link CacheManager} has been
     * {@link #setCacheManager(org.apache.shiro.cache.CacheManager) configured}, {@code false} otherwise.
     * <p/>
     * The default value is {@code true} since the large majority of Realms will benefit from caching if a CacheManager
     * has been configured.  However, memory-only realms should set this value to {@code false} since they would
     * manage account data in memory already lookups would already be as efficient as possible.
     *
     * @return {@code true} if caching will be globally enabled if a {@link CacheManager} has been
     *         configured, {@code false} otherwise
     */
    bool isCachingEnabled() {
        return cachingEnabled;
    }

    /**
     * Sets whether or not caching should be used if a {@link CacheManager} has been
     * {@link #setCacheManager(org.apache.shiro.cache.CacheManager) configured}.
     *
     * @param cachingEnabled whether or not to globally enable caching for this realm.
     */
    void setCachingEnabled(bool cachingEnabled) {
        this->cachingEnabled = cachingEnabled;
    }

    const std::string &getName() {
        return name;
    }

    void setName(const std::string &name) {
        this->name = name;
    }

protected:
    void afterCacheManagerSet() {
    }

private:
    /*--------------------------------------------
    |    I N S T A N C E   V A R I A B L E S    |
    ============================================*/
    bool cachingEnabled;
    std::string name;

    CacheManager &cacheManager;

    /**
     *
     * Generator name based on the current time.
     */
    std::string autoName();
};
}
}
}

#endif // #if !defined(CETTY_SHIRO_REALM_REALM_H)
