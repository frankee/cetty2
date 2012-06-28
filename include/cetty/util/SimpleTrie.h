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

#include <cetty/util/StringUtil.h>

namespace cetty { namespace util {

class SimpleTrieNode {
public:
    int words;
    int prefixes;
    SimpleTrieNode* references[26];

    SimpleTrieNode() : words(0), prefixes(0) {
        references = {0};
    }

    ~SimpleTrieNode() {

    }

    bool hasReferenceTo(char c) {
        return references[c - 'a'] != NULL;
    }
};

class SimpleTrie{
public:
    SimpleTrie();
    void addWord(const std::string& word);
    int countPrefix(const std::string& prefix) const;
    int countWord(const std::string& word) const;

private:
    void addWord(SimpleTrieNode &n, std::string* word);
    int countPrefix(const SimpleTrieNode &n, std::string* prefix) const;
    int countWord(const SimpleTrieNode &n, std::string* word) const;

    int getIndex(char c) { return c -'a'; }

private:
    SimpleTrieNode root;
};

SimpleTrie::SimpleTrie() {}

void SimpleTrie::addWord(SimpleTrieNode &n, std::string* word){
    BOOST_ASSERT(word);
    if (word->length() == 0){
        n.words += 1;
        return;
    }

    n.prefixes += 1;
    char first = word->front();
    int index = getIndex(first);
    if (!n.hasReferenceTo(first)){
        n.references[index] = new SimpleTrieNode();
    }

    addWord(*n.references[index], word->erase(0,1));
}

int SimpleTrie::countWord(const SimpleTrieNode &n, std::string* word) const {
    BOOST_ASSERT(word);

    if(word->length() == 0){
        return n.words;
    }

    char first = word[0];
    int index = getIndex(first);
    if(n.references[index] == NULL){
        return 0;
    }
    else {
        return countWord(*n.references[index], word->erase(0,1));
    }
}

int SimpleTrie::countPrefix(const SimpleTrieNode &n, std::string* word) const {
    BOOST_ASSERT(word);

    if(word->length() == 0){
        return n.prefixes;
    }

    char first = word[0];
    int index = getIndex(first);
    if(n.references[index] == NULL){
        return 0;
    }
    else {
        return countPrefix(*n.references[index], word->erase(0, 1));
    }
}

void SimpleTrie::addWord(const std::string& word) {
    std::string lower(word);
    StringUtil::strtolower(&lower);
    addWord(root, &lower);
}

int SimpleTrie::countPrefix(const std::string& prefix) const {
    std::string lower(prefix);
    StringUtil::strtolower(&lower);
    return countPrefix(root, &lower);
}

int SimpleTrie::countWord(const std::string& word) const {
    std::string lower;
    StringUtil::strtolower(&lower);
    return countWord(root, &lower);
}

}}

#endif //#if !defined(CETTY_UTIL_SIMPLETRIE_H)

// Local Variables:
// mode: c++
// End:
