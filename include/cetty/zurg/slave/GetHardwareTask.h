/*
 * GetHardwareTask.h
 *
 *  Created on: 2012-11-1
 *      Author: chenhl
 */

#ifndef GETHARDWARETASK_H_
#define GETHARDWARETASK_H_


#include <boost/intrusive_ptr.hpp>
#include <google/protobuf/stubs/common.h>

#include <cetty/util/ReferenceCounter.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/zurg/slave/slave.pb.h>
#include <cetty/zurg/slave/SlaveServiceImpl.h>
#include <cetty/zurg/slave/Process.h>

namespace cetty {
namespace zurg {
namespace slave {

class GetHardwareTask;
typedef boost::intrusive_ptr<GetHardwareTask> GetHardwareTaskPtr;

class GetHardwareTask : public cetty::util::ReferenceCounter<GetHardwareTask, int> {
public:
    GetHardwareTask(const ConstGetHardwareRequestPtr& request,
                    const GetHardwareResponsePtr& response,
                    const DoneCallback& done,
                    SlaveServiceImpl& slaveService);

    ~GetHardwareTask() {
        LOG_DEBUG << this;
    }

private:
    void start(SlaveServiceImpl& slave);

private:

#define DEFINE_DONE(KEY)                                         \
    void KEY##DONE(const google::protobuf::Message* response,    \
                   const google::protobuf::Message* request) {   \
        assert(KEY##Done == false);                              \
        KEY##Done = true;                                        \
        const RunCommandResponse* out =                          \
                google::protobuf::down_cast<const zurg::slave::RunCommandResponse*>(    \
                        response); \
        if (out->error_code() == 0) {               \
            this->response->set_##KEY(out->std_output()); \
        }                                           \
        delete response;                           \
        delete request;                            \
        checkAllDone();                             \
    }

    DEFINE_DONE(lspci)
    DEFINE_DONE(lscpu)
    DEFINE_DONE(lshw)
    DEFINE_DONE(ifconfig)

#undef DEFINE_DONE

    void checkAllDone();

private:
    bool lshw;
    bool lshwDone;
    bool lspciDone;
    bool lscpuDone;
    bool ifconfigDone;

    GetHardwareResponsePtr response;
    DoneCallback done;
};

}
}
}


#endif /* GETHARDWARETASK_H_ */
