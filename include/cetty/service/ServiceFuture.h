#if !defined(CETTY_SERVICE_SERVICEFUTURE_H)
#define CETTY_SERVICE_SERVICEFUTURE_H

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

#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace service {

template <typename T>
class ServiceFuture : public cetty::util::ReferenceCounter<ServiceFuture<T>, int> {
public:
    typedef const T& ConstRefType;
    typedef ServiceFuture<T> SelfType;
    typedef const ServiceFuture<T>& SelfConstRefType;

    typedef typename boost::function1<void, SelfConstRefType, ConstRefType> Callback;

public:
    // If true, indicates that the client canceled the RPC, so the server may
    // as well give up on replying to it.  The server should still call the
    // final "done" callback.
    bool isCanceled() const;

    void addListenser(const Callback& callback);


    virtual void setComplete();

private:
    std::vector<Callback> callbacks;
};

}
}
#endif //#if !defined(CETTY_SERVICE_SERVICEFUTURE_H)


