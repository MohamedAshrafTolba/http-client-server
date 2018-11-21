#include "Socket.h"
#include <cstring>
#include <chrono>
#include <thread>

Socket::Socket(std::string &host_name, std::string &port_number) {
    this->host_name = host_name;
    this->port_number = port_number;
    setup();
}

Socket::Socket(int socket_fd) {
    this->socket_fd = socket_fd;
}

Socket::~Socket() {
    close();
}

std::string Socket::recieve_http_msg_headers() {
    std::string http_headers;
    if (!read_http_headers_from_buffer(http_headers)) {
        bool http_headers_found = false;
        while (!http_headers_found) {
            std::string temp_read_buffer(MAX_BUFFER_SIZE, '\0');
            ssize_t bytes_recieved = recv(socket_fd, &temp_read_buffer[0], 
                    MAX_BUFFER_SIZE, 0);
            if (bytes_recieved < 0) {
                // Error
            }
            // while (temp_read_buffer.empty()) {
            //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // }
            socket_buffer += temp_read_buffer;
            http_headers_found = read_http_headers_from_buffer(http_headers);
        }
    }
    return http_headers;
}

std::string Socket::recieve_http_msg_body(std::size_t http_body_size) {
    std::string http_body;
    if (!read_http_body_from_buffer(http_body, http_body_size)) {
        std::string temp_read_buffer(http_body_size - http_body.length(), '\0');
        ssize_t bytes_recieved = recv(socket_fd, &temp_read_buffer[0], 
                temp_read_buffer.length(), 0);
        if (bytes_recieved < 0) {
            // Error
        }
        http_body += temp_read_buffer;
    }
    return http_body;
}

std::size_t Socket::send_http_msg(std::string &message) {
    std::string temp_write_buffer;
    unsigned long i = 0;
    while (i < message.length()) {
        temp_write_buffer = message.substr(i, 
                std::min(MAX_BUFFER_SIZE, (int)(message.length() - i)));
        ssize_t bytes_sent = send(socket_fd, temp_write_buffer.c_str(), 
                temp_write_buffer.length(), 0);
        if (bytes_sent < 0) {
            // Error
        }
        i += MAX_BUFFER_SIZE;
    }
    return message.length();
}

int Socket::close() {
    return shutdown(socket_fd, SHUT_RDWR);
}

int Socket::get_socket_fd() const {
    return socket_fd;
}

std::string Socket::get_host_name() const {
    return host_name;
}

std::string Socket::get_port_number() const {
    return port_number;
}

void Socket::setup() {
    struct addrinfo hints, *service_info, *itr;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    int status = getaddrinfo(host_name.c_str(), port_number.c_str(),
            &hints, &service_info);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s.\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    
    for (itr = service_info ; itr != NULL ; itr = itr->ai_next) {
        socket_fd = socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
        if (socket_fd == -1) {
            perror("Error creating client socket ");
            continue;
        }

        if (connect(socket_fd, itr->ai_addr, itr->ai_addrlen) == -1) {
            close();
            perror("Error connecting client socket ");
            continue;
        }
        break;
    }
    
    freeaddrinfo(service_info);
    
    if (itr == NULL) {
        fprintf(stderr, "Failed to connect client socket to the specified address.\n");
        exit(EXIT_FAILURE);
    }
}

bool Socket::read_http_headers_from_buffer(std::string &output) {
    if (!socket_buffer.empty()) {
        output += socket_buffer;
        std::size_t delim_pos_output = output.find("\r\n\r\n");
        if (delim_pos_output != std::string::npos) {
            socket_buffer = output.substr(delim_pos_output + 4);
            output = output.substr(0, delim_pos_output);
            return true;
        } else {
            socket_buffer.clear();
            return false;
        }
    } else {
        return false;
    }
}

bool Socket::read_http_body_from_buffer(std::string &output, 
        std::size_t http_body_size) {
    if (!socket_buffer.empty()) {
        if (http_body_size > socket_buffer.length()) {
            output += socket_buffer;
            socket_buffer.clear();
            return false;
        } else {
            output = socket_buffer.substr(0, http_body_size);
            socket_buffer = socket_buffer.substr(http_body_size + 1);
            return true;
        }
    } else {
        return false;
    }
}