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

#include <cetty/craft/security/AccessController.h>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>

#include <cetty/craft/craft.pb.h>

namespace cetty {
namespace craft {
namespace security {

using namespace google::protobuf;
using namespace cetty::channel;
using namespace cetty::protobuf::service;

void AccessController::filter(ChannelHandlerContext& ctx,
                              const ProtobufServiceMessagePtr& req,
                              const FilterCallback& filterCallback) {
    Message* request = req->request();
    const google::protobuf::Descriptor* descriptor = request->GetDescriptor();

    int fieldCnt = descriptor->field_count();

    for (int i = 0; i < fieldCnt; ++i) {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);
        const std::string& fieldName = field->name();

        if (fieldName.compare("authority") == 0) {
            const google::protobuf::Reflection* reflection = request->GetReflection();
            const Message& msg = reflection->GetMessage(*request, field);
            const Authority* authority =
                internal::down_cast<Authority const*, Message const>(&msg);

            const std::string& service = req->service();
            const std::string& method = req->method();

            if (authority) {
                std::string permission(service);
                permission += ":";
                permission += method;

                if (authority->has_session()) {
                    //SecurityManager::permit(authority->session().id(), permission);
                }
                else if (authority->has_key()) { // app mode
                    //SecurityManager::permit(authority->key(), permission);
                }
                else {
                    LOG_ERROR << "has not authority info";
                }
            }
        }
    }

    // the request message is illegle.
    //outboundTransfer.unfoldAndAdd();
    //ctx.flush();
}

void AccessController::onAuthorized(bool permitted,
                                    const std::string& principal,
                                    const std::string& permission,
                                    ChannelHandlerContext& ctx,
                                    const ProtobufServiceMessagePtr& req) {
    if (permitted) {
        //inboundTransfer.unfoldAndAdd(req);
        //ctx.fireMessageUpdated();
    }
    else {
        //outboundTransfer.unfoldAndAdd();
        ctx.flush();
        LOG_WARN << principal << " authorize the " << permission << " failed.";
    }
}

}
}
}
