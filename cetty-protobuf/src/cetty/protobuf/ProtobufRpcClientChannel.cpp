

#include <cetty/protobuf/ProtobufRpcChannel.h>

#include <boost/bind.hpp>
#include <google/protobuf/descriptor.h>

#include <cetty/protobuf/service.h>
#include <cetty/protobuf/rpc.pb.h>

// Call the given method of the remote service.  The signature of this
// procedure looks the same as Service::CallMethod(), but the requirements
// are less strict in one important way:  the request and response objects
// need not be of any specific class as long as their descriptors are
// method->input_type() and method->output_type().
void ProtobufRpcClientChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
        const ::google::protobuf::Message* request,
        const ::google::protobuf::Message* response,
        const DoneCallback& done) {

    if (!channel) { // the channel has not connected yet.
        if (done) {
            done->Run();
        }

        return;
    }

    proto::RpcMessage* message = new proto::RpcMessage;
    message->set_type(proto::REQUEST);

    message->set_service(method->service()->full_name());
    message->set_method(method->name());


    channel->write(ChannelMessage((MessageLite*)message));
}

