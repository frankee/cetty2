#if !defined(CETTY_CHANNEL_CHANNELCONFIG_H)
#define CETTY_CHANNEL_CHANNELCONFIG_H

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

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <cetty/channel/ChannelOptions.h>

namespace cetty {
namespace channel {

/**
 * A set of configuration properties of a {@link Channel}.
 * <p>
 * Please down-cast to more specific configuration type such as
 * {@link SocketChannelConfig} or use {@link #setOptions(const std::map<std::string, boost::any>&)} to set the
 * transport-specific properties:
 * <pre>
 * {@link Channel Channel&} ch = ...;
 * {@link SocketChannelConfig} *cfg = <strong>dynamic_cast<{@link SocketChannelConfig}>(&ch.getConfig());</strong>
 * cfg->setTcpNoDelay(false);
 * </pre>
 *
 * <h3>Option map</h3>
 *
 * An option map property is a dynamic write-only property which allows
 * the configuration of a {@link Channel} without down-casting its associated
 * {@link ChannelConfig}.  To update an option map, please call {@link #setOptions(const std::map<std::string, boost::any>&)}.
 * <p>
 * All {@link ChannelConfig} has the following options:
 *
 * <table border="1" cellspacing="0" cellpadding="6">
 * <tr>
 * <th>Name</th><th>Associated setter method</th>
 * </tr><tr>
 * <td><tt>"bufferFactory"</tt></td><td>{@link #setBufferFactory(ChannelBufferFactoryPtr)}</td>
 * </tr><tr>
 * <td><tt>"connectTimeoutMillis"</tt></td><td>{@link #setConnectTimeoutMillis(int)}</td>
 * </tr><tr>
 * <td><tt>"pipelineFactory"</tt></td><td>{@link #setPipelineFactory(const ChannelPipelineFactoryPtr& )}</td>
 * </tr><tr>
 * <td><tt>"channelOwnBufferSize"</tt></td><td>{@link #setChannelOwnBufferSize(int)}</td>
 * </tr>
 * </table>
 * <p>
 * More options are available in the sub-types of {@link ChannelConfig}.  For
 * example, you can configure the parameters which are specific to a TCP/IP
 * socket as explained in {@link SocketChannelConfig} or {@link AsioSocketChannelConfig}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */
class ChannelConfig : private boost::noncopyable {
public:
    typedef boost::function<bool (const ChannelOption&,
                                  const ChannelOption::Variant&)> OptionSetCallback;

public:
    ChannelConfig()
        : connectTimeoutMillis_(10000/*ms*/) {}

    ~ChannelConfig() {}

    /**
     * Sets the configuration properties from the specified map.
     *
     * @throws InvalidArgumentException if there is invalid value.
     */
    void setOptions(const ChannelOptions& options);

    /**
     * Sets a configuration property with the specified name and value.
     * To override this method properly, you must call the super class:
     * <pre>
     * bool setOption(const std::string& key, const boost::any& value) {
     *     if (super.setOption(key, value)) {
     *         return true;
     *     }
     *
     *     if (name.compare("additionalOption") == 0) {
     *         ....
     *         return true;
     *     }
     *
     *     return false;
     * }
     * </pre>
     *
     * @return <tt>true</tt> if and only if the property has been set
     *
     * @throws InvalidArgumentException if the value is invalid.
     */
    bool setOption(const ChannelOption& option,
                   const ChannelOption::Variant& value);

    /**
     * Returns the connect timeout of the channel in milliseconds.  If the
     * {@link Channel} does not support connect operation, this property is not
     * used at all, and therefore will be ignored.
     *
     * @return the connect timeout in milliseconds, <tt>0</tt> if disabled.
     */
    int connectTimeout() const;

    /**
     * Sets the connect timeout of the channel in milliseconds.  If the
     * {@link Channel} does not support connect operation, this property is not
     * used at all, and therefore will be ignored.
     *
     * @param connectTimeoutMillis the connect timeout in milliseconds.
     *                             <tt>0</tt> to disable.
     */
    void setConnectTimeout(int connectTimeoutMillis);

    const OptionSetCallback& optionSetCallback() const;

    void setOptionSetCallback(const OptionSetCallback& callback);

private:
    int connectTimeoutMillis_; // 10 seconds
    OptionSetCallback callback_;
};

inline
int ChannelConfig::connectTimeout() const {
    return connectTimeoutMillis_;
}

inline
const ChannelConfig::OptionSetCallback& ChannelConfig::optionSetCallback() const {
    return callback_;
}

inline
void ChannelConfig::setOptionSetCallback(
    const ChannelConfig::OptionSetCallback& callback) {
    callback_ = callback;
}

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELCONFIG_H)

// Local Variables:
// mode: c++
// End:
