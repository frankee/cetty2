#if !defined(CETTY_SHIRO_UTIL_MAPCONTEXT_H)
#define CETTY_SHIRO_UTIL_MAPCONTEXT_H
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
#include <cstdlib>
namespace cetty {
namespace shiro {
namespace util {

/**
 * A {@code MapContext} provides a common base for context-based data storage in a {@link Map}.  Type-safe attribute
 * retrieval is provided for subclasses with the {@link #getTypedValue(String, Class)} method.
 *
 * @see org.apache.shiro.subject.SubjectContext SubjectContext
 * @see org.apache.shiro.session.mgt.SessionContext SessionContext
 * @since 1.0
 */
class MapContext {
public:
    MapContext() {}

    int size() { return backingMap.size(); }
    bool isEmpty() { return backingMap.empty(); }
    bool containsKey(const std::string &key) { return backingMap.count(key) > 0; }

    template<typename T>
    bool containsValue(T *t) {
        std::map<std::string, boost::any *>::iterator it = backingMap.begin();
        boost::any *any = NULL;

        for(; it < backingMap.end(); ++it){
            any = it->second;
            if(any->type() == typeid(T)){
               T *value = boost::any_cast<T *>(any);
               if(t->equal(value)) return true;
            }

        }
        return false;
    }
    template<typename T>
    T *get(const std::string &key) {
       return getTypedValue<T>(key);
    }

    boost::any *put(const std::string &key, boost::any *value) {
        std::pair<std::map<std::string, boost::any *>::iterator, bool> ret =
            backingMap.insert(std::pair<std::string, boost::any *>(key, value));
        if(!ret.second) NULL;
        return ret.first->second;
    }

    boost::any * remove(const std::string &key) {
        std::map<std::string, boost::any *>::iterator it = backingMap.find(key);
        if(it == backingMap.end()) return NULL;
        boost::any *ret = it->second;
        backingMap.erase(it);
        return ret;
    }

    void putAll(const std::map<std::string, boost::any *> &map) {
        backingMap.insert(map.begin(), map.end());
    }

    void clear() {
       backingMap.clear();
    }

    std::vector<std::string> *keySet();

    std::vector<boost::any *> *values();
    std::vector<std::pair<std::string, boost::any *> > entrySet();

protected:
    /**
     * Performs a {@link #get get} operation but additionally ensures that the value returned is of the specified
     * {@code type}.  If there is no value, {@code null} is returned.
     *
     * @param key  the attribute key to look up a value
     * @param type the expected type of the value
     * @param <E>  the expected type of the value
     * @return the typed value or {@code null} if the attribute does not exist.
     */
    template<typename T>
    T *getTypedValue(const std::string &key) {
        std::map<std::string, boost::any *>::iterator it = backingMap.find(key);
        if(it == backingMap.end()) return NULL;
        T* t = boost::any_cast<T *>(it->second);
        if(t == NULL) return NULL;
        return t;
    }

    /**
     * Places a value in this context map under the given key only if the given {@code value} argument is not null.
     *
     * @param key   the attribute key under which the non-null value will be stored
     * @param value the non-null value to store.  If {@code null}, this method does nothing and returns immediately.
     */
    void nullSafePut(const std::string &key, boost::any *value) {
        if (key.size() > 0) {
            put(key, value);
        }
    }


private:
    std::map<std::string, boost::any *> backingMap;
};
}
}
}
#endif // #if !defined(CETTY_SHIRO_UTIL_MAPCONTEXT_H)
#define CETTY_SHIRO_UTIL_MAPCONTEXT_H
