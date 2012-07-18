#if !defined(CETTY_CHANNEL_DEFAULTSERVERCHANNELCONFIG_H)
#define CETTY_CHANNEL_DEFAULTSERVERCHANNELCONFIG_H

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

#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelPipelineFactory.h>

namespace cetty {
namespace channel {

using namespace cetty::buffer;

class DefaultServerChannelConfig : public ChannelConfig {
public:
    /**
     * Creates a new instance.
     */
    DefaultServerChannelConfig();

    virtual void setOptions(const std::map<std::string, boost::any>& options);

    /**
     * Sets an individual option.  You can override this method to support
     * additional configuration parameters.
     */
    virtual bool setOption(const std::string& key, const boost::any& value);

    virtual const ChannelPipelineFactoryPtr& getPipelineFactory() const;
    virtual void setPipelineFactory(const ChannelPipelineFactoryPtr& pipelineFactory);

    virtual const ChannelBufferFactoryPtr& getBufferFactory() const;
    virtual void setBufferFactory(const ChannelBufferFactoryPtr& bufferFactory);

    virtual int  getConnectTimeout() const;
    virtual void setConnectTimeout(int connectTimeoutMillis);

    virtual bool channelHasReaderBuffer() const;

private:
    ChannelPipelineFactoryPtr pipelineFactory;
    ChannelBufferFactoryPtr bufferFactory;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_DEFAULTSERVERCHANNELCONFIG_H)

// Local Variables:
// mode: c++
// End:
