#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIEHEADERNAMES_H)
#define CETTY_HANDLER_CODEC_HTTP_COOKIEHEADERNAMES_H

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
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */
class CookieHeaderNames {
public:
    static const std::string COOKIE_PATH;
    static const std::string COOKIE_EXPIRES;
    static const std::string COOKIE_MAX_AGE;
    static const std::string COOKIE_DOMAIN;
    static const std::string COOKIE_SECURE;
    static const std::string COOKIE_HTTP_ONLY;
    static const std::string COOKIE_COMMENT;
    static const std::string COOKIE_COMMENT_URL;
    static const std::string COOKIE_DISCARD;
    static const std::string COOKIE_PORT;
    static const std::string COOKIE_VERSION;

    static bool isReserved(const std::string& name);

private:
    CookieHeaderNames() {}
    ~CookieHeaderNames() {}
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIEHEADERNAMES_H)

// Local Variables:
// mode: c++
// End:

