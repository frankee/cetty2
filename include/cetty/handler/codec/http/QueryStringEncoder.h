#if !defined(CETTY_HANDLER_CODEC_HTTP_QUERYSTRINGENCODER_H)
#define CETTY_HANDLER_CODEC_HTTP_QUERYSTRINGENCODER_H

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
#include <utility>
#include <cetty/util/URI.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

/**
 * Creates an URL-encoded URI from a path string and key-value parameter pairs.
 * This encoder is for one time use only.  Create a new instance for each URI.
 *
 * <pre>
 * {@link QueryStringEncoder} encoder = new {@link QueryStringDecoder}("/hello");
 * encoder.addParam("recipient", "world");
 * assert encoder.toString().equals("/hello?recipient=world");
 * </pre>
 *
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @see QueryStringDecoder
 *
 * @apiviz.stereotype utility
 * @apiviz.has        org.jboss.netty.handler.codec.http.HttpRequest oneway - - encodes
 */
class QueryStringEncoder {
public:
    /**
     * Creates a new encoder that encodes a URI that starts with the specified
     * path string.  The encoder will encode the URI in UTF-8.
     */
    QueryStringEncoder(const std::string& uri);

    /**
     * Adds a parameter with the specified name and value to this encoder.
     */
    void addParam(const std::string& name, const std::string& value) {
        params.push_back(std::make_pair(name, value));
    }

    /**
     * Returns the URL-encoded URI object which was created from the path string
     * specified in the constructor and the parameters added by
     * {@link #addParam(std::string, std::string)} method.
     */
    URI toUri() const {
        return URI(toString());
    }

    /**
     * Returns the URL-encoded URI which was created from the path string
     * specified in the constructor and the parameters added by
     * {@link #addParam(std::string, std::string)} method.
     */
    std::string toString() const {
        if (params.empty()) {
            return uri;
        }
        else {
            std::string tmpStr;
            std::string str(uri);
            str.append("?");

            for (size_t i = 0; i < params.size(); ++i) {
                const std::pair<std::string, std::string>& param = params[i];
                encodeComponent(param.first, tmpStr, str);
                str.append("=");
                encodeComponent(param.second, tmpStr, str);

                if (i != params.size() - 1) {
                    str.append("&");
                }
            }

            return str;
        }
    }

private:
    void encodeComponent(const std::string& s,
                         std::string& tmp,
                         std::string& output) const {
        static std::string RESERVED;

        tmp.clear();
        URI::encode(s, RESERVED, tmp);
        output.append(tmp);
    }

private:
    std::string uri;
    std::vector<std::pair<std::string, std::string> > params;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_QUERYSTRINGENCODER_H)

// Local Variables:
// mode: c++
// End:

