#if !defined(CETTY_SHIRO_SUBJECTFACTORY_H)
#define CETTY_SHIRO_SUBJECTFACTORY_H
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
#include <cetty/shiro/subject/Subject.h>
namespace cetty { namespace shiro { namespace subject {
    class SubjectContext;
}}}

namespace cetty {
namespace shiro {
namespace subject {

using namespace cetty::shiro::subject;
/**
 * Default {@link SubjectFactory SubjectFactory} implementation that creates {@link org.apache.shiro.subject.support.DelegatingSubject DelegatingSubject}
 * instances.
 *
 * @since 1.0
 */
class SubjectFactory {
public:
    SubjectFactory(){}

    /**
     * Creates a new Subject instance reflecting the state of the specified contextual data.  The data would be
     * anything required to required to construct a {@code Subject} instance and its contents can vary based on
     * environment.  Any data supported by Shiro core will be accessible by one of the accessor methods.  All other
     * data is available as map {@link Map#get attribute}s.
     *
     * @param context the contextual data to be used by the implementation to construct an appropriate {@code Subject}
     *                instance.
     * @return a {@code Subject} instance created based on the specified context.
     */
void createSubject(SubjectContext &context, Subject *subject);

void newSubjectInstance(PrincipalCollection &principals,
                        bool authenticated,
                        const std::string &host,
                        SessionPtr &session,
                        SecurityManager *securityManager,
                        Subject *subject);
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_SUBJECTFACTORY_H)
