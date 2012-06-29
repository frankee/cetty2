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

#include <cetty/util/SimpleTrie.h>

namespace cetty {
namespace util {

SimpleTrie::SimpleTrie() {}

void SimpleTrie::addKey(SimpleTrieNode& n, std::string* word, void* data) {
    BOOST_ASSERT(word);

    if (word->length() == 0) {
        n.words += 1;
        n.data = data;
        return;
    }

    n.prefixes += 1;
    char first = word->front();
    int index = SimpleTrieNode::getIndex(first);

    if (!n.hasReferenceTo(first)) {
        n.references[index] = new SimpleTrieNode();
    }

    word->erase(0,1);
    addKey(*n.references[index], word, data);
}

const SimpleTrieNode* SimpleTrie::getLeafNode(const SimpleTrieNode& n,
        std::string* word) const {
    BOOST_ASSERT(word);

    if (word->length() == 0) {
        return &n;
    }

    char first = word->front();
    int index = SimpleTrieNode::getIndex(first);

    if (n.references[index] == NULL) {
        return NULL;
    }
    else {
        word->erase(0,1);
        return getLeafNode(*n.references[index], word);
    }
}

int SimpleTrie::countPrefix(const SimpleTrieNode& n, std::string* word) const {
    BOOST_ASSERT(word);

    if (word->length() == 0) {
        return n.prefixes;
    }

    char first = word->front();
    int index = SimpleTrieNode::getIndex(first);

    if (n.references[index] == NULL) {
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

}
}
