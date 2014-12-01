#include <cetty/config/ConfigCenter.h>
#include <cetty/craft/builder/CraftServerBuilder.h>
#include "echo.pb.h"

#include <yaml-cpp/yaml.h>
#include <cetty/protobuf/serialization/json/ProtobufJsonParser.h>
#include <cetty/protobuf/serialization/json/ProtobufJsonFormatter.h>

using namespace cetty::protobuf::serialization;

using namespace cetty::config;
using namespace cetty::craft::builder;

namespace echo {

class EchoServiceImpl : public EchoService {
public:
    EchoServiceImpl() {}
    virtual ~EchoServiceImpl() {}

    virtual void echo(const ConstEchoRequestPtr& request,
                      const EchoResponsePtr& response,
                      const DoneCallback& done) {
        EchoResponsePtr rep(response);

        if (!response) {
            rep = new EchoResponse;
        }

        //for (int i = 0; i < 100; ++i) {
            rep->mutable_payload()->append(request->payload());
        //}

        //boost::this_thread::sleep(boost::posix_time::microseconds(100));

        if (done) {
            done(rep);
        }
    }

	virtual void echoPair(const ConstEchoPairRequestPtr& request,
		const EchoPairResponsePtr& response,
		const DoneCallback& done) {
			response->mutable_pairs()->CopyFrom(request->pairs());
			if (done) {
				done(response);
			}
	}

};

}

using namespace echo;

int main(int argc, char* argv[]) {
    if (!ConfigCenter::instance().load(argc, argv)) {
        return -1;
    }
#if 0
	
	std::string out;
	YAML::Node node = YAML::Load("[{\"key\":\"k1\",\"value\":[12.333,-44.555,44444]},{\"key\":\"k2\",\"value\":25}]");
	ProtobufJsonFormatter::toJson(node, &out);

	EchoPairRequest req;
	ProtobufJsonParser parser;
	parser.parse(std::string("[{\"key\":\"k1\",\"value\":\"v1\"},{\"key\":\"k2\",\"value\":\"v2\"}]"), "pairs", &req);

	out.clear();
	ProtobufJsonFormatter formatter;
	formatter.format(req.pairs(), &out);
#endif
    CraftServerBuilder()
        .registerService(new echo::EchoServiceImpl)
        .buildAll()
        .waitingForExit();

    return 0;
}
