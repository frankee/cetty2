#if !defined(CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIOSOCKETCHANNELCONFIG_H)
#define CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIOSOCKETCHANNELCONFIG_H

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

#include <boost/asio/ip/tcp.hpp>

#include <cetty/channel/DefaultChannelConfig.h>
#include <cetty/channel/socket/asio/AsioSocketChannelConfig.h>

namespace cetty {
namespace logging {
class InternalLogger;
}
}

namespace cetty {
namespace channel  {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::logging;

class DefaultAsioSocketChannelConfig : public cetty::channel::DefaultChannelConfig,
    public AsioSocketChannelConfig {

public:
    typedef boost::asio::ip::tcp::socket TcpSocket;

public:
    DefaultAsioSocketChannelConfig(TcpSocket& socket);

    virtual bool setOption(const std::string& key, const boost::any& value);

    virtual const boost::optional<int>& getReceiveBufferSize() const;
    virtual const boost::optional<int>& getSendBufferSize() const;
    virtual const boost::optional<int>& getSoLinger() const;

    virtual const boost::optional<bool>& isKeepAlive() const;
    virtual const boost::optional<bool>& isReuseAddress() const;
    virtual const boost::optional<bool>& isTcpNoDelay() const;

    virtual void setKeepAlive(bool keepAlive);
    virtual void setPerformancePreferences(int connectionTime, int latency, int bandwidth);
    virtual void setReceiveBufferSize(int receiveBufferSize);
    virtual void setReuseAddress(bool reuseAddress);
    virtual void setSendBufferSize(int sendBufferSize);
    virtual void setSoLinger(int soLinger);
    virtual void setTcpNoDelay(bool tcpNoDelay);

    virtual int  getWriteBufferHighWaterMark() const;
    virtual void setWriteBufferHighWaterMark(int writeBufferHighWaterMark);

    virtual int  getWriteBufferLowWaterMark() const;
    virtual void setWriteBufferLowWaterMark(int writeBufferLowWaterMark);

    virtual int  getReceiveBufferLowWaterMark() const;
    virtual void setReceiveBufferLowWaterMark(int receiveBufferLowWaterMark);

    virtual const ReceiveBufferSizePredictorPtr& getReceiveBufferSizePredictor();
    virtual void setReceiveBufferSizePredictor(const ReceiveBufferSizePredictorPtr& predictor);

    virtual const ReceiveBufferSizePredictorFactoryPtr& getReceiveBufferSizePredictorFactory() const;
    virtual void setReceiveBufferSizePredictorFactory(const ReceiveBufferSizePredictorFactoryPtr& predictorFactory);

    virtual bool channelHasReaderBuffer() const;

private:
    static const int DEFAULT_WRITE_BUFFER_HIGH_WATERMARK = 2 * 1024 * 1024;
    static const int DEFAULT_WRITE_BUFFER_LOW_WATERMARK  = 2 * 1024;

private:
    static InternalLogger* logger;
    static ReceiveBufferSizePredictorFactoryPtr DEFAULT_PREDICTOR_FACTORY;

private:
    TcpSocket& socket;

    mutable boost::optional<bool> keepAlive;
    mutable boost::optional<bool> reuseAddress;
    mutable boost::optional<bool> tcpNoDelay;
    mutable boost::optional<int>  receiveBufferSize;
    mutable boost::optional<int>  sendBufferSize;
    mutable boost::optional<int>  soLinger;

    mutable int writeBufferLowWaterMark;
    int writeBufferHighWaterMark;

    ReceiveBufferSizePredictorPtr predictor;
    ReceiveBufferSizePredictorFactoryPtr predictorFactory;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_DEFAULTASIOSOCKETCHANNELCONFIG_H)

// Local Variables:
// mode: c++
// End:
