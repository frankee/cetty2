#if !defined(CETTY_SHIRO_SESSION_SESSIONVALIDATIONSCHEDULER_H)
#define CETTY_SHIRO_SESSION_SESSIONVALIDATIONSCHEDULER_H
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

namespace cetty {
namespace shiro {
namespace session {

class SessionValidationScheduler{
public:
    SessionValidationScheduler();
    ~SessionValidationScheduler(){}

    int getInterval() const{
        return interval;
    }

    void setInterval(int interval) {
        this->interval = interval;
    }

    bool isEnabled() const {
        return this->enabled;
    }

    /**
     * Creates a single thread  to validate sessions at fixed intervals
     * and enables this scheduler. The executor is created as a daemon thread to allow JVM to shut down
     */
    //TODO Implement an integration test to test for jvm exit as part of the standalone example
    // (so we don't have to change the unit test execution model for the core module)
    void enableSessionValidation() {};

    void run() {};
    void disableSessionValidation() {};

private:
    bool enabled;
    int  interval;
};

}
}
}
#endif // #if !defined(CETTY_SHIRO_SESSION_SESSIONVALIDATIONSCHEDULER_H)
