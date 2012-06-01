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

#include <cetty/channel/ReceiveBufferSizePredictorFwd.h>
#include <cetty/channel/ReceiveBufferSizePredictorFactoryFwd.h>
#include <cetty/channel/socket/SocketChannelConfig.h>

namespace cetty {
namespace channel  {
namespace socket {
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

class AsioSocketChannelConfig : public cetty::channel::socket::SocketChannelConfig {
public:
    virtual ~AsioSocketChannelConfig() {}

    /**
     *
     */
    virtual int  getWriteBufferHighWaterMark() const = 0;

    /**
     *
     */
    virtual void setWriteBufferHighWaterMark(int writeBufferHighWaterMark) = 0;

    /**
     * Gets the <a><tt>SO_SNDLOWAT</tt></a> option.
     */
    virtual int  getWriteBufferLowWaterMark() const = 0;

    /**
     * Sets the <a><tt>SO_SNDLOWAT</tt></a> option.
     */
    virtual void setWriteBufferLowWaterMark(int writeBufferLowWaterMark) = 0;

    /**
     * Gets the <a><tt>SO_RCVLOWAT</tt></a> option.
     */
    virtual int  getReceiveBufferLowWaterMark() const = 0;

    /**
     * Sets the <a><tt>SO_RCVLOWAT</tt></a> option.
     */
    virtual void setReceiveBufferLowWaterMark(int receiveBufferLowWaterMark) = 0;

    /**
     * Returns the {@link ReceiveBufferSizePredictor} which predicts the
     * number of readable bytes in the socket receive buffer.  The default
     * predictor is <tt>{@link AdaptiveReceiveBufferSizePredictor}(64, 1024, 65536)</tt>.
     */
    virtual const ReceiveBufferSizePredictorPtr& getReceiveBufferSizePredictor() = 0;

    /**
     * Sets the {@link ReceiveBufferSizePredictor} which predicts the
     * number of readable bytes in the socket receive buffer.  The default
     * predictor is <tt>{@link AdaptiveReceiveBufferSizePredictor}(64, 1024, 65536)</tt>.
     */
    virtual void setReceiveBufferSizePredictor(const ReceiveBufferSizePredictorPtr& predictor) = 0;

    /**
     * Returns the {@link ReceiveBufferSizePredictorFactory} which creates a new
     * {@link ReceiveBufferSizePredictor} when a new channel is created and
     * no {@link ReceiveBufferSizePredictor} was set.  If no predictor was set
     * for the channel, {@link #setReceiveBufferSizePredictor(ReceiveBufferSizePredictor)}
     * will be called with the new predictor.  The default factory is
     * <tt>{@link AdaptiveReceiveBufferSizePredictorFactory}(64, 1024, 65536)</tt>.
     */
    virtual const ReceiveBufferSizePredictorFactoryPtr& getReceiveBufferSizePredictorFactory() const = 0;

    /**
     * Sets the {@link ReceiveBufferSizePredictor} which creates a new
     * {@link ReceiveBufferSizePredictor} when a new channel is created and
     * no {@link ReceiveBufferSizePredictor} was set.  If no predictor was set
     * for the channel, {@link #setReceiveBufferSizePredictor(ReceiveBufferSizePredictor)}
     * will be called with the new predictor.  The default factory is
     * <tt>{@link AdaptiveReceiveBufferSizePredictorFactory}(64, 1024, 65536)</tt>.
     */
    virtual void setReceiveBufferSizePredictorFactory(
        const ReceiveBufferSizePredictorFactoryPtr& predictorFactory) = 0;
};

}
}
}
}


#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELCONFIG_H)
