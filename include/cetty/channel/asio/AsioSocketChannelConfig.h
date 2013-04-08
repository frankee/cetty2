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

    const boost::optional<int>& sendBufferSize() const;
    void setSendBufferSize(int sendBufferSize);

    const boost::optional<int>& receiveBufferSize() const;
    void setReceiveBufferSize(int receiveBufferSize);

    const boost::optional<int>& soLinger() const;
    void setSoLinger(int soLinger);

    const boost::optional<bool>& isKeepAlive() const;
    void setKeepAlive(bool keepAlive);

    const boost::optional<bool>& isReuseAddress() const;
    void setReuseAddress(bool reuseAddress);

    const boost::optional<bool>& isTcpNoDelay() const;
    void setTcpNoDelay(bool tcpNoDelay);

    const boost::optional<int>& sendBufferHighWaterMark() const;
    void setSendBufferHighWaterMark(int bufferHighWaterMark);

    /**
     * Gets the <a><tt>SO_SNDLOWAT</tt></a> option.
     */
    const boost::optional<int>& sendBufferLowWaterMark() const;

    /**
     * Sets the <a><tt>SO_SNDLOWAT</tt></a> option.
     */
    void setSendBufferLowWaterMark(int bufferLowWaterMark);

    const boost::optional<int>& receiveBufferHighWaterMark() const;
    void setReceiveBufferHighWaterMark(int bufferHighWaterMark);

    /**
     * Gets the <a><tt>SO_RCVLOWAT</tt></a> option.
     */
    const boost::optional<int>& receiveBufferLowWaterMark() const;

    /**
     * Sets the <a><tt>SO_RCVLOWAT</tt></a> option.
     */
    void setReceiveBufferLowWaterMark(int bufferLowWaterMark);

private:
    template<typename Option>
    void setSocketOption(const ChannelOption& key,
                         bool src,
                         boost::optional<bool>* dest) {
        BOOST_ASSERT(dest);
        boost::optional<bool>& value = *dest;

        if (value && *value == src) {
            LOG_WARN << "the new " << key.name()
                     << " is same with the old, need not set";
            return;
        }

        if (socket_.is_open() && applyOptionToSocket<Option>(key, src)) {
            value = src;
        }
    }

    template<typename Option>
    void setSocketOption(const ChannelOption& key,
                         int src,
                         boost::optional<int>* dest) {
        BOOST_ASSERT(dest);
        boost::optional<int>& value = *dest;

        if (value && *value == src) {
            LOG_WARN << "the new " << key.name()
                     << " is same with the old, need not set";
            return;
        }

        if (src > 0) {
            if (socket_.is_open() && applyOptionToSocket<Option>(key, src)) {
                *dest = src;
            }
        }
        else {
            LOG_WARN << "the " << key.name() << " " << src
                     << " should not be negative";
        }
    }

    template<typename Option, typename Value>
    bool applyOptionToSocket(const ChannelOption& key, Value value) {
        boost::system::error_code ec;
        socket_.set_option(Option(value), ec);

        if (ec) {
            LOG_ERROR << "failed to set " << key.name()
                      << " " << value
                      << " to socket, code: " << ec.value()
                      << " message: " << ec.message();
            return false;
        }
        else {
            LOG_INFO << "has set " << key.name()
                     << " " << value << " to socket";
            return true;
        }
    }

    template<typename Option, typename Value>
    void updateOptionFromSocket(const ChannelOption& key, Value* value) const {
        BOOST_ASSERT(value);

        boost::system::error_code ec;
        Option option;
        socket_.get_option(option, ec);

        if (!ec) {
            *value = option.value();
        }
        else {
            LOG_ERROR << "fail to get " << key.name()
                      << " option from socket, code: " << ec.value()
                      << " message: " << ec.message();
        }
    }

private:
    TcpSocket& socket_;

    mutable boost::optional<bool> keepAlive_;
    mutable boost::optional<bool> tcpNoDelay_;
    mutable boost::optional<bool> reuseAddress_;

    mutable boost::optional<int>  soLinger_;
    mutable boost::optional<int>  sendBufferSize_;
    mutable boost::optional<int>  receiveBufferSize_;

    mutable boost::optional<int> sendBufferLowWaterMark_;
    mutable boost::optional<int> sendBufferHighWaterMark_;

    mutable boost::optional<int> receiveBufferLowWaterMark_;
    mutable boost::optional<int> receiveBufferHighWaterMark_;
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELCONFIG_H)

// Local Variables:
// mode: c++
// End:
