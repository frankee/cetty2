#if !defined(CETTY_CHANNEL_CHANNELOPTIONS_H)
#define CETTY_CHANNEL_CHANNELOPTIONS_H

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

#include <cetty/logging/LoggerHelper.h>
#include <cetty/channel/ChannelOption.h>

namespace cetty {
namespace channel {

class ChannelOptions {
public:
    typedef std::map<ChannelOption, ChannelOption::Variant> OptionsMap;

    typedef OptionsMap::iterator Iterator;
    typedef OptionsMap::const_iterator ConstIterator;

public:
    ChannelOptions() {}

    bool empty() const { return options_.empty(); }

    Iterator begin() { return options_.begin(); }
    Iterator end() { return options_.end(); }

    ConstIterator begin() const { return options_.begin(); }
    ConstIterator end() const { return options_.end(); }

    ConstIterator find(const ChannelOption& option) { return options_.find(option); }

    ChannelOptions& setOption(const ChannelOption& option,
                              const ChannelOption::Variant& value) {
        if (value.empty()) {
            options_.erase(option);
            LOG_WARN << "setOption, the key ("
                     << option.name()
                     << ") is empty value, remove from the options.";
        }
        else {
            LOG_INFO << "set Option, the key is " << option.name();
            options_.insert(std::make_pair(option, value));
        }

        return *this;
    }

private:
    OptionsMap options_;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELOPTIONS_H)

// Local Variables:
// mode: c++
// End:
