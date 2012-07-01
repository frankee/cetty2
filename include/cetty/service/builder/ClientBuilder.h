

template<typename ReqT, typename RepT>
class ClientBuilder {
public:
    ClientBuilder();
    ClientBuilder(ConfigureCenter& center);

    ClientServicePtr build();
};


ClientBuilder<HttpRequestPtr, HttpResponsePtr>()
.hosts()
.
.
.build();