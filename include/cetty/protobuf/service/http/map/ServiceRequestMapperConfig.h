

class ServiceRequestMapperConfig {
    class Template {
        std::string uri;
        std::string method;
        std::string cookie;
    };

    std::vector<Template> templates;
};
