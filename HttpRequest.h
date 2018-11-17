#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <unordered_map>

enum RequestMethod {
    GET,
    POST,
    NOP
};

class HttpRequest {
    public:
        HttpRequest(std::string &request);
        ~HttpRequest();
        std::unordered_map<std::string, std::string> &get_options();
        std::string get_file_url();
        RequestMethod get_request_method();
        std::string get_client_version();
        std::string get_body();
    private:
        RequestMethod method;
        std::string client_version;
        std::string file_url;
        std::string body;
        std::unordered_map<std::string, std::string> options;
        void parse(std::string &request);
};

#endif
