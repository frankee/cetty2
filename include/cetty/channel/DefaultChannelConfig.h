#if !defined(CETTY_CHANNEL_DEFAULTCHANNELCONFIG_H)
#define CETTY_CHANNEL_DEFAULTCHANNELCONFIG_H

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

#include <cetty/channel/ChannelConfig.h>

namespace cetty {
namespace channel {

using namespace cetty::buffer;

/**
 * The default {@link SocketChannelConfig} implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class DefaultChannelConfig : public ChannelConfig {
public:
    /**
     * Creates a new instance.
     */
    DefaultChannelConfig();
    virtual ~DefaultChannelConfig() {}

    virtual void setOptions(const ChannelOption::Options& options);
    virtual bool setOption(const ChannelOption& option,
                           const ChannelOption::Variant& value);

    virtual int getConnectTimeout() const;
    virtual void setConnectTimeout(int connectTimeoutMillis);

private:
    int connectTimeoutMillis; // 10 seconds
};

}
}

#endif //#if !defined(CETTY_CHANNEL_DEFAULTCHANNELCONFIG_H)

// Local Variables:
// mode: c++
// End:

