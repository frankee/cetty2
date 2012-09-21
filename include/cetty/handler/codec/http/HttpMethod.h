#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPMETHOD_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPMETHOD_H

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
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 *
 */

#include <string>

#if defined(BOOST_MSVC)
#undef TRACE
#endif

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
 * The request method of HTTP or its derived protocols, such as
 * <a href="http://en.wikipedia.org/wiki/Real_Time_Streaming_Protocol">RTSP</a> and
 * <a href="http://en.wikipedia.org/wiki/Internet_Content_Adaptation_Protocol">ICAP</a>.
 *
 * @remark the DELETE and the TRACE will duplicated with the macro under windows system,
 *         so all of the HttpMethod will add a <em>HM_</em> prefix.
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.exclude
 */

class HttpMethod {
public:
    /**
     * The {@link HM_OPTIONS OPTIONS} method represents a request for information about the communication options available on the request/response
     * chain identified by the Request-URI. This method allows the client to determine the options and/or requirements
     * associated with a resource, or the capabilities of a server, without implying a resource action or initiating a
     * resource retrieval.
     */
    static const HttpMethod OPTIONS;

    /**
     * The {@link HM_GET GET} method means retrieve whatever information (in the form of an entity) is identified by the Request-URI.
     * If the Request-URI refers to a data-producing process, it is the produced data which shall be returned as the entity
     * in the response and not the source text of the process, unless that text happens to be the output of the process.
     *
     */
    static const HttpMethod GET;

    /**
     * The {@link HM_HEAD HEAD} method is identical to GET except that the server MUST NOT return a message-body in the response.
     */
    static const HttpMethod HEAD;

    /**
     * The {@link HM_POST POST} method is used to request that the origin server accept the entity enclosed in the request as a new
     * subordinate of the resource identified by the Request-URI in the Request-Line.
     */
    static const HttpMethod POST;

    /**
     * The {@link HM_PUT PUT} method requests that the enclosed entity be stored under the supplied Request-URI.
     */
    static const HttpMethod PUT;

    /**
     * The {@link HM_PATCH PATCH} method requests that a set of changes described in the
     * request entity be applied to the resource identified by the Request-URI.
     */
    static const HttpMethod PATCH;

    /**
     * The {@link HM_DELETE DELETE} method requests that the origin server delete the resource identified by the Request-URI.
     */
    static const HttpMethod DELETE;

    /**
     * The {@link HM_TRACE TRACE} method is used to invoke a remote, application-layer loop- back of the request message.
     */
    static const HttpMethod TRACE;

    /**
     * This specification reserves the method name {@link HM_CONNECT CONNECT} for use with a proxy that can dynamically switch to being a tunnel
     */
    static const HttpMethod CONNECT;

    /**
     * Returns the {@link HttpMethod} represented by the specified name.
     * If the specified name is a standard HTTP method name, a cached instance
     * will be returned.  Otherwise, a new instance will be returned.
     */
    static HttpMethod valueOf(const std::string& name);

    static HttpMethod valueOf(const StringPiece& name);

    /**
     * Creates a new HTTP method with the specified name.  You will not need to
     * create a new method unless you are implementing a protocol derived from
     * HTTP, such as
     * <a href="http://en.wikipedia.org/wiki/Real_Time_Streaming_Protocol">RTSP</a> and
     * <a href="http://en.wikipedia.org/wiki/Internet_Content_Adaptation_Protocol">ICAP</a>
     */
    HttpMethod(const std::string& name);

    bool operator==(const HttpMethod& method) const {
        return compareTo(method) == 0;
    }

    bool operator!=(const HttpMethod& method) const {
        return compareTo(method) != 0;
    }

    /**
     * Returns the name of this method.
     */
    const std::string& getName() const {
        return this->name;
    }

    std::string toString() const {
        return getName();
    }

    int compareTo(const HttpMethod& method) const {
        return getName().compare(method.getName());
    }

private:
    std::string name;
};


}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPMETHOD_H)

// Local Variables:
// mode: c++
// End:
