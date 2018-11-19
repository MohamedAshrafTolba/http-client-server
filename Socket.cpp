#include "Socket.h"

Socket::Socket(std::string host_name, std::string port_number) {
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

std::string Socket::get_http_headers() {
    // TODO
    return NULL;
}

std::string Socket::get_http_body(size_t http_body_size) {
    // TODO
    return NULL;
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

std::string Socket::get_port_name() const {
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