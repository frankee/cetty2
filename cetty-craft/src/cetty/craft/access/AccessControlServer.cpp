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

#include <cetty/config/ConfigCenter.h>
#include <cetty/craft/builder/CraftServerBuilder.h>
#include <cetty/craft/access/AccessControlServiceImpl.h>

using namespace cetty::config;
using namespace cetty::craft::builder;

int main(int argc, char* argv[]) {
    ConfigCenter::instance().load(argc, argv);

    CraftServerBuilder builder;
    builder.registerService(new cetty::craft::access::AccessControlServiceImpl);
    builder.buildRpc(1980);
    builder.buildHttp(8080);

    //builder.buildAll();
    builder.waitingForExit();

    return 0;
}
