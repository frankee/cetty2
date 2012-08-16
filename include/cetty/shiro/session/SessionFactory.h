#if !defined(CETTY_SHIRO_SESSION_SESSIONFACTORY_H)
#define CETTY_SHIRO_SESSION_SESSIONFACTORY_H
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#include <cetty/shiro/session/Session.h>

namespace cetty {
namespace shiro {
namespace session {

/**
 * A simple factory class that instantiates concrete {@link Session Session} instances.  This is mainly a
 * mechanism to allow instances to be created at runtime if they need to be different the
 * defaults.  It is not used by end-users of the framework, but rather those configuring Shiro to work in an
 * application, and is typically injected into the {@link SecurityManager SecurityManager} or a
 * {@link SessionManager}.
 */
class SessionFactory {

    /**
     * Creates a new {@link SimpleSession SimpleSession} instance retaining the context's
     * {@link SessionContext#getHost() host} if one can be found.
     *
     * @param initData the initialization data to be used during {@link Session} creation.
     * @return a new {@link SimpleSession SimpleSession} instance
     */
public:
    static SessionPtr createSession(SessionContext &initData) {
        return SessionPtr(new Session(initData.getHost()));
    }
};
}
}
}
#endif // #if !define(CETTY_SHIRO_SESSION_SESSIONFACTORY_H)
