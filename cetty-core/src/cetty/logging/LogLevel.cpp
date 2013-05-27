/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/logging/LogLevel.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace logging {

using namespace cetty::util;

const LogLevel LogLevel::FATAL(5, "FATAL");
const LogLevel LogLevel::ERROR(4, "ERROR");
const LogLevel LogLevel::WARN(3, "WARN");
const LogLevel LogLevel::INFO(2, "INFO");
const LogLevel LogLevel::DEBUG(1, "DEBUG");
const LogLevel LogLevel::TRACE(0, "TRACE");

}
}
