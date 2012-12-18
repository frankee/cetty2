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

#include <cstring>
#include <boost/assert.hpp>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace util {

template<typename T>
class SimpleTrieNode {
private:
    static const int MAX_CHAR_COUNT = 30;
    static const int ALPHABET_COUNT = 26;

public:
    int words;
    int prefixes;
    // . - _ ?
    SimpleTrieNode* references[MAX_CHAR_COUNT];
    T* data;

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

template<typename T>
class SimpleTrie {
public:
    typedef SimpleTrieNode<T> Node;

public:
    SimpleTrie() {}

    void  freeData() { root.freeData(); }

    void addKey(const std::string& key, T* value);
    void addKey(const std::string& key);

    T* getValue(const std::string& key) const;

    // count the prefix and include the key.
    int count(const std::string& prefix) const;

    // only count the prefix, excluding the key.
    int countPrefix(const std::string& prefix) const;
    int countKey(const std::string& key) const;

private:
    void addKey(Node& n, std::string* key, T* data);
    int countPrefix(const Node& n, std::string* prefix, bool includeKey) const;

    const Node* getLeafNode(const Node& n, std::string* word) const;

private:
    Node root;
};

template<typename T>
void SimpleTrie<T>::addKey(Node& n, std::string* word, T* data) {
    BOOST_ASSERT(word);

    if (word->empty()) {
        n.words += 1;
        n.data = data;
        return;
    }

    n.prefixes += 1;
    char first = (*word)[0];
    int index = Node::getIndex(first);

    if (!n.hasReferenceTo(first)) {
        n.references[index] = new Node();
    }

    word->erase(0, 1);
    addKey(*n.references[index], word, data);
}

template<typename T> inline
const SimpleTrieNode<T>* SimpleTrie<T>::getLeafNode(const Node& n,
        std::string* word) const {
    BOOST_ASSERT(word);

    if (word->empty()) {
        return &n;
    }

    char first = (*word)[0];
    int index = Node::getIndex(first);

    if (n.references[index] == NULL) {
        return NULL;
    }
    else {
        word->erase(0, 1);
        return getLeafNode(*n.references[index], word);
    }
}

template<typename T> inline
int SimpleTrie<T>::countPrefix(const Node& n, std::string* word, bool includeKey) const {
    BOOST_ASSERT(word);

    if (word->empty()) {
        if (includeKey) {
            return n.prefixes + n.words;
        }

        return n.prefixes;
    }

    char first = (*word)[0];
    int index = Node::getIndex(first);

    if (n.references[index] == NULL) {
        return 0;
    }
    else {
        word->erase(0, 1);
        return countPrefix(*n.references[index], word, includeKey);
    }
}

template<typename T> inline
void SimpleTrie<T>::addKey(const std::string& word) {
    std::string lower(word);
    StringUtil::toLower(&lower);
    addKey(root, &lower, NULL);
}

template<typename T> inline
void SimpleTrie<T>::addKey(const std::string& word, T* data) {
    std::string lower(word);
    StringUtil::toLower(&lower);
    addKey(root, &lower, data);
}

template<typename T> inline
int SimpleTrie<T>::countPrefix(const std::string& prefix) const {
    std::string lower(prefix);
    StringUtil::toLower(&lower);
    return countPrefix(root, &lower, false);
}

template<typename T> inline
int SimpleTrie<T>::count(const std::string& prefix) const {
    std::string lower(prefix);
    StringUtil::toLower(&lower);
    return countPrefix(root, &lower, true);
}

template<typename T> inline
int SimpleTrie<T>::countKey(const std::string& word) const {
    std::string lower(word);
    StringUtil::toLower(&lower);

    const Node* node = getLeafNode(root, &lower);
    return node ? node->words : 0;
}

template<typename T> inline
T* SimpleTrie<T>::getValue(const std::string& key) const {
    std::string lower(key);
    StringUtil::toLower(&lower);

    const Node* node = getLeafNode(root, &lower);
    return node ? node->data : 0;
}

}
}

#endif //#if !defined(CETTY_UTIL_SIMPLETRIE_H)

// Local Variables:
// mode: c++
// End:
