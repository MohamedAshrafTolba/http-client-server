#include <iostream>
#include "Client.h"

int main(int argc, char **argv) {
    if (argc < 4) {
        perror("Please enter hostname, port number and requests file path.");
        return 1;
    }
    std::string host_name(argv[1]);
    std::string port_number(argv[2]);
    std::string requests_file(argv[3]);
    Client *client = new Client(host_name, port_number, requests_file);
    std::cout << "The client has been successfully connected to host:port  " 
            << host_name << ":" << port_number << std::endl;
    client->run();
    delete(client);
    return 0;
}
