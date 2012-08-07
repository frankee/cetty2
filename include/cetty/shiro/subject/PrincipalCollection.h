#if !defined(CETTY_SHIRO_PRINCIPALCOLLECTION_H)
#define CETTY_SHIRO_PRINCIPALCOLLECTION_H
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

#include <map>
#include <set>
#include <string>

namespace cetty {
namespace shiro {
namespace subject {

/**
 * A simple implementation of the {@link MutablePrincipalCollection} interface that tracks principals internally
 * by storing them in a {@link LinkedHashMap}.
 *
 * @since 0.9
 */
class PrincipalCollection {
private:
    std::map<std::string, std::set<std::string> > realmPrincipals;

    std::string cachedToString; //cached toString() result, as this can be printed many times in logging

public:
    PrincipalCollection() {
    }

    PrincipalCollection(const std::string &principal, const std::string &realmName) {
    }

    PrincipalCollection(const PrincipalCollection &principals, const std::string &realmName) {
        realmPrincipals = new std::map<std::string, std::set<std::string> >();
        addAll(principals, realmName);
    }

    PrincipalCollection(const PrincipalCollection &principals) {
        realmPrincipals = new std::map<std::string, std::set<std::string> >();
        addAll(principals);
    }

    PrincipalCollection getPrincipalsLazy(std::string realmName);

    /**
     * Returns the first available principal from any of the {@code Realm} principals, or {@code null} if there are
     * no principals yet.
     * <p/>
     * The 'first available principal' is interpreted as the principal that would be returned by
     * <code>{@link #iterator() iterator()}.{@link java.util.Iterator#next() next()}.</code>
     *
     * @inheritDoc
     */
    const std::string &getPrimaryPrincipal() const;

    void add(const std::string &principal, const std::string &realName);

    void addAll(const PrincipalCollection &principals, const std::string &realmName);

    void addAll(const PrincipalCollection &principals);

    /**
     * Return the first principal
     */

    //const boost::any &oneByType() const;

    //public <T> Collection<T> byType(Class<T> type);

    const std::set<std::string> &asSet();

    const std::set<std::string> &fromRealm(std::string realmName);

    const std::set<std::string> &getRealmNames();

    bool isEmpty();

    void clear();

    std::map<std::string, std::set<std::string> >::iterator iterator();

    bool equals(const std::string &o);

    int hashCode();

    /**
     * Returns a simple string representation suitable for printing.
     *
     * @return a simple string representation suitable for printing.
     * @since 1.0
     */
    std::string toString();
    virtual ~PrincipalCollection(){}

private:
    /**
     * Serialization write support.
     * <p/>
     * NOTE: Don't forget to change the serialVersionUID constant at the top of this class
     * if you make any backwards-incompatible serializatoin changes!!!
     * (use the JDK 'serialver' program for this)
     *
     * @param out output stream provided by Java serialization
     * @throws IOException if there is a stream error
     */
    //void writeObject(ObjectOutputStream out);

    /**
     * Serialization read support - reads in the Map principals collection if it exists in the
     * input stream.
     * <p/>
     * NOTE: Don't forget to change the serialVersionUID constant at the top of this class
     * if you make any backwards-incompatible serializatoin changes!!!
     * (use the JDK 'serialver' program for this)
     *
     * @param in input stream provided by
     * @throws IOException            if there is an input/output problem
     * @throws ClassNotFoundException if the underlying Map implementation class is not available to the classloader.
     */
    //void readObject(ObjectInputStream in);
};
}
}
}
#endif // #if !defined(CETTY_SHIRO_PRINCIPALCOLLECTION_H)
