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
    Subject *createSubject(SubjectContext *context);

    Subject *newSubjectInstance(PrincipalCollection *principals, bool authenticated, const std::string &host,
                                         Session *session, SecurityManager *securityManager) {
        return new Subject(principals, authenticated, host, session, securityManager);
    }
};
}
}

#endif // #if !defined(CETTY_SHIRO_SUBJECTFACTORY_H)
