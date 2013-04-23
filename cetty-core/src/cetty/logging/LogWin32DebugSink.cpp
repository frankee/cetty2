
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

#include <cetty/logging/LogWin32DebugSink.h>

#if defined(WIN32) && defined(DEBUG)
#include <windows.h>
#endif

namespace cetty {
namespace logging {

void LogWin32DebugSink::doSink(const LogMessage& msg) {
#if defined(WIN32) && defined(DEBUG)
    const char* buffer = message_.buffer();
    std::wstring out;
    ::cetty::util::StringUtil::utftoucs(buffer, &out);
    OutputDebugString(out.c_str());
#endif
}

void LogWin32DebugSink::doFlush() {
    // NOOP
}

}
}
