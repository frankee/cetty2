#if !defined(CETTY_UTIL_EXTERNALRESOURCEUTIL_H)
#define CETTY_UTIL_EXTERNALRESOURCEUTIL_H

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
 * A utility class that provides the convenient shutdown of
 * {@link ExternalResourceReleasable}s.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class ExternalResourceUtil {
public:
    /**
     * Releases the specified {@link ExternalResourceReleasable}s.
     */
    static void release(ExternalResourceReleasable* releasable, ...) {
        ExternalResourceReleasable[] releasablesCopy =
            new ExternalResourceReleasable[releasables.length];

        for (int i = 0; i < releasables.length; i ++) {
            if (releasables[i] == null) {
                throw new NullPointerException("releasables[" + i + "]");
            }

            releasablesCopy[i] = releasables[i];
        }

for (ExternalResourceReleasable e: releasablesCopy) {
            e.releaseExternalResources();
        }
    }

    static void release(std::vector<ExternalResourceReleasable*> releasables)

private:
    ExternalResourceUtil() {}
    virtual ~ExternalResourceUtil() {}
};

}
}

#endif //#if !defined(CETTY_UTIL_EXTERNALRESOURCEUTIL_H)

// Local Variables:
// mode: c++
// End:
