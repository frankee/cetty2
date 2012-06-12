/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/handler/codec/http/Cookie.h>

#include <boost/algorithm/string/trim.hpp>

#include <cetty/util/Exception.h>
#include <cetty/util/Integer.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/NameValueCollection.h>
#include <cetty/handler/codec/http/CookieHeaderNames.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

Cookie::Cookie(const std::string& name, const std::string& value)
    : secure(false), httpOnly(false), discard(false), maxAge(-1), version(0) {
    std::string nameStr = boost::trim_copy(name);

    if (nameStr.empty()) {
        throw InvalidArgumentException("empty name");
    }

    int nameSize = name.size();

    for (int i = 0; i < nameSize; i ++) {
        char c = name.at(i);

        if (c > 127) {
            throw InvalidArgumentException("name contains non-ascii character", name);
        }

        // Check prohibited characters.
        switch (c) {
        case '\t': case '\n': case 0x0b: case '\f': case '\r':
        case ' ':  case ',':  case ';':  case '=':
            throw InvalidArgumentException(
                "name contains one of the following prohibited characters: =,; \\t\\r\\n\\v\\f",
                name);
        }
    }

    if (CookieHeaderNames::isReserved(name)) {
        throw InvalidArgumentException("reserved name", name);
    }

    this->name = name;
    setValue(value);
}

Cookie::Cookie(const std::string& name) {

}

Cookie::Cookie(const NameValueCollection& nvc) {

}

void Cookie::validateValue(const std::string& name, const std::string& value, std::string* output) const {
    if (output) {
        *output = boost::trim_copy(value);

        for (size_t i = 0; i < output->size(); ++i) {
            char c = output->at(i);

            switch (c) {
            case '\r': case '\n': case '\f': case 0x0b: case ';':
                throw InvalidArgumentException(
                    name + " contains one of the following prohibited characters: " +
                    ";\\r\\n\\f\\v (" + value + ')');
            }
        }
    }
}

void Cookie::setValue(const std::string& value) {
    this->value = value;
}

void Cookie::setDomain(const std::string& domain) {
    validateValue("domain", domain, &this->domain);
}

void Cookie::setPath(const std::string& path) {
    validateValue("path", path, &this->path);
}

void Cookie::setComment(const std::string& comment) {
    validateValue("comment", comment, &this->comment);
}

void Cookie::setCommentUrl(const std::string& commentUrl) {
    validateValue("commentUrl", commentUrl, &this->commentUrl);
}

void Cookie::setDiscard(bool discard) {
    this->discard = discard;
}

void Cookie::setPorts(const std::vector<int>& ports) {
    /*
    Set<Integer> newPorts = new TreeSet<Integer>();
    for (int p: ports) {
        if (p <= 0 || p > 65535) {
            throw new InvalidArgumentException("port out of range: " + p);
        }
        newPorts.add(Integer.valueOf(p));
    }
    if (newPorts.isEmpty()) {
        unmodifiablePorts = this.ports = Collections.emptySet();
    }
    else {
        this.ports = newPorts;
        unmodifiablePorts = null;
    }*/
}

void Cookie::setMaxAge(int maxAge) {
    if (maxAge < -1) {
        throw InvalidArgumentException(
            "maxAge must be either -1, 0, or a positive integer: " +
            maxAge);
    }

    this->maxAge = maxAge;
}

void Cookie::setVersion(int version) {
    this->version = version;
}

void Cookie::setSecure(bool secure) {
    this->secure = secure;
}

void Cookie::setHttpOnly(bool httpOnly) {
    this->httpOnly = httpOnly;
}

#if 0
bool Cookie::equals(const Cookie& that) const {
    /*
    if (!getName().equalsIgnoreCase(that.getName())) {
        return false;
    }

    if (getPath() == null && that.getPath() != null) {
        return false;
    } else if (that.getPath() == null) {
        return false;
    }
    if (!getPath().equals(that.getPath())) {
        return false;
    }

    if (getDomain() == null && that.getDomain() != null) {
        return false;
    } else if (that.getDomain() == null) {
        return false;
    }
    if (!getDomain().equalsIgnoreCase(that.getDomain())) {
        return false;
    }
    */
    return true;
}
#endif

int Cookie::compareTo(const Cookie& c) const {
    int v;
    /*
    v = getName().compareToIgnoreCase(c.getName());
    if (v != 0) {
        return v;
    }

    if (getPath() == null && c.getPath() != null) {
        return -1;
    } else if (c.getPath() == null) {
        return 1;
    }
    v = getPath().compareTo(c.getPath());
    if (v != 0) {
        return v;
    }

    if (getDomain() == null && c.getDomain() != null) {
        return -1;
    } else if (c.getDomain() == null) {
        return 1;
    }
    v = getDomain().compareToIgnoreCase(c.getDomain());
    */
    return v;
}

std::string Cookie::toString() const {
    std::string buf(getName());

    buf += "=";
    buf += getValue();

    if (!domain.empty()) {
        buf.append(", domain=");
        buf.append(domain);
    }

    if (!path.empty()) {
        buf.append(", path=");
        buf.append(path);
    }

    if (!comment.empty()) {
        buf.append(", comment=");
        buf.append(getComment());
    }

    if (maxAge >= 0) {
        StringUtil::strprintf(&buf, ", maxAge=%ds", maxAge);
    }

    if (isSecure()) {
        buf.append(", secure");
    }

    if (isHttpOnly()) {
        buf.append(", HTTPOnly");
    }

    return buf;
}

}
}
}
}