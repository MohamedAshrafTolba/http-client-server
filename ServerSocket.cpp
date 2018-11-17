#include "ServerSocket.h"

ServerSocket::ServerSocket(char port_number[], unsigned short backlog) {
    setup_socket(port_number, backlog);
}

ServerSocket::~ServerSocket() {
    shutdown_socket();
}

int ServerSocket::accept_connection(struct sockadddr *address, socklen_t *address_len) {
    return accept(address, address_len);
}

int ServerSocket::shutdown_socket() {
    return shutdown(socket_fd, SHUT_RDWR);
}

int ServerSocket::get_socket_fd() {
    return socket_fd;
}

void ServerSocket::setup_socket(char port_number[], unsigned short backlog) {
    struct addrinfo hints, *service_info, *itr;
    int yes = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, port_number, &hints, &service_info);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s.\n", gai_strerror(status));
        return;
    }

    for (itr = service_info ; itr != NULL ; itr = itr->ai_next) {
        socket_fd = socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
        if (socket_fd == -1) {
            perror("Error creating server socket ");
            continue;
        }

        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, 
                sizeof(int)) == -1) {
            shutdown_socket();
            perror("Error setting server socket options ");
            exit(1);
        }

        if (bind(socket_fd, itr->ai_addr, itr->ai_addrlen) == -1) {
            shutdown_socket();
            perror("Error binding server socket to the specified port ");
            continue;
        }

        break;
    }

    freeaddrinfo(service_info);

    if (itr == NULL) {
        shutdown_socket();
        fprintf(stderr, "Failed to bind socket to port.\n");
        exit(1);
    }

    if (listen(socket_fd, backlog) == -1) {
        shutdown_socket();
        perror("Error making socket listen to incoming connections ");
        exit(1);
    }
}