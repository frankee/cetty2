#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFHTTPMESSAGEHANDLER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFHTTPMESSAGEHANDLER_H

/**
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

#include <boost/cstdint.hpp>
#include <boost/thread/mutex.hpp>
#include <google/protobuf/service.h>
#include <cetty/channel/SimpleChannelUpstreamHandler.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>

#include <cetty/handler/rpc/protobuf/ProtobufServicePtr.h>

namespace google {
namespace protobuf {
class MethodDescriptor;
class RpcController;
class Message;
class Closure;
}
}

namespace cetty {
namespace channel {
class Channel;
}
}

namespace cetty {
namespace handler {
namespace rpc {
namespace protobuf {

using namespace cetty::channel;
using namespace cetty::handler::codec::http;
using namespace cetty::handler::rpc::protobuf;

class ProtobufServiceRegister;
class ProtobufHttpMessageHandler;
typedef boost::intrusive_ptr<ProtobufHttpMessageHandler> ProtobufHttpMessageHandlerPtr;

class ProtobufHttpMessageHandler : public cetty::channel::SimpleChannelUpstreamHandler,
    public google::protobuf::RpcChannel {
public:
    typedef google::protobuf::Message          MessageType;
    typedef google::protobuf::MethodDescriptor MethodType;
    typedef MessageType*       MessagePtr;
    typedef MessageType const* ConstMessagePtr;
    typedef MethodType*        MethodPtr;
    typedef MethodType const*  ConstMethodPtr;

public:
    enum HttpRpcType {
        HTTP_XML,
        HTTP_JSON,
        HTTP_PROTOBUF
    };

public:
    ProtobufHttpMessageHandler();
    virtual ~ProtobufHttpMessageHandler();

    // client will call this function.
    virtual void CallMethod(ConstMethodPtr method,
                            google::protobuf::RpcController* controller,
                            const google::protobuf::Message* request,
                            google::protobuf::Message* response,
                            google::protobuf::Closure* done);

    virtual void channelConnected(ChannelHandlerContext& ctx, const ChannelStateEvent& e);
    virtual void channelDisconnected(ChannelHandlerContext& ctx, const ChannelStateEvent& e);
    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e);

    void setServices(ProtobufServiceRegister* serviceRegister);
    ProtobufServiceRegister* getServices() { return serviceRegister; }

protected:
    class RequestContext {
    public:
        typedef google::protobuf::Message MessageType;
        typedef google::protobuf::MethodDescriptor MethodType;
        typedef MessageType*      MessagePtr;
        typedef MethodType*       MethodPtr;
        typedef MethodType const* ConstMethodPtr;

    public:
        std::string    serviceName;
        std::string    methodName;

        ProtobufServicePtr service;
        ConstMethodPtr     method;

        MessagePtr     protobufRequest;
        MessagePtr     protobufResponse;

        HttpRequestPtr  httpRequest;
        HttpResponsePtr httpResponse;

        RequestContext() : method(NULL), protobufRequest(NULL), protobufResponse(NULL) {}
        ~RequestContext() { reset(); }

        void reset();
        bool methodValid() const { return (service && method); }
        bool isValid() const { return protobufResponse != NULL; }
    };

protected:
    virtual int  getRpcInfo(const HttpRequestPtr& request, std::string* service, std::string* method) = 0;
    virtual int  decodeRequest(RequestContext& context) = 0;
    virtual void encodeResponse(RequestContext& context) = 0;

private:
    void doneCallback(RequestContext* context);
    void writeResponse(RequestContext* context);

private:
    Channel* channel;

    RequestContext context;
    ProtobufServiceRegister*  serviceRegister;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFHTTPMESSAGEHANDLER_H)

// Local Variables:
// mode: c++
// End:

