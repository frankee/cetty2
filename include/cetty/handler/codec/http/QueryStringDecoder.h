#if !defined(CETTY_HANDLER_CODEC_HTTP_QUERYSTRINGDECODER_H)
#define CETTY_HANDLER_CODEC_HTTP_QUERYSTRINGDECODER_H

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

#include <map>
#include <vector>
#include <string>

#include <cetty/util/URI.h>
#include <cetty/util/NameValueCollection.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

/**
 * Splits an HTTP query string into a path string and key-value parameter pairs.
 * This decoder is for one time use only.  Create a new instance for each URI:
 * <pre>
 * {@link QueryStringDecoder} decoder = new {@link QueryStringDecoder}("/hello?recipient=world");
 * assert decoder.getPath().equals("/hello");
 * assert decoder.getParameters().get("recipient").equals("world");
 * </pre>
 *
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="http://tsunanet.net/">Benoit Sigoure</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @see QueryStringEncoder
 *
 * @apiviz.stereotype utility
 * @apiviz.has        org.jboss.netty.handler.codec.http.HttpRequest oneway - - decodes
 */

class QueryStringDecoder {
public:
    /**
     * Creates a new decoder that decodes the specified URI. The decoder will
     * assume that the query string is encoded in UTF-8.
     */
    QueryStringDecoder(const URI& uri);

    /**
     * Returns the decoded path string of the URI.
     */
    const std::string& getPath() const {
        return uri.getPath();
    }

    /**
     * Returns the decoded key-value parameter pairs of the URI.
     */
    void getParameters(NameValueCollection* params) const;

private:
    const URI& uri;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_QUERYSTRINGDECODER_H)

// Local Variables:
// mode: c++
// End:

