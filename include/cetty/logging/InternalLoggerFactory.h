#if !defined(CETTY_LOGGING_INTERNALLOGGERFACTORY_H)
#define CETTY_LOGGING_INTERNALLOGGERFACTORY_H

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

#include <string>
#include <map>

namespace cetty {
namespace logging {

class InternalLogger;

/**
 * Creates an {@link InternalLogger} or changes the default factory
 * implementation.  This factory allows you to choose what logging framework
 * Cetty should use.  The default factory is {@link JdkLoggerFactory}.
 * You can change it to your preferred logging framework before other Cetty
 * classes are loaded:
 * <pre>
 * {@link InternalLoggerFactory}.setDefaultFactory(new {@link Log4JLoggerFactory}());
 * </pre>
 * Please note that the new default factory is effective only for the classes
 * which were loaded after the default factory is changed.  Therefore,
 * {@link #setDefaultFactory(InternalLoggerFactory)} should be called as early
 * as possible and shouldn't be called more than once.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 * @apiviz.has org.jboss.netty.logging.InternalLogger oneway - - creates
 */

class InternalLoggerFactory {
public:
    /**
     * Returns the default factory.  The initial default factory is
     * {@link JdkLoggerFactory}.
     */
    static InternalLoggerFactory* getDefaultFactory();

    /**
     * Changes the default factory.
     */
    static void setDefaultFactory(InternalLoggerFactory* defaultFactory);

    /**
     * Creates a new logger instance with the specified name.
     */
    static InternalLogger* getInstance(const std::string& name);

public:
    virtual ~InternalLoggerFactory() {}

    /**
     * Creates a new logger instance with the specified name.
     */
    virtual InternalLogger* newInstance(const std::string& name) = 0;

private:
    static InternalLoggerFactory* defaultFactory;
    static std::map<std::string, InternalLogger*>* loggers;
};

}
}

#endif //#if !defined(CETTY_LOGGING_INTERNALLOGGERFACTORY_H)

// Local Variables:
// mode: c++
// End:

