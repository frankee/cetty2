#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIEENCODER_H)
#define CETTY_HANDLER_CODEC_HTTP_COOKIEENCODER_H

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
namespace handler {
namespace codec {
namespace http {

/**
 * Encodes {@link Cookie}s into an HTTP header value.  This encoder can encode
 * the HTTP cookie version 0, 1, and 2.
 * <p>
 * This encoder is stateful.  It maintains an internal data structure that
 * holds the {@link Cookie}s added by the {@link #addCookie(String, String)}
 * method.  Once {@link #encode()} is called, all added {@link Cookie}s are
 * encoded into an HTTP header value and all {@link Cookie}s in the internal
 * data structure are removed so that the encoder can start over.
 * <pre>
 * // Client-side example
 * {@link HttpRequest} req = ...;
 * {@link CookieEncoder} encoder = new {@link CookieEncoder}(false);
 * encoder.addCookie("JSESSIONID", "1234");
 * res.setHeader("Cookie", encoder.encode());
 *
 * // Server-side example
 * {@link HttpResponse} res = ...;
 * {@link CookieEncoder} encoder = new {@link CookieEncoder}(true);
 * encoder.addCookie("JSESSIONID", "1234");
 * res.setHeader("Set-Cookie", encoder.encode());
 * </pre>
 *
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 * @see CookieDecoder
 *
 * @apiviz.stereotype utility
 * @apiviz.has        org.jboss.netty.handler.codec.http.Cookie oneway - - encodes
 */

class CookieEncoder {
public:
    /**
     * Creates a new encoder.
     *
     * @param server <tt>true</tt> if and only if this encoder is supposed to
     *               encode server-side cookies.  <tt>false</tt> if and only if
     *               this encoder is supposed to encode client-side cookies.
     */
    CookieEncoder(bool server) : server(server) {
    }

    /**
     * Adds a new {@link Cookie} created with the specified name and value to
     * this encoder.
     */
    void addCookie(const std::string& name, const std::string& value) {
        cookies.add(new DefaultCookie(name, value));
    }

    /**
     * Adds the specified {@link Cookie} to this encoder.
     */
    void addCookie(Cookie cookie) {
        cookies.add(cookie);
    }

    /**
     * Encodes the {@link Cookie}s which were added by {@link #addCookie(Cookie)}
     * so far into an HTTP header value.  If no {@link Cookie}s were added,
     * an empty string is returned.
     */
    void encode(std::string* str) {
        if (str) {
            if (server) {
                encodeServerSide(str);
            }
            else {
                encodeClientSide(str);
            }
        }

        cookies.clear();
    }

private:
    void encodeServerSide(std::string* str) {
for (Cookie cookie: cookies) {
            add(sb, cookie.getName(), cookie.getValue());

            if (cookie.getMaxAge() >= 0) {
                if (cookie.getVersion() == 0) {
                    addUnquoted(sb, CookieHeaderNames::COOKIE_EXPIRES,
                                new CookieDateFormat().format(
                                    new Date(System.currentTimeMillis() +
                                             cookie.getMaxAge() * 1000L)));
                }
                else {
                    add(sb, CookieHeaderNames::COOKIE_MAX_AGE, cookie.getMaxAge());
                }
            }

            if (!cookie.getPath().empty()) {
                if (cookie.getVersion() > 0) {
                    add(sb, CookieHeaderNames::COOKIE_PATH, cookie.getPath());
                }
                else {
                    addUnquoted(sb, CookieHeaderNames::COOKIE_PATH, cookie.getPath());
                }
            }

            if (!cookie.getDomain().empty()) {
                if (cookie.getVersion() > 0) {
                    add(sb, CookieHeaderNames::COOKIE_DOMAIN, cookie.getDomain());
                }
                else {
                    addUnquoted(sb, CookieHeaderNames::COOKIE_DOMAIN, cookie.getDomain());
                }
            }

            if (cookie.isSecure()) {
                sb.append(CookieHeaderNames::COOKIE_SECURE);
                sb.append((char) HttpCodecUtil::SEMICOLON);
            }

            if (cookie.isHttpOnly()) {
                sb.append(CookieHeaderNames::COOKIE_HTTP_ONLY);
                sb.append((char) HttpCodecUtil::SEMICOLON);
            }

            if (cookie.getVersion() >= 1) {
                if (cookie.getComment() != null) {
                    add(sb, CookieHeaderNames::COOKIE_COMMENT, cookie.getComment());
                }

                add(sb, CookieHeaderNames::COOKIE_VERSION, 1);

                if (cookie.getCommentUrl() != null) {
                    addQuoted(sb, CookieHeaderNames::COOKIE_COMMENT_URL, cookie.getCommentUrl());
                }

                if (!cookie.getPorts().isEmpty()) {
                    sb.append(CookieHeaderNames::COOKIE_PORT);
                    sb.append((char) HttpCodecUtil.EQUALS);
                    sb.append((char) HttpCodecUtil.DOUBLE_QUOTE);

for (int port: cookie.getPorts()) {
                        sb.append(port);
                        sb.append((char) HttpCodecUtil.COMMA);
                    }

                    sb.setCharAt(sb.length() - 1, (char) HttpCodecUtil.DOUBLE_QUOTE);
                    sb.append((char) HttpCodecUtil.SEMICOLON);
                }

                if (cookie.isDiscard()) {
                    sb.append(CookieHeaderNames::COOKIE_DISCARD);
                    sb.append((char) HttpCodecUtil.SEMICOLON);
                }
            }
        }

        sb.setLength(sb.length() - 1);
    }

    void encodeClientSide(std::string* str) {
for (Cookie cookie: cookies) {
            if (cookie.getVersion() >= 1) {
                add(sb, '$' + CookieHeaderNames::COOKIE_VERSION, 1);
            }

            add(sb, cookie.getName(), cookie.getValue());

            if (!cookie.getPath().empty()) {
                add(sb, '$' + CookieHeaderNames::COOKIE_PATH, cookie.getPath());
            }

            if (!cookie.getDomain().empty()) {
                add(sb, '$' + CookieHeaderNames::COOKIE_DOMAIN, cookie.getDomain());
            }

            if (cookie.getVersion() >= 1) {
                if (!cookie.getPorts().empty()) {
                    sb.append('$');
                    sb.append(CookieHeaderNames::COOKIE_PORT);
                    sb.append((char) HttpCodecUtil.EQUALS);
                    sb.append((char) HttpCodecUtil.DOUBLE_QUOTE);

for (int port: cookie.getPorts()) {
                        sb.append(port);
                        sb.append((char) HttpCodecUtil.COMMA);
                    }

                    sb.setCharAt(sb.length() - 1, (char) HttpCodecUtil.DOUBLE_QUOTE);
                    sb.append((char) HttpCodecUtil.SEMICOLON);
                }
            }
        }

        sb.setLength(sb.length() - 1);
    }

    static void add(const std::string& name, const std::string& val, std::string* buffer) {
        if (val.empty()) {
            addQuoted(name, "", buffer);
            return;
        }

        for (int i = 0; i < val.length(); i ++) {
            char c = val.at(i);

            switch (c) {
            case '\t': case ' ': case '"': case '(':  case ')': case ',':
            case '/':  case ':': case ';': case '<':  case '=': case '>':
            case '?':  case '@': case '[': case '\\': case ']':
            case '{':  case '}':
                addQuoted(name, val, buffer);
                return;
            }
        }

        addUnquoted(name, val, buffer);
    }

    static void addUnquoted(const std::string& name, const std::string& val, std::string* buffer) {
        if (buffer) {
            buffer->append(name);
            buffer->append(HttpCodecUtil::EQUALS, 1);
            buffer->append(val);
            buffer->append(HttpCodecUtil.SEMICOLON, 1);
        }
    }

    static void addQuoted(const std::string& name, const std::string& val, std::string* buffer) {
        if (buffer) {
            std::string value(val);

            buffer->append(name);
            buffer->append(HttpCodecUtil::EQUALS);
            buffer->append(HttpCodecUtil::DOUBLE_QUOTE);
            //buffer->append(value.replace("\\", "\\\\").replace("\"", "\\\""));
            buffer->append(HttpCodecUtil::DOUBLE_QUOTE);
            buffer->append(HttpCodecUtil::SEMICOLON);
        }
    }

    static void add(const std::string& name, int val, std::string* buffer) {
        if (buffer) {
            buffer->append(name);
            buffer->append(HttpCodecUtil::EQUALS, 1);
            buffer->append(Integer::toString(val));
            buffer->append(HttpCodecUtil::SEMICOLON, 1);
        }
    }

private:
    private final Set<Cookie> cookies = new TreeSet<Cookie>();
    bool server;
};


}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIEENCODER_H)

// Local Variables:
// mode: c++
// End:

