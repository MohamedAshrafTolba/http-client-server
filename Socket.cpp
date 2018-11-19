#include "Socket.h"

Socket::Socket(std::string &host_name, std::string &port_number) {
    this->host_name = host_name;
    this->port_number = port_number;
    socket_buffer.resize(MAX_BUFFER_SIZE + 1, '\0');
    setup();
}

Socket::Socket(int socket_fd) {
    this->socket_fd = socket_fd;
}

Socket::~Socket() {
    close();
}

std::string Socket::read_http_msg_headers() {
    std::string http_headers;
    if (!read_socket_buffer(http_headers)) {
        bool http_headers_found = false;
        while (!http_headers_found) {
            ssize_t bytes_recieved = recv(socket_fd, &socket_buffer[0],
                     MAX_BUFFER_SIZE, MSG_WAITALL);
            if (bytes_recieved < 0) {
                // Error
            }
            http_headers_found = read_socket_buffer(http_headers);
        }
    }
    http_headers += '\0';
    return http_headers;
}

std::string Socket::read_http_msg_body(std::size_t http_body_size) {
    std::string http_body;
    if (!read_socket_buffer(http_body)) {
        ssize_t bytes_recieved = recv(socket_fd, &http_body[0], 
                http_body_size - http_body.length() + 4, MSG_WAITALL);
        if (bytes_recieved < 0) {
            // Error
        }
    }
    http_body = http_body.substr(0, http_body.length - 4);
    http_body += '\0';
    return http_body;
}

std::size_t Socket::write_http_msg(std::string &message) {
    std::string socket_write_buffer;
    int i = 0;
    while (i < message.length()) {
        socket_write_buffer = message.substr(i, std::min(MAX_BUFFER_SIZE,
                (int)(message.length() - i)));
        
        ssize_t bytes_sent = send(socket_fd, &socket_write_buffer[0], MAX_BUFFER_SIZE, 0);
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

bool Socket::read_socket_buffer(std::string &output) {
    if (!socket_buffer.empty()) {
        std::size_t delim_pos = socket_buffer.find(HTTP_REQ_BLOCK_DELIMITER);
        if (delim_pos != std::string::npos) {
            output += socket_buffer.substr(0, delim_pos);
            socket_buffer = socket_buffer.substr(delim_pos + 4);
            return true;
        } else {
            output += socket_buffer;
            socket_buffer.clear();
            return false;
        }
    } else {
        return false;
    }
}