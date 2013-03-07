#if !defined(CETTY_CHANNEL_CHANNELHANDLERWRAPPER_H)
#define CETTY_CHANNEL_CHANNELHANDLERWRAPPER_H

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

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>

namespace cetty {
namespace channel {

template<class T>
struct ChannelHandlerWrapper {
    typedef T Handler;
    typedef boost::shared_ptr<T> HandlerPtr;
    typedef boost::shared_ptr<T> StoredHandlerPtr;
};

template<class T>
struct ChannelHandlerWrapper<boost::intrusive_ptr<T> > {
    typedef T Handler;
    typedef boost::intrusive_ptr<T> HandlerPtr;
    typedef boost::intrusive_ptr<T> StoredHandlerPtr;
};

template<class T>
struct ChannelHandlerWrapper<boost::shared_ptr<T> > {
    typedef T Handler;
    typedef boost::shared_ptr<T> HandlerPtr;
    typedef boost::shared_ptr<T> StoredHandlerPtr;
};

template<typename T>
struct ChannelHandlerWrapper<boost::weak_ptr<T> > {
    typedef T Handler;
    typedef boost::shared_ptr<T> HandlerPtr;
    typedef boost::weak_ptr<T>   StoredHandlerPtr;
};

template<typename T>
struct ChannelHandlerWrapper<T* > {
    typedef T  Handler;
    typedef T* HandlerPtr;
    typedef T* StoredHandlerPtr;
};

}
}


#endif //#if !defined(CETTY_CHANNEL_CHANNELHANDLERWRAPPER_H)

// Local Variables:
// mode: c++
// End:
