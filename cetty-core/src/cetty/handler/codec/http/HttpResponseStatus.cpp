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

#include <cetty/util/Integer.h>
#include <cetty/util/Exception.h>
#include <cetty/handler/codec/http/HttpResponseStatus.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

HttpResponseStatus HttpResponseStatus::CONTINUE(100, "Continue");
HttpResponseStatus HttpResponseStatus::SWITCHING_PROTOCOLS(101, "Switching Protocols");
HttpResponseStatus HttpResponseStatus::PROCESSING(102, "Processing");
HttpResponseStatus HttpResponseStatus::OK(200, "OK");
HttpResponseStatus HttpResponseStatus::CREATED(201, "Created");
HttpResponseStatus HttpResponseStatus::ACCEPTED(202, "Accepted");
HttpResponseStatus HttpResponseStatus::NON_AUTHORITATIVE_INFORMATION(203, "Non-Authoritative Information");
HttpResponseStatus HttpResponseStatus::NO_CONTENT(204, "No Content");
HttpResponseStatus HttpResponseStatus::RESET_CONTENT(205, "Reset Content");
HttpResponseStatus HttpResponseStatus::PARTIAL_CONTENT(206, "Partial Content");
HttpResponseStatus HttpResponseStatus::MULTI_STATUS(207, "Multi-Status");
HttpResponseStatus HttpResponseStatus::MULTIPLE_CHOICES(300, "Multiple Choices");
HttpResponseStatus HttpResponseStatus::MOVED_PERMANENTLY(301, "Moved Permanently");
HttpResponseStatus HttpResponseStatus::FOUND(302, "Found");
HttpResponseStatus HttpResponseStatus::SEE_OTHER(303, "See Other");
HttpResponseStatus HttpResponseStatus::NOT_MODIFIED(304, "Not Modified");
HttpResponseStatus HttpResponseStatus::USE_PROXY(305, "Use Proxy");
HttpResponseStatus HttpResponseStatus::TEMPORARY_REDIRECT(307, "Temporary Redirect");
HttpResponseStatus HttpResponseStatus::BAD_REQUEST(400, "Bad Request");
HttpResponseStatus HttpResponseStatus::UNAUTHORIZED(401, "Unauthorized");
HttpResponseStatus HttpResponseStatus::PAYMENT_REQUIRED(402, "Payment Required");
HttpResponseStatus HttpResponseStatus::FORBIDDEN(403, "Forbidden");
HttpResponseStatus HttpResponseStatus::NOT_FOUND(404, "Not Found");
HttpResponseStatus HttpResponseStatus::METHOD_NOT_ALLOWED(405, "Method Not Allowed");
HttpResponseStatus HttpResponseStatus::NOT_ACCEPTABLE(406, "Not Acceptable");
HttpResponseStatus HttpResponseStatus::PROXY_AUTHENTICATION_REQUIRED(407, "Proxy Authentication Required");
HttpResponseStatus HttpResponseStatus::REQUEST_TIMEOUT(408, "Request Timeout");
HttpResponseStatus HttpResponseStatus::CONFLICT(409, "Conflict");
HttpResponseStatus HttpResponseStatus::GONE(410, "Gone");
HttpResponseStatus HttpResponseStatus::LENGTH_REQUIRED(411, "Length Required");
HttpResponseStatus HttpResponseStatus::PRECONDITION_FAILED(412, "Precondition Failed");
HttpResponseStatus HttpResponseStatus::REQUEST_ENTITY_TOO_LARGE(413, "Request Entity Too Large");
HttpResponseStatus HttpResponseStatus::REQUEST_URI_TOO_LONG(414, "Request-URI Too Long");
HttpResponseStatus HttpResponseStatus::UNSUPPORTED_MEDIA_TYPE(415, "Unsupported Media Type");
HttpResponseStatus HttpResponseStatus::REQUESTED_RANGE_NOT_SATISFIABLE(416, "Requested Range Not Satisfiable");
HttpResponseStatus HttpResponseStatus::EXPECTATION_FAILED(417, "Expectation Failed");
HttpResponseStatus HttpResponseStatus::UNPROCESSABLE_ENTITY(422, "Unprocessable Entity");
HttpResponseStatus HttpResponseStatus::LOCKED(423, "Locked");
HttpResponseStatus HttpResponseStatus::FAILED_DEPENDENCY(424, "Failed Dependency");
HttpResponseStatus HttpResponseStatus::UNORDERED_COLLECTION(425, "Unordered Collection");
HttpResponseStatus HttpResponseStatus::UPGRADE_REQUIRED(426, "Upgrade Required");
HttpResponseStatus HttpResponseStatus::INTERNAL_SERVER_ERROR(500, "Internal Server Error");
HttpResponseStatus HttpResponseStatus::NOT_IMPLEMENTED(501, "Not Implemented");
HttpResponseStatus HttpResponseStatus::BAD_GATEWAY(502, "Bad Gateway");
HttpResponseStatus HttpResponseStatus::SERVICE_UNAVAILABLE(503, "Service Unavailable");
HttpResponseStatus HttpResponseStatus::GATEWAY_TIMEOUT(504, "Gateway Timeout");
HttpResponseStatus HttpResponseStatus::HTTP_VERSION_NOT_SUPPORTED(505, "HTTP Version Not Supported");
HttpResponseStatus HttpResponseStatus::VARIANT_ALSO_NEGOTIATES(506, "Variant Also Negotiates");
HttpResponseStatus HttpResponseStatus::INSUFFICIENT_STORAGE(507, "Insufficient Storage");
HttpResponseStatus HttpResponseStatus::NOT_EXTENDED(510, "Not Extended");

cetty::handler::codec::http::HttpResponseStatus HttpResponseStatus::valueOf(int code) {
    switch (code) {
    case 100:
        return CONTINUE;

    case 101:
        return SWITCHING_PROTOCOLS;

    case 102:
        return PROCESSING;

    case 200:
        return OK;

    case 201:
        return CREATED;

    case 202:
        return ACCEPTED;

    case 203:
        return NON_AUTHORITATIVE_INFORMATION;

    case 204:
        return NO_CONTENT;

    case 205:
        return RESET_CONTENT;

    case 206:
        return PARTIAL_CONTENT;

    case 207:
        return MULTI_STATUS;

    case 300:
        return MULTIPLE_CHOICES;

    case 301:
        return MOVED_PERMANENTLY;

    case 302:
        return FOUND;

    case 303:
        return SEE_OTHER;

    case 304:
        return NOT_MODIFIED;

    case 305:
        return USE_PROXY;

    case 307:
        return TEMPORARY_REDIRECT;

    case 400:
        return BAD_REQUEST;

    case 401:
        return UNAUTHORIZED;

    case 402:
        return PAYMENT_REQUIRED;

    case 403:
        return FORBIDDEN;

    case 404:
        return NOT_FOUND;

    case 405:
        return METHOD_NOT_ALLOWED;

    case 406:
        return NOT_ACCEPTABLE;

    case 407:
        return PROXY_AUTHENTICATION_REQUIRED;

    case 408:
        return REQUEST_TIMEOUT;

    case 409:
        return CONFLICT;

    case 410:
        return GONE;

    case 411:
        return LENGTH_REQUIRED;

    case 412:
        return PRECONDITION_FAILED;

    case 413:
        return REQUEST_ENTITY_TOO_LARGE;

    case 414:
        return REQUEST_URI_TOO_LONG;

    case 415:
        return UNSUPPORTED_MEDIA_TYPE;

    case 416:
        return REQUESTED_RANGE_NOT_SATISFIABLE;

    case 417:
        return EXPECTATION_FAILED;

    case 422:
        return UNPROCESSABLE_ENTITY;

    case 423:
        return LOCKED;

    case 424:
        return FAILED_DEPENDENCY;

    case 425:
        return UNORDERED_COLLECTION;

    case 426:
        return UPGRADE_REQUIRED;

    case 500:
        return INTERNAL_SERVER_ERROR;

    case 501:
        return NOT_IMPLEMENTED;

    case 502:
        return BAD_GATEWAY;

    case 503:
        return SERVICE_UNAVAILABLE;

    case 504:
        return GATEWAY_TIMEOUT;

    case 505:
        return HTTP_VERSION_NOT_SUPPORTED;

    case 506:
        return VARIANT_ALSO_NEGOTIATES;

    case 507:
        return INSUFFICIENT_STORAGE;

    case 510:
        return NOT_EXTENDED;
    }

    std::string reasonPhrase;

    if (code < 100) {
        reasonPhrase = "Unknown Status ";
    }
    else if (code < 200) {
        reasonPhrase = "Informational ";
    }
    else if (code < 300) {
        reasonPhrase = "Successful ";
    }
    else if (code < 400) {
        reasonPhrase = "Redirection ";
    }
    else if (code < 500) {
        reasonPhrase = "Client Error ";
    }
    else if (code < 600) {
        reasonPhrase = "Server Error ";
    }
    else {
        reasonPhrase = "Unknown Status ";
    }

    Integer::appendString(code, &reasonPhrase);
    return HttpResponseStatus(code, reasonPhrase);
}

HttpResponseStatus::HttpResponseStatus(int code, const std::string& reasonPhrase)
    : code(code), reasonPhrase(reasonPhrase) {
    if (code < 0) {
        throw InvalidArgumentException("code: < 0 (expected: 0+)");
    }

    for (size_t i = 0; i < reasonPhrase.length(); i ++) {
        char c = reasonPhrase[i];

        // Check prohibited characters.
        if (c == '\n' || c == '\r') {
            throw InvalidArgumentException(
                "reasonPhrase contains one of the following prohibited characters: \\r\\n.");
        }
    }
}

std::string HttpResponseStatus::toString() const {
    std::string buf;
    buf.reserve(reasonPhrase.size() + 16);

    Integer::appendString(code, &buf);
    buf.append(" ");
    buf.append(reasonPhrase);
    return buf;
}

}
}
}
}