#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <sys/socket.h>
#include <netdb.h>

class Socket {
    public:
        Socket(std::string host_name, std::string port_number);
        Socket(int socket_fd);
        ~Socket();
        std::string get_http_headers();
        std::string get_http_body(size_t http_body_size);
        int close();
        int get_socket_fd() const;
        std::string get_host_name() const;
        std::string get_port_name() const;
    private:
        int socket_fd;
        std::string host_name;
        std::string port_number;  
        void setup();
};

#endif