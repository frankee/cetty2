#if !defined(CETTY_CRAFT_HTTP_SERVICEREQUESTMAPPER_H)
#define CETTY_CRAFT_HTTP_SERVICEREQUESTMAPPER_H

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

#include <vector>
#include <string>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <cetty/util/StringPiece.h>
#include <cetty/handler/codec/http/HttpRequestPtr.h>
#include <cetty/protobuf/service/ProtobufServicePtr.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>

namespace google {
namespace protobuf {
class Message;
class FieldDescriptor;
}
}

class CraftFieldOptions;
class CraftMessageOptions;

namespace cetty {
namespace craft {
namespace http {

using namespace google::protobuf;
using namespace cetty::util;
using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;

class ServiceMethod;

class ServiceRequestMapping : private boost::noncopyable {
public:
    typedef boost::function<bool (StringPiece const&, Message*)> Consumer;

public:
    static ServiceRequestMapping& instance();

public:
    void registerConsumer(const std::string& name, const Consumer& consumer);

    ProtobufServiceMessagePtr toProtobufMessage(const HttpRequestPtr& request);

private:
    ServiceRequestMapping();
    ~ServiceRequestMapping();

    bool parseMessage(const HttpRequestPtr& request,
                      const ServiceMethod& method,
                      Message* message);

    bool parseMessage(const StringPiece& value,
                      const std::string& consumer,
                      Message* message);

    bool parseField(const HttpRequestPtr& request,
                    const ServiceMethod& method,
                    const CraftFieldOptions& options,
                    const FieldDescriptor* field,
                    Message* message);

    bool getValues(const HttpRequestPtr& request,
                   const ServiceMethod& method,
                   const CraftFieldOptions& options,
                   std::vector<StringPiece>* values);

    StringPiece getValue(const HttpRequestPtr& request,
                         const CraftMessageOptions& options);

    ServiceMethod* route(const HttpRequestPtr& request);

    void onServiceRegistered(const ProtobufServicePtr& service);

private:
    typedef boost::ptr_map<std::string, ServiceMethod> ServiceMethods;
    ServiceMethods serviceMethods_;

private:
    static ServiceRequestMapping* mapping_;
};

}
}
}

#endif //#if !defined(CETTY_CRAFT_HTTP_SERVICEREQUESTMAPPER_H)

// Local Variables:
// mode: c++
// End:
