#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPHEADER_H)
#define CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPHEADER_H

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/handler/codec/http/HttpHeader.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class DefaultHttpHeader : public HttpHeader {
public:
    DefaultHttpHeader();
    virtual ~DefaultHttpHeader();

    virtual void validateHeaderName(const std::string& name);

    virtual const std::string& get(const std::string& name) const;

    virtual StringList gets(const std::string& name) const;
    virtual void gets(const std::string& name, StringList& headers) const;

    virtual NameValueList gets() const;
    virtual void gets(NameValueList& nameValues) const;

    virtual StringList getNames() const;
    virtual void getNames(StringList& names) const;

    virtual bool contains(const std::string& name) const;

    virtual void add(const std::string& name, const std::string& value);
    virtual void add(const std::string& name, int value);

    virtual void set(const std::string& name, const std::string& value);
    virtual void set(const std::string& name, int value);

    virtual void set(const std::string& name, const StringList& values);
    virtual void set(const std::string& name, const IntList& values);

    virtual void remove(const std::string& name);
    virtual void remove(const std::string& name, const std::string& value);
    virtual void clear();

private:
    class Entry {
    public:
        int hash;
        std::string key;
        std::string value;
        Entry* next;
        Entry* before;
        Entry* after;

        Entry() : hash(-1), key(), value(), next(NULL), before(NULL), after(NULL) {}
        Entry(int hash, const std::string& key, const std::string& value)
            : hash(hash), key(key), value(value), next(NULL), before(NULL), after(NULL) {
        }

        void remove() {
            before->after = after;
            after->before = before;
        }

        void addBefore(Entry* e) {
            after  = e;
            before = e->before;
            before->after = this;
            after->before = this;
        }

        const std::string& getKey() const {
            return key;
        }

        const std::string& getValue() const {
            return value;
        }

        void setValue(const std::string& value) {
            this->value = value;
        }

        void set(int hash, const std::string& key, const std::string& value) {
            this->hash = hash;
            this->key = key;
            this->value = value;
        }

        std::string toString() {
            std::string buf(key);
            buf += "=";
            buf += value;
            return buf;
        }

        void clear() {
            key.clear();
            value.clear();
            next = NULL;
        }

        bool empty() const {
            return key.empty();
        }

    private:
        Entry(const Entry&);
        Entry& operator=(const Entry&);
    };

private:
    Entry* newEntry(int h, const std::string& name, const std::string& value);
    void deleteEntry(Entry* entry);
    void clearFreeList();

    void addHeader(int h, int i, const std::string& name, const std::string& value);
    void removeHeader(int h, int i, const std::string& name);
    void removeHeader(int h, int i, const std::string& name, const std::string& value);

private:
    DefaultHttpHeader(const DefaultHttpHeader&);
    DefaultHttpHeader& operator=(const DefaultHttpHeader&);

private:
    static const int BUCKET_SIZE = 17;
    static const std::string EMPTY_VALUE;

    static int  hash(const std::string& name);
    static bool eq(const std::string& name1, const std::string& name2);
    static int  index(int hash);

    Entry* entries[BUCKET_SIZE];
    Entry* head;
    Entry* freelist;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTHTTPHEADER_H)

// Local Variables:
// mode: c++
// End:

