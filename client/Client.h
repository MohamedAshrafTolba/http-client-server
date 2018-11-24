#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <queue>
#include "../Socket.h"
#include "../HttpRequest.h"

#define HTTP_VERSION "HTTP/1.1"

class Client {
    public:
        Client(std::string &host_name, std::string &port_number,
                std::string &requests_file, bool dry_run = false);
        ~Client();
        void run();
        int get_client_socket_fd() const;
        std::string get_requests_file() const;
        bool is_dry_run() const;
    private:
        std::string read_file(std::string &file_name);
        void write_file(std::string &file_name, std::string &content);
        void make_request(RequestMethod method, std::string &file_name);
        std::string get_response();
        Socket *socket;
        std::queue<std::string> pipelined_requests;
        std::string requests_file;
        bool dry_run;
};

#endif