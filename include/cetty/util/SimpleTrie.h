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

namespace cetty { namespace util {

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
    void freeValue() {
        root.freeData();
    }

    int countPrefix(const std::string& prefix) const;
    int countKey(const std::string& key) const;

private:
    void addKey(SimpleTrieNode &n, std::string* key, void* data);
    int countPrefix(const SimpleTrieNode &n, std::string* prefix) const;
    
    const SimpleTrieNode* getLeafNode(const SimpleTrieNode &n, std::string* word) const;

private:
    SimpleTrieNode root;
};

SimpleTrie::SimpleTrie() {}

void SimpleTrie::addKey(SimpleTrieNode &n, std::string* word, void* data) {
    BOOST_ASSERT(word);
    if (word->length() == 0){
        n.words += 1;
        n.data = data;
        return;
    }

    n.prefixes += 1;
    char first = word->front();
    int index = SimpleTrieNode::getIndex(first);
    if (!n.hasReferenceTo(first)){
        n.references[index] = new SimpleTrieNode();
    }

    word->erase(0,1);
    addKey(*n.references[index], word, data);
}

const SimpleTrieNode* SimpleTrie::getLeafNode(const SimpleTrieNode &n,
    std::string* word) const {
        BOOST_ASSERT(word);

        if(word->length() == 0){
            return &n;
        }

        char first = word->front();
        int index = SimpleTrieNode::getIndex(first);
        if(n.references[index] == NULL){
            return NULL;
        }
        else {
            word->erase(0,1);
            return getLeafNode(*n.references[index], word);
        }
}

int SimpleTrie::countPrefix(const SimpleTrieNode &n, std::string* word) const {
    BOOST_ASSERT(word);

    if(word->length() == 0){
        return n.prefixes;
    }

    char first = word->front();
    int index = SimpleTrieNode::getIndex(first);
    if(n.references[index] == NULL){
        return 0;
    }
    else {
        word->erase(0, 1);
        return countPrefix(*n.references[index], word);
    }
}

void SimpleTrie::addKey(const std::string& word) {
    std::string lower(word);
    StringUtil::strtolower(&lower);
    addKey(root, &lower, NULL);
}

void SimpleTrie::addKey(const std::string& word, void* data) {
    std::string lower(word);
    StringUtil::strtolower(&lower);
    addKey(root, &lower, data);
}

int SimpleTrie::countPrefix(const std::string& prefix) const {
    std::string lower(prefix);
    StringUtil::strtolower(&lower);
    return countPrefix(root, &lower);
}

int SimpleTrie::countKey(const std::string& word) const {
    std::string lower;
    StringUtil::strtolower(&lower);

    const SimpleTrieNode* node = getLeafNode(root, &lower);
    return node ? node->words : 0;
}

void* SimpleTrie::getValue(const std::string& key) const {
    std::string lower;
    StringUtil::strtolower(&lower);

    const SimpleTrieNode* node = getLeafNode(root, &lower);
    return node ? node->data : 0;
}

}}

#endif //#if !defined(CETTY_UTIL_SIMPLETRIE_H)

// Local Variables:
// mode: c++
// End:
