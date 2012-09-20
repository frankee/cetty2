#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPVERSION_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPVERSION_H

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

namespace cetty {
namespace util {
class StringPiece;
}
}

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

/**
 * The version of HTTP or its derived protocols, such as
 * <a href="http://en.wikipedia.org/wiki/Real_Time_Streaming_Protocol">RTSP</a> and
 * <a href="http://en.wikipedia.org/wiki/Internet_Content_Adaptation_Protocol">ICAP</a>.
 *
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.exclude
 */

class HttpVersion {
public:
    /**
     * HTTP/1.0
     */
    static HttpVersion HTTP_1_0;

    /**
     * HTTP/1.1
     */
    static HttpVersion HTTP_1_1;

    /**
     * Returns an existing or new {@link HttpVersion} instance which matches to
     * the specified protocol version string.  If the specified <tt>text</tt> is
     * equal to <tt>"HTTP/1.0"</tt>, {@link #HTTP_1_0} will be returned.  If the
     * specified <tt>text</tt> is equal to <tt>"HTTP/1.1"</tt>, {@link #HTTP_1_1}
     * will be returned.  Otherwise, a new {@link HttpVersion} instance will be
     * returned.
     */
    static HttpVersion valueOf(const std::string& text);

    static HttpVersion valueOf(const StringPiece& text);

    /**
     * Creates a new HTTP version with the specified version string.  You will
     * not need to create a new instance unless you are implementing a protocol
     * derived from HTTP, such as
     * <a href="http://en.wikipedia.org/wiki/Real_Time_Streaming_Protocol">RTSP</a> and
     * <a href="http://en.wikipedia.org/wiki/Internet_Content_Adaptation_Protocol">ICAP</a>.
     *
     * @param keepAliveDefault
     *        <tt>true</tt> if and only if the connection is kept alive unless
     *        the <tt>"Connection"</tt> header is set to <tt>"close"</tt> explicitly.
     */
    HttpVersion(const std::string& text, bool keepAliveDefault);

    HttpVersion(const StringPiece& text, bool keepAliveDefault);

    /**
     * Creates a new HTTP version with the specified protocol name and version
     * numbers.  You will not need to create a new instance unless you are
     * implementing a protocol derived from HTTP, such as
     * <a href="http://en.wikipedia.org/wiki/Real_Time_Streaming_Protocol">RTSP</a> and
     * <a href="http://en.wikipedia.org/wiki/Internet_Content_Adaptation_Protocol">ICAP</a>
     *
     * @param keepAliveDefault
     *        <tt>true</tt> if and only if the connection is kept alive unless
     *        the <tt>"Connection"</tt> header is set to <tt>"close"</tt> explicitly.
     */
    HttpVersion(const std::string& protocolName,
                int majorVersion,
                int minorVersion,
                bool keepAliveDefault);

    bool operator ==(const HttpVersion& version) const {
        return equals(version);
    }
    bool operator < (const HttpVersion& version) const {
        return compareTo(version) < 0;
    }

    /**
     * Returns the name of the protocol such as <tt>"HTTP"</tt> in <tt>"HTTP/1.0"</tt>.
     */
    const std::string& getProtocolName() const {
        return this->protocolName;
    }

    /**
     * Returns the name of the protocol such as <tt>1</tt> in <tt>"HTTP/1.0"</tt>.
     */
    int getMajorVersion() const {
        return this->majorVersion;
    }

    /**
     * Returns the name of the protocol such as <tt>0</tt> in <tt>"HTTP/1.0"</tt>.
     */
    int getMinorVersion() const {
        return this->minorVersion;
    }

    /**
     * Returns the full protocol version text such as <tt>"HTTP/1.0"</tt>.
     */
    const std::string& getText() const {
        return this->text;
    }

    /**
     * Returns <tt>true</tt> if and only if the connection is kept alive unless
     * the <tt>"Connection"</tt> header is set to <tt>"close"</tt> explicitly.
     */
    bool isKeepAliveDefault() const {
        return this->keepAliveDefault;
    }

    /**
     * Returns the full protocol version text such as <tt>"HTTP/1.0"</tt>.
     */
    std::string toString() const {
        return getText();
    }

    int hashCode() const;

    bool equals(const HttpVersion& verion) const;
    int  compareTo(const HttpVersion& version) const;

private:
    bool match(const std::string& text,
               std::string& protocol,
               std::string& major,
               std::string& minor);

    bool verifyProtocl(const std::string& protocolName);
    bool verifyVersion(const std::string& version);

private:
    bool keepAliveDefault;
    int majorVersion;
    int minorVersion;
    std::string protocolName;
    std::string text;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPVERSION_H)

// Local Variables:
// mode: c++
// End:
