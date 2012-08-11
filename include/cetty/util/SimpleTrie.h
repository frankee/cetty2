#if !defined(CETTY_UTIL_SIMPLETRIE_H)
#define CETTY_UTIL_SIMPLETRIE_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <boost/assert.hpp>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace util {

class SimpleTrieNode {
private:
    static const int MAX_CHAR_COUNT = 30;
    static const int ALPHABET_COUNT = 26;

public:
    int words;
    int prefixes;
    // . - _ ?
    SimpleTrieNode* references[MAX_CHAR_COUNT];
    void* data;

    SimpleTrieNode() : words(0), prefixes(0), data(NULL) {
        std::memset(references, 0, sizeof(references));
    }

    ~SimpleTrieNode() {
        for (int i = 0; i < MAX_CHAR_COUNT; ++i) {
            if (references[i]) {
                delete references[i];
                references[i] = NULL;
            }
        }
    }

    void freeData() {
        for (int i = 0; i < MAX_CHAR_COUNT; ++i) {
            if (references[i]) {
                references[i]->freeData();
            }
        }

        if (data) {
            delete data;
            data = NULL;
        }
    }

    bool hasReferenceTo(char c) {
        return references[getIndex(c)] != NULL;
    }

    static inline int getIndex(int c) {
        if (c == 46) { // '.'
            return ALPHABET_COUNT;
        }
        else if (c > 96 && c < 123) { // 'a~z'
            return c - 97;
        }
        else {
            return MAX_CHAR_COUNT - 1;
        }
    }
};

class SimpleTrie {
public:
    SimpleTrie();

    void addKey(const std::string& key, void* value);
    void addKey(const std::string& key);

    void* getValue(const std::string& key) const;
    void  freeValue() { root.freeData(); }

    // count the prefix and include the key.
    int count(const std::string& prefix) const;

    // only count the prefix, excluding the key.
    int countPrefix(const std::string& prefix) const;
    int countKey(const std::string& key) const;

private:
    void addKey(SimpleTrieNode& n, std::string* key, void* data);
    int countPrefix(const SimpleTrieNode& n, std::string* prefix, bool includeKey) const;

    const SimpleTrieNode* getLeafNode(const SimpleTrieNode& n, std::string* word) const;

private:
    SimpleTrieNode root;
};

}
}

#endif //#if !defined(CETTY_UTIL_SIMPLETRIE_H)

// Local Variables:
// mode: c++
// End:
