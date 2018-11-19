#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <sys/socket.h>
#include <netdb.h>

#define MAX_BUFFER_SIZE 8192

class Socket {
    public:
        Socket(std::string &host_name, std::string &port_number);
        Socket(int socket_fd);
        ~Socket();
        std::string recieve_http_msg_headers();
        std::string recieve_http_msg_body(std::size_t http_body_size);
        std::size_t send_http_msg(std::string &message);
        int close();
        int get_socket_fd() const;
        std::string get_host_name() const;
        std::string get_port_number() const;
    private:
        int socket_fd;
        std::string socket_buffer;
        std::string host_name;
        std::string port_number;  
        void setup();
        bool read_socket_buffer(std::string &output);
};

#endif