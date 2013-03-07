#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELCONFIG_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELCONFIG_H

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

#include <boost/optional.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <cetty/channel/ChannelConfig.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace cetty::channel;

/**
 * A {@link SocketChannelConfig} for a NIO TCP/IP {@link SocketChannel}.
 *
 * <h3>Available options</h3>
 *
 * In addition to the options provided by {@link ChannelConfig} and
 * {@link SocketChannelConfig}, {@link NioSocketChannelConfig} allows the
 * following options in the option map:
 *
 * <table border="1" cellspacing="0" cellpadding="6">
 * <tr>
 * <th>Name</th><th>Associated setter method</th>
 * </tr><tr>
 * <td><tt>"writeBufferHighWaterMark"</tt></td><td>{@link #setWriteBufferHighWaterMark(int)}</td>
 * </tr><tr>
 * <td><tt>"writeBufferLowWaterMark"</tt></td><td>{@link #setWriteBufferLowWaterMark(int)}</td>
 * </tr><tr>
 * <td><tt>"readBufferLowWaterMark"</tt></td><td>{@link #setReadBufferLowWaterMark(int)}</td>
 * </tr><tr>
 * <td><tt>"receiveBufferSizePredictor"</tt></td><td>{@link #setReceiveBufferSizePredictor(ReceiveBufferSizePredictor)}</td>
 * </tr><tr>
 * <td><tt>"receiveBufferSizePredictorFactory"</tt></td><td>{@link #setReceiveBufferSizePredictorFactory(ReceiveBufferSizePredictorFactory)}</td>
 * </tr>
 * </table>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */
class AsioSocketChannelConfig : private boost::noncopyable {
public:
    typedef boost::asio::ip::tcp::socket TcpSocket;

public:
    AsioSocketChannelConfig(TcpSocket& socket);

    bool setOption(const ChannelOption& option,
                           const ChannelOption::Variant& value);

    const boost::optional<int>& receiveBufferSize() const;
    const boost::optional<int>& sendBufferSize() const;
    const boost::optional<int>& soLinger() const;

    const boost::optional<bool>& isKeepAlive() const;
    const boost::optional<bool>& isReuseAddress() const;
    const boost::optional<bool>& isTcpNoDelay() const;

    void setKeepAlive(bool keepAlive);
    void setPerformancePreferences(int connectionTime,
        int latency,
        int bandwidth);

    void setReceiveBufferSize(int receiveBufferSize);
    void setReuseAddress(bool reuseAddress);
    void setSendBufferSize(int sendBufferSize);
    void setSoLinger(int soLinger);
    void setTcpNoDelay(bool tcpNoDelay);

    int writeBufferHighWaterMark() const;
    void setWriteBufferHighWaterMark(int writeBufferHighWaterMark);

    /**
     * Gets the <a><tt>SO_SNDLOWAT</tt></a> option.
     */
    int writeBufferLowWaterMark() const;

    /**
     * Sets the <a><tt>SO_SNDLOWAT</tt></a> option.
     */
    void setWriteBufferLowWaterMark(int writeBufferLowWaterMark);

    /**
     * Gets the <a><tt>SO_RCVLOWAT</tt></a> option.
     */
    int receiveBufferLowWaterMark() const;

    /**
     * Sets the <a><tt>SO_RCVLOWAT</tt></a> option.
     */
    void setReceiveBufferLowWaterMark(int receiveBufferLowWaterMark);

private:
    static const int DEFAULT_WRITE_BUFFER_HIGH_WATERMARK = 2 * 1024 * 1024;
    static const int DEFAULT_WRITE_BUFFER_LOW_WATERMARK  = 2 * 1024;

private:
    TcpSocket& socket_;

    mutable int writeBufferLowWaterMark_;
    mutable int writeBufferHighWaterMark_;

    mutable boost::optional<bool> keepAlive_;
    mutable boost::optional<bool> reuseAddress_;
    mutable boost::optional<bool> tcpNoDelay_;
    mutable boost::optional<int>  receiveBufferSize_;
    mutable boost::optional<int>  sendBufferSize_;
    mutable boost::optional<int>  soLinger_;
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELCONFIG_H)

// Local Variables:
// mode: c++
// End:
