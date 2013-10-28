#if !defined(CETTY_SERVICE_HTTP_HTTPCLIENT_H)
#define CETTY_SERVICE_HTTP_HTTPCLIENT_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
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

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/intrusive_ptr.hpp>
#include <cetty/service/ClientService.h>
#include <cetty/service/builder/ClientBuilder.h>
#include <cetty/handler/codec/http/HttpPackage.h>
#include <cetty/handler/codec/http/HttpMethod.h>
#include <cetty/handler/codec/http/HttpRequestEncoder.h>
#include <cetty/handler/codec/http/HttpResponseDecoder.h>

namespace cetty {
namespace service {
namespace http {

using namespace cetty::channel;
using namespace cetty::service::builder;
using namespace cetty::handler::codec::http;

class HttpClient : private boost::noncopyable {
public:
    typedef ServiceFuture<HttpPackage> HttpServiceFuture;
    typedef HttpServiceFuture::Ptr HttpServiceFuturePtr;

    typedef boost::function<void (HttpResponsePtr const&)> ResponseCallback;

public:
    HttpClient() {
        future_ = new HttpServiceFuture();
        builder_.setClientInitializer(boost::bind(&HttpClient::initChannelPipeline, this, _1));
    }

    void setHeader(const std::string& name, const std::string& value) {

    }

    void get(const std::string& url, const ResponseCallback& callback) {
        HttpRequestPtr req = new HttpRequest;
        //req->headers().
        req->setMethod(HttpMethod::GET);
        //req->setKeepAlive(false);
        req->setUri(url);
        req->setVersion(HttpVersion::HTTP_1_1);

        Connection conn(req->uri().getHost(), req->uri().getPort(), 1);
        ChannelPtr ch = builder_.build(conn);
        HttpPackage package(req);
        future_->addListener(boost::bind(&HttpClient::onResponse,
                                         this,
                                         _1,
                                         _2,
                                         callback));
        ClientService<HttpPackage>::callMethod(ch, package, future_);
    }

    void post(const std::string& url, const std::string& data) {

    }

private:
    void onResponse(const HttpServiceFuture& future,
                    const HttpPackage& response,
                    const ResponseCallback& callback) {
        if (response.isHttpResponse()) {
            if (callback) {
                callback(response.httpResponse());
            }
        }
    }

    bool initChannelPipeline(ChannelPipeline& pipeline) {
        pipeline.addLast<HttpRequestEncoder::Handler>(
            "encoder",
            HttpRequestEncoder::HandlerPtr(new HttpRequestEncoder));

        pipeline.addLast<HttpResponseDecoder::Handler>(
            "decoder",
            HttpResponseDecoder::HandlerPtr(new HttpResponseDecoder));

        return true;
    }

private:
    std::map<std::string, std::string> headers_;
    ClientBuilder<HttpPackage> builder_;
    HttpServiceFuturePtr future_;
};


}
}
}

#endif //#if !defined(CETTY_SERVICE_HTTP_HTTPCLIENT_H)

// Local Variables:
// mode: c++
// End:
