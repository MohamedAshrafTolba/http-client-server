#ifndef CLIENT_H
#define CLIENT_H

#include<string>
#include "Socket.h"
#include "HttpRequest.h"

#define HTTP_VERSION "HTTP/1.1"

class Client {
    public:
        Client(std::string &host, std::string &port,
         std::string &file_name, RequestMethod method, bool dry_run = false);
        ~Client();
        std::string get_response();
    private:
        std::string read_file(std::string &file_name);
        void write_file(std::string &file_name, std::string &content);
        void make_request();
        Socket *socket;
        std::string file_name;
        RequestMethod method;
        bool dry_run;
};

#endif