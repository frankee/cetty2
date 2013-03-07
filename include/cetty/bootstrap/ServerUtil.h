#if !defined(CETTY_BOOTSTRAP_SERVERUTIL_H)
#define CETTY_BOOTSTRAP_SERVERUTIL_H

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

#include <string>
#include <vector>
#include <boost/function.hpp>

namespace cetty {
namespace bootstrap {

class ServerUtil {
public:
    enum SignalType {
        SIG_RELOAD,
        SIG_STOP
    };

    typedef boost::function<void (const SignalType&)> SignalHandler;

public:
    static void daemonize();
    static void createPidFile(const std::string& pidfile);

    static void setupSignalHandlers();
    static void setupSignalHandlers(const SignalHandler& handler);

    static void registerSignalHandler(const SignalHandler& handler);

private:
    ServerUtil();

private:
    static std::vector<SignalHandler> handlers_;
};

}
}


#endif //#if !defined(CETTY_BOOTSTRAP_SERVERUTIL_H)

// Local Variables:
// mode: c++
// End:
