#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSESTATUS_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSESTATUS_H

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
#include <stdexcept>

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
 * The response code and its description of HTTP or its derived protocols, such as
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

class HttpResponseStatus {
public:
    /**
     * 100 Continue
     */
    static HttpResponseStatus CONTINUE;

    /**
     * 101 Switching Protocols
     */
    static HttpResponseStatus SWITCHING_PROTOCOLS;

    /**
     * 102 Processing (WebDAV, RFC2518)
     */
    static HttpResponseStatus PROCESSING;

    /**
     * 200 OK
     */
    static HttpResponseStatus OK;

    /**
     * 201 Created
     */
    static HttpResponseStatus CREATED;

    /**
     * 202 Accepted
     */
    static HttpResponseStatus ACCEPTED;

    /**
     * 203 Non-Authoritative Information (since HTTP/1.1)
     */
    static HttpResponseStatus NON_AUTHORITATIVE_INFORMATION;

    /**
     * 204 No Content
     */
    static HttpResponseStatus NO_CONTENT;

    /**
     * 205 Reset Content
     */
    static HttpResponseStatus RESET_CONTENT;

    /**
     * 206 Partial Content
     */
    static HttpResponseStatus PARTIAL_CONTENT;

    /**
     * 207 Multi-Status (WebDAV, RFC2518)
     */
    static HttpResponseStatus MULTI_STATUS;

    /**
     * 300 Multiple Choices
     */
    static HttpResponseStatus MULTIPLE_CHOICES;

    /**
     * 301 Moved Permanently
     */
    static HttpResponseStatus MOVED_PERMANENTLY;

    /**
     * 302 Found
     */
    static HttpResponseStatus FOUND;

    /**
     * 303 See Other (since HTTP/1.1)
     */
    static HttpResponseStatus SEE_OTHER;

    /**
     * 304 Not Modified
     */
    static HttpResponseStatus NOT_MODIFIED;

    /**
     * 305 Use Proxy (since HTTP/1.1)
     */
    static HttpResponseStatus USE_PROXY;

    /**
     * 307 Temporary Redirect (since HTTP/1.1)
     */
    static HttpResponseStatus TEMPORARY_REDIRECT;

    /**
     * 400 Bad Request
     */
    static HttpResponseStatus BAD_REQUEST;

    /**
     * 401 Unauthorized
     */
    static HttpResponseStatus UNAUTHORIZED;

    /**
     * 402 Payment Required
     */
    static HttpResponseStatus PAYMENT_REQUIRED;

    /**
     * 403 Forbidden
     */
    static HttpResponseStatus FORBIDDEN;

    /**
     * 404 Not Found
     */
    static HttpResponseStatus NOT_FOUND;

    /**
     * 405 Method Not Allowed
     */
    static HttpResponseStatus METHOD_NOT_ALLOWED;

    /**
     * 406 Not Acceptable
     */
    static HttpResponseStatus NOT_ACCEPTABLE;

    /**
     * 407 Proxy Authentication Required
     */
    static HttpResponseStatus PROXY_AUTHENTICATION_REQUIRED;

    /**
     * 408 Request Timeout
     */
    static HttpResponseStatus REQUEST_TIMEOUT;

    /**
     * 409 Conflict
     */
    static HttpResponseStatus CONFLICT;

    /**
     * 410 Gone
     */
    static HttpResponseStatus GONE;

    /**
     * 411 Length Required
     */
    static HttpResponseStatus LENGTH_REQUIRED;

    /**
     * 412 Precondition Failed
     */
    static HttpResponseStatus PRECONDITION_FAILED;

    /**
     * 413 Request Entity Too Large
     */
    static HttpResponseStatus REQUEST_ENTITY_TOO_LARGE;

    /**
     * 414 Request-URI Too Long
     */
    static HttpResponseStatus REQUEST_URI_TOO_LONG;

    /**
     * 415 Unsupported Media Type
     */
    static HttpResponseStatus UNSUPPORTED_MEDIA_TYPE;

    /**
     * 416 Requested Range Not Satisfiable
     */
    static HttpResponseStatus REQUESTED_RANGE_NOT_SATISFIABLE;

    /**
     * 417 Expectation Failed
     */
    static HttpResponseStatus EXPECTATION_FAILED;

    /**
     * 422 Unprocessable Entity (WebDAV, RFC4918)
     */
    static HttpResponseStatus UNPROCESSABLE_ENTITY;

    /**
     * 423 Locked (WebDAV, RFC4918)
     */
    static HttpResponseStatus LOCKED;

    /**
     * 424 Failed Dependency (WebDAV, RFC4918)
     */
    static HttpResponseStatus FAILED_DEPENDENCY;

    /**
     * 425 Unordered Collection (WebDAV, RFC3648)
     */
    static HttpResponseStatus UNORDERED_COLLECTION;

    /**
     * 426 Upgrade Required (RFC2817)
     */
    static HttpResponseStatus UPGRADE_REQUIRED;

    /**
     * 500 Internal Server Error
     */
    static HttpResponseStatus INTERNAL_SERVER_ERROR;

    /**
     * 501 Not Implemented
     */
    static HttpResponseStatus NOT_IMPLEMENTED;

    /**
     * 502 Bad Gateway
     */
    static HttpResponseStatus BAD_GATEWAY;

    /**
     * 503 Service Unavailable
     */
    static HttpResponseStatus SERVICE_UNAVAILABLE;

    /**
     * 504 Gateway Timeout
     */
    static HttpResponseStatus GATEWAY_TIMEOUT;

    /**
     * 505 HTTP Version Not Supported
     */
    static HttpResponseStatus HTTP_VERSION_NOT_SUPPORTED;

    /**
     * 506 Variant Also Negotiates (RFC2295)
     */
    static HttpResponseStatus VARIANT_ALSO_NEGOTIATES;

    /**
     * 507 Insufficient Storage (WebDAV, RFC4918)
     */
    static HttpResponseStatus INSUFFICIENT_STORAGE;

    /**
     * 510 Not Extended (RFC2774)
     */
    static HttpResponseStatus NOT_EXTENDED;

public:
    /**
     * Returns the {@link HttpResponseStatus} represented by the specified code.
     * If the specified code is a standard HTTP status code, a cached instance
     * will be returned.  Otherwise, a new instance will be returned.
     */
    static HttpResponseStatus valueOf(int code);

public:
    /**
     * Creates a new instance with the specified <tt>code</tt> and its
     * <tt>reasonPhrase</tt>.
     */
    HttpResponseStatus(int code, const StringPiece& reasonPhrase);

    /**
     * Returns the code of this status.
     */
    int code() const {
        return code_;
    }

    /**
     * Returns the reason phrase of this status.
     */
    const std::string& reasonPhrase() const {
        return reasonPhrase_;
    }

    bool equals(const HttpResponseStatus& status) {
        return code_ == status.code_;
    }

    int compareTo(const HttpResponseStatus& status) {
        return code_ - status.code_;
    }

    std::string toString() const;

private:
    int code_;
    std::string reasonPhrase_;
};


}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSESTATUS_H)

// Local Variables:
// mode: c++
// End:
