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

#include <cetty/logging/Log4cplusLoggerFactory.h>
#include <boost/assert.hpp>

#if defined(HAS_LOG4CPLUS)

#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/configurator.h>
#include <cetty/logging/Log4cplusLogger.h>

namespace cetty {
namespace logging {

using namespace log4cplus;
using namespace log4cplus::helpers;

Log4cplusLoggerFactory::Log4cplusLoggerFactory(const std::string& configureFile) {
    try {
        PropertyConfigurator::doConfigure(configureFile);
    }
    catch (...) {
        //LOG4CPLUS_ERROR(root, "Exception occured while opening log4cplus.properties¡­");
    }
}

Log4cplusLoggerFactory::~Log4cplusLoggerFactory() {
    Logger::shutdown();
}

InternalLogger* Log4cplusLoggerFactory::newInstance(const std::string& name) {
    return new Log4cplusLogger(name, log4cplus::Logger::getInstance(name));
}

}
}

#else

namespace cetty {
namespace logging {

Log4cplusLoggerFactory::Log4cplusLoggerFactory(const std::string& configureFile) {}
Log4cplusLoggerFactory::~Log4cplusLoggerFactory() {}
InternalLogger* Log4cplusLoggerFactory::newInstance(const std::string& name) {
    InternalLoggerFactory* factory = InternalLoggerFactory::getDefaultFactory();
    BOOST_ASSERT(factory);
    return factory ? factory->newInstance(name) : NULL;
}

}
}

#endif