#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPHEADER_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPHEADER_H

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

#include <string>
#include <vector>
#include <utility>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpHeader {
public:
    typedef std::pair<std::string, std::string> NameValuePair;
    typedef std::vector<NameValuePair> NameValueList;
    typedef std::vector<NameValuePair>::iterator NameValueListItr;
    typedef std::vector<NameValuePair>::const_iterator NameValueListConstItr;
    typedef std::vector<std::string> StringList;
    typedef std::vector<int> IntList;

public:
    virtual ~HttpHeader() {}

    /**
     * Returns the header value with the specified header name.  If there are
     * more than one header value for the specified header name, the first
     * value is returned.
     *
     * @return the header value or <tt>null</tt> if there is no such header
     *
     */
    virtual const std::string& get(const std::string& name) const = 0;

    /**
     * Returns the header values with the specified header name.
     *
     * @return the {@link List} of header values.  An empty list if there is no
     *         such header.
     */
    virtual StringList gets(const std::string& name) const = 0;
    virtual void gets(const std::string& name, StringList& headers) const = 0;

    /**
     * Returns the all header names and values that this message contains.
     *
     * @return the {@link List} of the header name-value pairs.  An empty list
     *         if there is no header in this message.
     */
    virtual NameValueList gets() const = 0;
    virtual void gets(NameValueList& nameValues) const = 0;

    /**
     * Returns <tt>true</tt> if and only if there is a header with the specified
     * header name.
     */
    virtual bool contains(const std::string& name) const = 0;

    /**
     * Returns the {@link Set} of all header names that this message contains.
     */
    virtual StringList getNames() const = 0;
    virtual void getNames(StringList& names) const = 0;

    /**
     * Adds a new header with the specified name and value.
     */
    virtual void add(const std::string& name, const std::string& value) = 0;
    virtual void add(const std::string& name, int value) = 0;

    /**
     * Sets a new header with the specified name and value.  If there is an
     * existing header with the same name, the existing header is removed.
     */
    virtual void set(const std::string& name, const std::string& value) = 0;
    virtual void set(const std::string& name, int value) = 0;

    /**
     * Sets a new header with the specified name and values.  If there is an
     * existing header with the same name, the existing header is removed.
     */
    virtual void set(const std::string& name, const StringList& values) = 0;
    virtual void set(const std::string& name, const IntList& values) = 0;

    /**
     * Removes the header with the specified name.
     */
    virtual void remove(const std::string& name) = 0;

    /**
     * Removes the header with the specified name and value.
     * If only one value with the name, then move the name item completely,
     * otherwise only remove the value.
     */
    virtual void remove(const std::string& name, const std::string& value) = 0;

    /**
     * Removes all headers from this message.
     */
    virtual void clear() = 0;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPHEADER_H)

// Local Variables:
// mode: c++
// End:
