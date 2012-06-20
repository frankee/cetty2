#if !defined(CETTY_SERVICE_SPECILIZEDSERVICEFUTURE_H)
#define CETTY_SERVICE_SPECILIZEDSERVICEFUTURE_H

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

#include <boost/function.hpp>
#include <cetty/service/ServiceFuture.h>

namespace cetty {
namespace service {

template<typename GeneralT, typename SpecialT>
class SpecilizedServiceFuture : public ServiceFuture<GeneralT> {
public:
    typedef boost::intrusive_ptr<ServiceFuture<SpecialT> > SpecialTypePtr;
    typedef boost::function1<SpecialT, const GeneralT&> TypeCastFunctor;

public:
    SpecilizedServiceFuture(const SpecialTypePtr& future, const TypeCastFunctor& functor);
    virtual ~SpecilizedServiceFuture() {}

    virtual bool isCancelled() const {
        return future->isCancelled();
    }

    /**
     * Marks this future as a success and notifies all
     * listeners.
     *
     * @return <tt>true</tt> if and only if successfully marked this future as
     *         a success. Otherwise <tt>false</tt> because this future is
     *         already marked as either a success or a failure.
     */
    virtual bool setSuccess() {
        return future->setSuccess();
    }

    virtual bool setSuccess(const GeneralT& response) {
        this->setResponse(response);
        return future->setSuccess();
    }

    /**
     * Marks this future as a failure and notifies all
     * listeners.
     *
     * @return <tt>true</tt> if and only if successfully marked this future as
     *         a failure. Otherwise <tt>false</tt> because this future is
     *         already marked as either a success or a failure.
     */
    virtual bool setFailure(const Exception& cause) {
        return future->setFailure(cause);
    }

    virtual bool setFailure(const GeneralT& response, const Exception& cause) {
        this->setResponse(response);
        return future->setFailure(cause)
    }

    virtual void setResponse(const GeneralT& response) {
        ServiceFuture<GeneralT>::setResponse(response);
        future->setResponse(typecastFunctor(response));
    }

protected:
    void startCancelDelegator() {
        ServiceFuture<GeneralT>::startCancel();
    }

private:
    SpecialTypePtr future;
    TypeCastFunctor typecastFunctor;
};

}
}

#endif //#if !defined(CETTY_SERVICE_SPECILIZEDSERVICEFUTURE_H)

// Local Variables:
// mode: c++
// End:
