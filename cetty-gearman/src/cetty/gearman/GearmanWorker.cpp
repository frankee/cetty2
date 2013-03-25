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

#include <cetty/gearman/GearmanWorker.h>

#include <cetty/bootstrap/ClientBootstrap.h>

namespace cetty {
namespace gearman {

using namespace cetty::bootstrap;
using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::service::pool;

GearmanWorker::GearmanWorker(const EventLoopPtr& eventLoop,
                             const Channel::Initializer& initializer,
                             const Connections& connections)
    : connectionPool_(connections,
                      eventLoop,
                     static_cast<int>(connections.size()),
                     static_cast<int>(connections.size())) {
    connectionPool_.setChannelInitializer(initializer);
    connectionPool_.start();
}

GearmanWorker::~GearmanWorker() {
}

void GearmanWorker::setCando(const std::string& cando) {

}

void GearmanWorker::setCando(const std::string& cando, int64_t timeout) {

}

void GearmanWorker::setCantdo(const std::string& cantdo) {

}

void GearmanWorker::resetAbilities() {

}

void GearmanWorker::grabJob() {

}

void GearmanWorker::grabUniqueJob() {

}

}
}
