#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFUTIL_H)
#define CETTY_PROTOBUF_SERVICE_PROTOBUFUTIL_H

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
#include <boost/cstdint.hpp>
#include <boost/variant.hpp>

namespace google {
namespace protobuf {
class Message;
}
}

namespace cetty {
namespace protobuf {
namespace service {

using google::protobuf::Message;

class ProtobufUtil {
public:
    typedef boost::variant<boost::int64_t,
            double,
            const std::string*,
            const Message*,
            std::vector<boost::int64_t>,
            std::vector<double>,
            std::vector<const std::string*>,
            std::vector<const Message*> > FieldValue;

public:
    static FieldValue getMessageField(const std::string& name,
                                      const Message& message);

private:
    ProtobufUtil();
};

}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFUTIL_H)

// Local Variables:
// mode: c++
// End:
