#if !defined(CETTY_UTIL_TIMEOUT_H)
#define CETTY_UTIL_TIMEOUT_H

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

#include <cetty/util/TimerPtr.h>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace util {

/**
 * A handle associated with a {@link TimerTask} that is returned by a
 * {@link Timer}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class Timeout : public cetty::util::ReferenceCounter<Timeout> {
public:
    virtual ~Timeout() {}

    /**
     * Returns the {@link Timer} that created this handle.
     */
    virtual const TimerPtr& getTimer() const = 0;

    /**
     * Returns <tt>true</tt> if and only if the {@link TimerTask} associated
     * with this handle has been expired.
     */
    virtual bool isExpired() const = 0;

    /**
     * Returns <tt>true</tt> if and only if the {@link TimerTask} associated
     * with this handle has been cancelled.
     */
    virtual bool isCancelled() const = 0;

    /**
     * Returns <tt>true</tt> if the {@link TimerTask} associated
     * with this handle is actived.
     */
    virtual bool isActived() const = 0;

    /**
     * Returns the time duration (milliseconds) from now to the absolute
     * expires time.
     */
    virtual int expiresFromNow() const = 0;

    /**
     * Cancels the {@link TimerTask} associated with this handle.  If the
     * task has been executed or canceled already, it will return with no
     * side effect.
     */
    virtual void cancel() = 0;
};

}
}

#endif //#if !defined(CETTY_UTIL_TIMEOUT_H)

// Local Variables:
// mode: c++
// End:
