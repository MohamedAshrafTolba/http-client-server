#include "Client.h"
#include "../strutil.h"
#include <fstream>
#include <sstream>
#include <iostream>

#define PORT "8080"

int main(int argc, char **argv) {
    if (argc < 2) {
        perror("Please enter the input file name");
        return 1;
    }
    std::string input_file(argv[1]);
    std::ifstream input_stream(input_file);
    std::string line;
    while (std::getline(input_stream, line)) {
        std::stringstream split_stream(line);
        std::string method, file_name, host, port;
        split_stream >> method;
        split_stream >> file_name;
        split_stream >> host;
        if (!split_stream.eof() && split_stream.tellg() != -1) {
            split_stream >> port;
        } else {
            port = PORT;
        }
        RequestMethod req_method = NOP;
        if (strutil::iequals(method, "GET")) {
            req_method = GET;
        } else if (strutil::iequals(method, "POST")) {
            req_method = POST;
        }
        std::cout << Client(host, port, file_name, req_method, false).get_response() << "\n";
    }
}
