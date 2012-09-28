#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIE_H)
#define CETTY_HANDLER_CODEC_HTTP_COOKIE_H

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

#include <string>
#include <vector>

namespace cetty {
namespace util {
class NameValueCollection;
}
}

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

/**
 * An HTTP <a href="http://en.wikipedia.org/wiki/HTTP_cookie">Cookie</a>.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class Cookie {
public:
    Cookie(const std::string& name);
    /// Creates a cookie with the given name.
    /// The cookie never expires.

    Cookie(const NameValueCollection& nvc);
    /// Creates a cookie from the given NameValueCollection.

    Cookie(const std::string& name, const std::string& value);
    /// Creates a cookie with the given name and value.
    /// The cookie never expires.
    ///
    /// Note: If value contains whitespace or non-alphanumeric
    /// characters, the value should be escaped by calling escape()
    /// before passing it to the constructor.

    Cookie(const Cookie& cookie);

    Cookie& operator=(const Cookie& cookie);

    bool operator==(const Cookie& cookie);
    bool operator<(const Cookie& cookie);

    ~Cookie() {}

    /**
     * Returns the name of this cookie.
     */
    const std::string& getName() const;

    /**
     * Returns the value of this cookie.
     */
    const std::string& getValue() const;

    /**
     * Sets the value of this cookie.
     */
    void setValue(const std::string& value);

    /**
     * Returns the domain of this cookie.
     */
    const std::string& getDomain() const;

    /**
     * Sets the domain of this cookie.
     */
    void setDomain(const std::string& domain);

    /**
     * Returns the path of this cookie.
     */
    const std::string& getPath() const;

    /**
     * Sets the path of this cookie.
     */
    void setPath(const std::string& path);

    /**
     * Returns the comment of this cookie.
     */
    const std::string& getComment() const;

    /**
     * Sets the comment of this cookie.
     */
    void setComment(const std::string& comment);

    /**
     * Returns the max age of this cookie in seconds.
     */
    int getMaxAge() const;

    /**
     * Sets the max age of this cookie in seconds.  If <tt>0</tt> is specified,
     * this cookie will be removed by browser because it will be expired
     * immediately.  If <tt>-1</tt> is specified, this cookie will be removed
     * when a user terminates browser.
     */
    void setMaxAge(int maxAge);

    /**
     * Returns the version of this cookie.
     */
    int getVersion() const;

    /**
     * Sets the version of this cookie.
     */
    void setVersion(int version);

    /**
     * Returns the secure flag of this cookie.
     */
    bool isSecure() const;

    /**
     * Sets the secure flag of this cookie.
     */
    void setSecure(bool secure);

    /**
     * Returns if this cookie cannot be accessed through client side script.
     * This flag works only if the browser supports it.  For more information,
     * see <a href="http://www.owasp.org/index.php/HTTPOnly">here</a>.
     */
    bool isHttpOnly() const;

    /**
     * Sets if this cookie cannot be accessed through client side script.
     * This flag works only if the browser supports it.  For more information,
     * see <a href="http://www.owasp.org/index.php/HTTPOnly">here</a>.
     */
    void setHttpOnly(bool httpOnly);

    /**
     * Returns the comment URL of this cookie.
     */
    const std::string& getCommentUrl() const;

    /**
     * Sets the comment URL of this cookie.
     */
    void setCommentUrl(const std::string& commentUrl);

    /**
     * Returns the discard flag of this cookie.
     */
    bool isDiscard() const;

    /**
     * Sets the discard flag of this cookie.
     */
    void setDiscard(bool discard);

    /**
     * Returns the ports of this cookie.
     */
    const std::vector<int>& getPorts() const;

    /**
     * Sets the ports of this cookie.
     */
    void setPorts(const std::vector<int>& ports);

    int compare(const Cookie& c) const;

    std::string toString() const;

private:
    bool validateName(const std::string& name);
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

inline const std::string& Cookie::getName() const { return name; }
inline const std::string& Cookie::getValue() const { return value; }
inline const std::string& Cookie::getDomain() const { return domain; }
inline const std::string& Cookie::getPath() const { return path; }
inline const std::string& Cookie::getComment() const { return comment; }
inline const std::string& Cookie::getCommentUrl() const { return commentUrl; }
inline const std::vector<int>& Cookie::getPorts() const { return ports; }
inline int  Cookie::getMaxAge() const { return maxAge; }
inline int  Cookie::getVersion() const { return version; }
inline bool Cookie::isDiscard() const { return discard; }
inline bool Cookie::isSecure() const { return secure; }
inline bool Cookie::isHttpOnly() const { return httpOnly; }

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIE_H)

// Local Variables:
// mode: c++
// End:
