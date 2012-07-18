/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <map>
#include <boost/algorithm/string/case_conv.hpp>
#include <cetty/handler/codec/http/CookieHeaderNames.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

const std::string CookieHeaderNames::COOKIE_PATH = "Path";
const std::string CookieHeaderNames::COOKIE_EXPIRES = "Expires";
const std::string CookieHeaderNames::COOKIE_MAX_AGE = "Max-Age";
const std::string CookieHeaderNames::COOKIE_DOMAIN = "Domain";
const std::string CookieHeaderNames::COOKIE_SECURE = "Secure";
const std::string CookieHeaderNames::COOKIE_HTTP_ONLY = "HTTPOnly";
const std::string CookieHeaderNames::COOKIE_COMMENT = "Comment";
const std::string CookieHeaderNames::COOKIE_COMMENT_URL = "CommentURL";
const std::string CookieHeaderNames::COOKIE_DISCARD = "Discard";
const std::string CookieHeaderNames::COOKIE_PORT = "Port";
const std::string CookieHeaderNames::COOKIE_VERSION = "Version";

bool CookieHeaderNames::isReserved(const std::string& name) {
    static bool init = false;
    static std::map<std::string, int> reservedNames;

    if (!init) {
        reservedNames[boost::to_lower_copy(COOKIE_PATH)] = 1;
        reservedNames[boost::to_lower_copy(COOKIE_EXPIRES)] = 1;
        reservedNames[boost::to_lower_copy(COOKIE_MAX_AGE)] = 1;
        reservedNames[boost::to_lower_copy(COOKIE_DOMAIN)] = 1;
        reservedNames[boost::to_lower_copy(COOKIE_SECURE)] = 1;
        reservedNames[boost::to_lower_copy(COOKIE_HTTP_ONLY)] = 1;
        reservedNames[boost::to_lower_copy(COOKIE_COMMENT)] = 1;
        reservedNames[boost::to_lower_copy(COOKIE_COMMENT_URL)] = 1;
        reservedNames[boost::to_lower_copy(COOKIE_DISCARD)] = 1;
        reservedNames[boost::to_lower_copy(COOKIE_PORT)] = 1;
        reservedNames[boost::to_lower_copy(COOKIE_VERSION)] = 1;

        init = true;
    }

    return reservedNames.find(boost::to_lower_copy(name)) != reservedNames.end();
}

}
}
}
}
