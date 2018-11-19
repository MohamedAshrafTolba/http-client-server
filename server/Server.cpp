#include "Server.h"

Server::Server(std::string server_name, std::string port_number, int backlog, int max_workers) {
    this->server_name = server_name;
    this->port_number = port_number;
    this->backlog = backlog;
    this->max_workers = max_workers;
    server_socket = new ServerSocket(port_number, backlog);
}

Server::~Server() {
    delete(server_socket);
}

void Server::run() {
    while (true) {
        while (workers_pool.size() >= max_workers) {
            sleep(2);
        }

        int connecting_socket_fd = server_socket->accept_connection();

        if (connecting_socket_fd == -1) {
            // Error
        }

        Socket *client_socket = new Socket(connecting_socket_fd);

        // TODO: Create a thread for this socket
    }
}

std::string Server::get_server_name() const {
    return server_name;
}

std::string Server::get_port_number() const {
    return port_number;
}

int Server::get_backlog() const {
    return backlog;
}

int Server::get_max_workers() const {
    return max_workers;
}