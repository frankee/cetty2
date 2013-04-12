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

#include <boost/noncopyable.hpp>
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

    bool empty() const;

    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;

    ConstIterator find(const ChannelOption& option) const;

    ChannelOptions& setOption(const ChannelOption& option,
                              const ChannelOption::Variant& value);

private:
    OptionsMap options_;
};

inline
bool ChannelOptions::empty() const {
    return options_.empty();
}

inline
ChannelOptions::Iterator ChannelOptions::begin() {
    return options_.begin();
}

inline
ChannelOptions::Iterator ChannelOptions::end() {
    return options_.end();
}

inline
ChannelOptions::ConstIterator ChannelOptions::begin() const {
    return options_.begin();
}

inline
ChannelOptions::ConstIterator ChannelOptions::end() const {
    return options_.end();
}

inline
ChannelOptions::ConstIterator ChannelOptions::find(const ChannelOption& key) const {
    return options_.find(key);
}

inline
ChannelOptions& ChannelOptions::setOption(const ChannelOption& key,
        const ChannelOption::Variant& value) {
    if (value.empty()) {
        options_.erase(key);
        LOG_WARN << "setOption, the key ("
                 << key.name()
                 << ") is empty value, remove from the options.";
    }
    else {
        LOG_INFO << "set Option, the key is " << key.name();
        options_.insert(std::make_pair(key, value));
    }

    return *this;
}

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELOPTIONS_H)

// Local Variables:
// mode: c++
// End:
