#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTCOOKIE_H)
#define CETTY_HANDLER_CODEC_HTTP_DEFAULTCOOKIE_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
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

#include <cetty/handler/codec/http/Cookie.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

/**
 * The default {@link Cookie} implementation.
 *
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class DefaultCookie : public Cookie {
public:
    /**
     * Creates a new cookie with the specified name and value.
     */
    explicit Cookie(const std::string& name);
    /// Creates a cookie with the given name.	
    /// The cookie never expires.

    explicit Cookie(const NameValueCollection& nvc);
    /// Creates a cookie from the given NameValueCollection.

    Cookie(const std::string& name, const std::string& value);
    /// Creates a cookie with the given name and value.
    /// The cookie never expires.
    ///
    /// Note: If value contains whitespace or non-alphanumeric
    /// characters, the value should be escaped by calling escape()
    /// before passing it to the constructor.
    DefaultCookie(const std::string& name, const std::string& value);

    virtual ~DefaultCookie() {}

    virtual const std::string& getName() const;

    virtual const std::string& getValue() const;
    virtual void setValue(const std::string& value);

    virtual const std::string& getDomain() const;
    virtual void setDomain(const std::string& domain);

    virtual const std::string& getPath() const;
    virtual void setPath(const std::string& path);

    virtual const std::string& getComment() const;
    virtual void setComment(const std::string& comment);

    virtual const std::string& getCommentUrl() const;
    virtual void setCommentUrl(const std::string& commentUrl);

    virtual bool isDiscard() const;
    virtual void setDiscard(bool discard);

    virtual const std::vector<int>& getPorts() const;
    virtual void setPorts(const std::vector<int>& ports);

    virtual int getMaxAge() const;
    virtual void setMaxAge(int maxAge);

    virtual int getVersion() const;
    virtual void setVersion(int version);

    virtual bool isSecure() const;
    virtual void setSecure(bool secure);

    virtual bool isHttpOnly() const;
    virtual void setHttpOnly(bool httpOnly);

    bool equals(const Cookie& that) const;
    int compareTo(const Cookie& c) const;

    std::string toString() const;

private:
    void validateValue(const std::string& name, const std::string& value, std::string* output) const;

private:
    bool secure;
    bool httpOnly;
    bool discard;

    int maxAge;
    int version;

    std::string name;
    std::string value;
    std::string domain;
    std::string path;
    std::string comment;
    std::string commentUrl;

    std::vector<int> ports;
    std::vector<int> unmodifiablePorts;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTCOOKIE_H)

// Local Variables:
// mode: c++
// End:
