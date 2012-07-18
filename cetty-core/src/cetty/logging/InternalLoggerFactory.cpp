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

#include <algorithm>

#include <cetty/util/Exception.h>
#include <cetty/logging/InternalLoggerFactory.h>
#include <cetty/logging/SimpleLoggerFactory.h>

namespace cetty {
namespace logging {

using namespace cetty::util;

InternalLoggerFactory* cetty::logging::InternalLoggerFactory::defaultFactory = NULL;
std::map<std::string, InternalLogger*>* InternalLoggerFactory::loggers = NULL;

InternalLoggerFactory* InternalLoggerFactory::getDefaultFactory() {
    if (NULL == defaultFactory) {
        defaultFactory = new SimpleLoggerFactory;
    }

    return defaultFactory;
}

void InternalLoggerFactory::setDefaultFactory(InternalLoggerFactory* defaultFactory) {
    if (defaultFactory == NULL) {
        throw NullPointerException("defaultFactory");
    }

    if (NULL != InternalLoggerFactory::defaultFactory) {
        delete  InternalLoggerFactory::defaultFactory;
        InternalLoggerFactory::defaultFactory = NULL;
    }

    InternalLoggerFactory::defaultFactory = defaultFactory;
}

InternalLogger* InternalLoggerFactory::getInstance(const std::string& name) {
    if (NULL == loggers) {
        loggers = new std::map<std::string, InternalLogger*>();
    }

    std::map<std::string, InternalLogger*>::iterator itr = loggers->find(name);

    if (itr != loggers->end()) {
        return itr->second;
    }

    InternalLogger* logger = getDefaultFactory()->newInstance(name);

    if (logger) {
        loggers->insert(std::make_pair(name, logger));
    }

    return logger;
}

}
}
