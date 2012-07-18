#if !defined(CETTY_UTIL_EXTERNALRESOURCERELEASABLE_H)
#define CETTY_UTIL_EXTERNALRESOURCERELEASABLE_H

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

namespace cetty {
namespace util {

/**
 * A common interface for a class which depends on external resources that
 * need explicit release or shutdown.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 */
class ExternalResourceReleasable {
public:
    virtual ~ExternalResourceReleasable() {}

    /**
     * Releases the external resources that this object depends on.  You should
     * not call this method if the external resources (e.g. thread pool) are
     * in use by other objects.
     */
    virtual void releaseExternalResources() = 0;
};

}
}

#endif //#if !defined(CETTY_UTIL_EXTERNALRESOURCERELEASABLE_H)

// Local Variables:
// mode: c++
// End:
