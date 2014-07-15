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

#include <cetty/handler/codec/http/HttpHeaders.h>

#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpResponseStatus.h>
#include <cetty/handler/codec/http/HttpMethod.h>

#include <cetty/util/StringUtil.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

/**
 * <tt>"Accept"</tt>
 */
const std::string HttpHeaders::Names::ACCEPT = "Accept";
/**
 * <tt>"Accept-Charset"</tt>
 */
const std::string HttpHeaders::Names::ACCEPT_CHARSET = "Accept-Charset";
/**
 * <tt>"Accept-Encoding"</tt>
 */
const std::string HttpHeaders::Names::ACCEPT_ENCODING= "Accept-Encoding";
/**
 * <tt>"Accept-Language"</tt>
 */
const std::string HttpHeaders::Names::ACCEPT_LANGUAGE = "Accept-Language";
/**
 * <tt>"Accept-Ranges"</tt>
 */
const std::string HttpHeaders::Names::ACCEPT_RANGES= "Accept-Ranges";
/**
 * <tt>"Accept-Patch"</tt>
 */
const std::string HttpHeaders::Names::ACCEPT_PATCH= "Accept-Patch";
/**
 * <tt>"Age"</tt>
 */
const std::string HttpHeaders::Names::AGE = "Age";
/**
 * <tt>"Allow"</tt>
 */
const std::string HttpHeaders::Names::ALLOW = "Allow";
/**
 * <tt>"Authorization"</tt>
 */
const std::string HttpHeaders::Names::AUTHORIZATION = "Authorization";
/**
 * <tt>"Cache-Control"</tt>
 */
const std::string HttpHeaders::Names::CACHE_CONTROL = "Cache-Control";
/**
 * <tt>"Connection"</tt>
 */
const std::string HttpHeaders::Names::CONNECTION = "Connection";
/**
 * <tt>"Content-Base"</tt>
 */
const std::string HttpHeaders::Names::CONTENT_BASE = "Content-Base";
/**
 * <tt>"Content-Encoding"</tt>
 */
const std::string HttpHeaders::Names::CONTENT_ENCODING = "Content-Encoding";
/**
 * <tt>"Content-Language"</tt>
 */
const std::string HttpHeaders::Names::CONTENT_LANGUAGE= "Content-Language";
/**
 * <tt>"Content-Length"</tt>
 */
const std::string HttpHeaders::Names::CONTENT_LENGTH = "Content-Length";
/**
 * <tt>"Content-Location"</tt>
 */
const std::string HttpHeaders::Names::CONTENT_LOCATION = "Content-Location";
/**
 * <tt>"Content-Transfer-Encoding"</tt>
 */
const std::string HttpHeaders::Names::CONTENT_TRANSFER_ENCODING = "Content-Transfer-Encoding";
/**
 * <tt>"Content-MD5"</tt>
 */
const std::string HttpHeaders::Names::CONTENT_MD5 = "Content-MD5";
/**
 * <tt>"Content-Range"</tt>
 */
const std::string HttpHeaders::Names::CONTENT_RANGE = "Content-Range";
/**
 * <tt>"Content-Type"</tt>
 */
const std::string HttpHeaders::Names::CONTENT_TYPE= "Content-Type";
/**
 * <tt>"Cookie"</tt>
 */
const std::string HttpHeaders::Names::COOKIE = "Cookie";
/**
 * <tt>"Date"</tt>
 */
const std::string HttpHeaders::Names::DATE = "Date";
/**
 * <tt>"ETag"</tt>
 */
const std::string HttpHeaders::Names::ETAG = "ETag";
/**
 * <tt>"Expect"</tt>
 */
const std::string HttpHeaders::Names::EXPECT = "Expect";
/**
 * <tt>"Expires"</tt>
 */
const std::string HttpHeaders::Names::EXPIRES = "Expires";
/**
 * <tt>"From"</tt>
 */
const std::string HttpHeaders::Names::FROM = "From";
/**
 * <tt>"Host"</tt>
 */
const std::string HttpHeaders::Names::HOST = "Host";
/**
 * <tt>"If-Match"</tt>
 */
const std::string HttpHeaders::Names::IF_MATCH = "If-Match";
/**
 * <tt>"If-Modified-Since"</tt>
 */
const std::string HttpHeaders::Names::IF_MODIFIED_SINCE = "If-Modified-Since";
/**
 * <tt>"If-None-Match"</tt>
 */
const std::string HttpHeaders::Names::IF_NONE_MATCH = "If-None-Match";
/**
 * <tt>"If-Range"</tt>
 */
const std::string HttpHeaders::Names::IF_RANGE= "If-Range";
/**
 * <tt>"If-Unmodified-Since"</tt>
 */
const std::string HttpHeaders::Names::IF_UNMODIFIED_SINCE = "If-Unmodified-Since";
/**
 * <tt>"Last-Modified"</tt>
 */
const std::string HttpHeaders::Names::LAST_MODIFIED = "Last-Modified";
/**
 * <tt>"Location"</tt>
 */
const std::string HttpHeaders::Names::LOCATION = "Location";
/**
 * <tt>"Max-Forwards"</tt>
 */
const std::string HttpHeaders::Names::MAX_FORWARDS = "Max-Forwards";
/**
 * <tt>"Origin"</tt>
 */
const std::string HttpHeaders::Names::ORIGIN = "Origin";
/**
 * <tt>"Pragma"</tt>
 */
const std::string HttpHeaders::Names::PRAGMA = "Pragma";
/**
 * <tt>"Proxy-Authenticate"</tt>
 */
const std::string HttpHeaders::Names::PROXY_AUTHENTICATE = "Proxy-Authenticate";
/**
 * <tt>"Proxy-Authorization"</tt>
 */
const std::string HttpHeaders::Names::PROXY_AUTHORIZATION = "Proxy-Authorization";
/**
 * <tt>"Range"</tt>
 */
const std::string HttpHeaders::Names::RANGE = "Range";
/**
 * <tt>"Referer"</tt>
 */
const std::string HttpHeaders::Names::REFERER = "Referer";
/**
 * <tt>"Retry-After"</tt>
 */
const std::string HttpHeaders::Names::RETRY_AFTER = "Retry-After";
/**
 * <tt>"Sec-WebSocket-Key1"</tt>
 */
const std::string HttpHeaders::Names::SEC_WEBSOCKET_KEY1 = "Sec-WebSocket-Key1";
/**
 * <tt>"Sec-WebSocket-Key2"</tt>
 */
const std::string HttpHeaders::Names::SEC_WEBSOCKET_KEY2 = "Sec-WebSocket-Key2";
/**
 * <tt>"Sec-WebSocket-Location"</tt>
 */
const std::string HttpHeaders::Names::SEC_WEBSOCKET_LOCATION = "Sec-WebSocket-Location";
/**
 * <tt>"Sec-WebSocket-Origin"</tt>
 */
const std::string HttpHeaders::Names::SEC_WEBSOCKET_ORIGIN = "Sec-WebSocket-Origin";
/**
 * <tt>"Sec-WebSocket-Protocol"</tt>
 */
const std::string HttpHeaders::Names::SEC_WEBSOCKET_PROTOCOL = "Sec-WebSocket-Protocol";
/**
 * {@code "Sec-WebSocket-Version"}
 */
const std::string HttpHeaders::Names::SEC_WEBSOCKET_VERSION = "Sec-WebSocket-Version";
/**
 * {@code "Sec-WebSocket-Key"}
 */
const std::string HttpHeaders::Names::SEC_WEBSOCKET_KEY = "Sec-WebSocket-Key";
/**
 * {@code "Sec-WebSocket-Accept"}
 */
const std::string HttpHeaders::Names::SEC_WEBSOCKET_ACCEPT = "Sec-WebSocket-Accept";
/**
 * <tt>"Server"</tt>
 */
const std::string HttpHeaders::Names::SERVER = "Server";
/**
 * <tt>"Set-Cookie"</tt>
 */
const std::string HttpHeaders::Names::SET_COOKIE = "Set-Cookie";
/**
 * <tt>"Set-Cookie2"</tt>
 */
const std::string HttpHeaders::Names::SET_COOKIE2 = "Set-Cookie2";
/**
 * <tt>"TE"</tt>
 */
const std::string HttpHeaders::Names::TE = "TE";
/**
 * <tt>"Trailer"</tt>
 */
const std::string HttpHeaders::Names::TRAILER = "Trailer";
/**
 * <tt>"Transfer-Encoding"</tt>
 */
const std::string HttpHeaders::Names::TRANSFER_ENCODING = "Transfer-Encoding";
/**
 * <tt>"Upgrade"</tt>
 */
const std::string HttpHeaders::Names::UPGRADE = "Upgrade";
/**
 * <tt>"User-Agent"</tt>
 */
const std::string HttpHeaders::Names::USER_AGENT = "User-Agent";
/**
 * <tt>"Vary"</tt>
 */
const std::string HttpHeaders::Names::VARY = "Vary";
/**
 * <tt>"Via"</tt>
 */
const std::string HttpHeaders::Names::VIA = "Via";
/**
 * <tt>"Warning"</tt>
 */
const std::string HttpHeaders::Names::WARNING = "Warning";
/**
 * <tt>"WebSocket-Location"</tt>
 */
const std::string HttpHeaders::Names::WEBSOCKET_LOCATION = "WebSocket-Location";
/**
 * <tt>"WebSocket-Origin"</tt>
 */
const std::string HttpHeaders::Names::WEBSOCKET_ORIGIN = "WebSocket-Origin";
/**
 * <tt>"WebSocket-Protocol"</tt>
 */
const std::string HttpHeaders::Names::WEBSOCKET_PROTOCOL = "WebSocket-Protocol";
/**
 * <tt>"WWW-Authenticate"</tt>
 */
const std::string HttpHeaders::Names::WWW_AUTHENTICATE = "WWW-Authenticate";

/**
 * <tt>"base64"</tt>
 */
const std::string HttpHeaders::Values::BASE64 = "base64";
/**
 * <tt>"binary"</tt>
 */
const std::string HttpHeaders::Values::BINARY = "binary";
/**
 * <tt>"bytes"</tt>
 */
const std::string HttpHeaders::Values::BYTES = "bytes";
/**
 * <tt>"charset"</tt>
 */
const std::string HttpHeaders::Values::CHARSET = "charset";
/**
 * <tt>"chunked"</tt>
 */
const std::string HttpHeaders::Values::CHUNKED = "chunked";
/**
 * <tt>"close"</tt>
 */
const std::string HttpHeaders::Values::CLOSE = "close";
/**
 * <tt>"compress"</tt>
 */
const std::string HttpHeaders::Values::COMPRESS = "compress";
/**
 * <tt>"100-continue"</tt>
 */
const std::string HttpHeaders::Values::CONTINUE =  "100-continue";
/**
 * <tt>"deflate"</tt>
 */
const std::string HttpHeaders::Values::DEFLATE = "deflate";
/**
 * <tt>"gzip"</tt>
 */
const std::string HttpHeaders::Values::GZIP = "gzip";
/**
 * <tt>"identity"</tt>
 */
const std::string HttpHeaders::Values::IDENTITY = "identity";
/**
 * <tt>"keep-alive"</tt>
 */
const std::string HttpHeaders::Values::KEEP_ALIVE = "keep-alive";
/**
 * <tt>"max-age"</tt>
 */
const std::string HttpHeaders::Values::MAX_AGE = "max-age";
/**
 * <tt>"max-stale"</tt>
 */
const std::string HttpHeaders::Values::MAX_STALE = "max-stale";
/**
 * <tt>"min-fresh"</tt>
 */
const std::string HttpHeaders::Values::MIN_FRESH = "min-fresh";
/**
 * <tt>"must-revalidate"</tt>
 */
const std::string HttpHeaders::Values::MUST_REVALIDATE = "must-revalidate";
/**
 * <tt>"no-cache"</tt>
 */
const std::string HttpHeaders::Values::NO_CACHE = "no-cache";
/**
 * <tt>"no-store"</tt>
 */
const std::string HttpHeaders::Values::NO_STORE = "no-store";
/**
 * <tt>"no-transform"</tt>
 */
const std::string HttpHeaders::Values::NO_TRANSFORM = "no-transform";
/**
 * <tt>"none"</tt>
 */
const std::string HttpHeaders::Values::NONE = "none";
/**
 * <tt>"only-if-cached"</tt>
 */
const std::string HttpHeaders::Values::ONLY_IF_CACHED = "only-if-cached";
/**
 * <tt>"private"</tt>
 */
const std::string HttpHeaders::Values::PRIVATE = "private";
/**
 * <tt>"proxy-revalidate"</tt>
 */
const std::string HttpHeaders::Values::PROXY_REVALIDATE = "proxy-revalidate";
/**
 * <tt>"public"</tt>
 */
const std::string HttpHeaders::Values::PUBLIC = "public";
/**
 * <tt>"quoted-printable"</tt>
 */
const std::string HttpHeaders::Values::QUOTED_PRINTABLE = "quoted-printable";
/**
 * <tt>"s-maxage"</tt>
 */
const std::string HttpHeaders::Values::S_MAXAGE = "s-maxage";
/**
 * <tt>"trailers"</tt>
 */
const std::string HttpHeaders::Values::TRAILERS = "trailers";
/**
 * <tt>"Upgrade"</tt>
 */
const std::string HttpHeaders::Values::UPGRADE = "Upgrade";
/**
 * <tt>"WebSocket"</tt>
 */
const std::string HttpHeaders::Values::WEBSOCKET = "WebSocket";

const std::string HttpHeaders::Values::X_PROTOBUFFER = "application/x-protobuffer";


const std::string HttpHeaders::Values::X_WWW_FORM_URLENCODED = "application/x-www-form-urlencoded";


void HttpHeaders::setHeader(const std::string& name,
    const std::vector<int>& values) {
    for (size_t i = 0; i < values.size(); ++i) {
        setHeader(name, StringUtil::numtostr(values[i]));
    }
}

bool HttpHeaders::keepAlive(const HttpVersion& version) const {
    const std::string& connection = headerValue(Names::CONNECTION);

    if (StringUtil::iequals(Values::CLOSE, connection)) {
        return false;
    }

    if (version.keepAliveDefault()) {
        return !StringUtil::iequals(Values::CLOSE, connection);
    }
    else {
        return StringUtil::iequals(Values::KEEP_ALIVE, connection);
    }
}

void HttpHeaders::setKeepAlive(bool keepAlive, const HttpVersion& version) {
    if (version.keepAliveDefault()) {
        if (keepAlive) {
            removeHeader(Names::CONNECTION);
        }
        else {
            setHeader(Names::CONNECTION, Values::CLOSE);
        }
    }
    else {
        if (keepAlive) {
            setHeader(Names::CONNECTION, Values::KEEP_ALIVE);
        }
        else {
            removeHeader(Names::CONNECTION);
        }
    }
}

int HttpHeaders::contentLength() const {
    const std::string& contentLength = headerValue(Names::CONTENT_LENGTH);

    if (!contentLength.empty()) {
        return StringUtil::strto32(contentLength);
    }

    return -1;
}

void HttpHeaders::setContentLength(int length) {
    setHeader(Names::CONTENT_LENGTH, StringUtil::numtostr(length));
}

const std::string& HttpHeaders::host() const {
    return headerValue(Names::HOST);
}

const std::string& HttpHeaders::host(const std::string& defaultValue) const {
    return headerValue(Names::HOST, defaultValue);
}

void HttpHeaders::setHost(const std::string& value) {
    setHeader(Names::HOST, value);
}

const HttpTransferEncoding& HttpHeaders::transferEncoding() const {
    return transferEncoding_;
}

void HttpHeaders::setTransferEncoding(const HttpTransferEncoding& te) {
    transferEncoding_ = te;
    const std::string& transferEnocodingStr = HttpHeaders::Names::TRANSFER_ENCODING;
    const std::string& chunkedStr = HttpHeaders::Values::CHUNKED;

    if (te == HttpTransferEncoding::CHUNKED) {
        if (!headers_.has(transferEnocodingStr, chunkedStr)) {
            addHeader(transferEnocodingStr, chunkedStr);
        }
        removeHeader(HttpHeaders::Names::CONTENT_LENGTH);
    }
    else {
        headers_.erase(transferEnocodingStr, chunkedStr);
    }
}

void HttpHeaders::addCookie(const Cookie& cookie) {
    cookies_.push_back(cookie);
}

void HttpHeaders::addCookie(const std::string& name, const std::string& value) {
    Cookie cookie(name, value);
    cookies_.push_back(cookie);
}

const std::vector<Cookie>& HttpHeaders::cookies() const {
    if (cookies_.empty()) {
        const std::string& header = headers_.get(HttpHeaders::Names::COOKIE);
        if (!header.empty()) {
            return cookies_;
        }

        NameValueCollection::ConstIterator lower =
            headers_.lowerBound(HttpHeaders::Names::SET_COOKIE);

        if (lower != headers_.end()) {
            NameValueCollection::ConstIterator up =
                headers_.upperBound(HttpHeaders::Names::SET_COOKIE);

            for (NameValueCollection::ConstIterator itr = lower; itr != up; ++itr) {

            }
        }
    }

    return cookies_;
}

void HttpHeaders::toString(std::string* str) const {

    //     StringUtil::printf(&buf,
    //         "HttpMessage (version: %d, keepAlive: %s,  transferEncoding: %s)",
    //         getProtocolVersion().getText().c_str(),
    //         HttpHeaders::keepAlive(*this) ? "true" : "false",
    //         transferEncoding_.toString().c_str());

    ConstHeaderIterator itr = firstHeader();
    ConstHeaderIterator end = lastHeader();

    for (itr; itr != end; ++itr) {
        StringUtil::printf(str, "\r\n%s: %s", itr->first.c_str(), itr->second.c_str());
    }
}

}
}
}
}
