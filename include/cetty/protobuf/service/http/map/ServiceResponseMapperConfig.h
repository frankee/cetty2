

class ServiceResponseMapperConfig {
public:
    class Template {
    public:
        class Header {
        public:
            std::string name;
            std::string value;
        };

        std::string service;
        std::string method;
        std::string content;
        std::vector<Header> headers;
    };

    std::vector<Template*> templates;
};
