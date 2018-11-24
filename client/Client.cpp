#include "Client.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <sys/stat.h>
#include <condition_variable>
#include "../strutil.h"

Client::Client(std::string &host_name, std::string &port_number,
        std::string &requests_file, bool dry_run) {
    socket = new Socket(host_name, port_number);
    this->requests_file = requests_file;
    this->dry_run = dry_run;
}

Client::~Client() {
    if (socket != nullptr) {
        delete socket;
    }
}

void Client::run() {
    std::mutex pipelined_requests_mutex;
    std::mutex running_mutex;
    bool running = true;
    auto f = [&] {
        running_mutex.lock();
        while (running) {
            running_mutex.unlock();
            pipelined_requests_mutex.lock();
            if (pipelined_requests.empty()) {
                pipelined_requests_mutex.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(2));
            } else {
                std::string file_name = pipelined_requests.front();
                pipelined_requests.pop();
                pipelined_requests_mutex.unlock();
                std::cout << file_name << ":\n" << get_response(GET, file_name);
            }
        }
        pipelined_requests_mutex.lock();
        while (!pipelined_requests.empty()) {
            std::string file_name = pipelined_requests.front();
            pipelined_requests.pop();
            pipelined_requests_mutex.unlock();
            std::cout << file_name << ":\n" << get_response(GET, file_name);
        }
        pipelined_requests_mutex.unlock();
    };

    std::thread pipelining_thread(f);

    std::ifstream requests_file_stream(requests_file);
    std::string request;
    while (std::getline(requests_file_stream, request)) {
        if (request.empty()) {
            continue;
        }
        std::stringstream split_stream(request);
        std::string method, file_name;
        split_stream >> method;
        if (!split_stream.eof() && split_stream.tellg() != -1) {
            split_stream >> file_name;
        } else {
            perror("Skipping request: File name is missing");
            continue;
        }
        RequestMethod req_method = NOP;
        if (strutil::iequals(method, "GET")) {
            req_method = GET;
        } else if (strutil::iequals(method, "POST")) {
            req_method = POST;
        }
        make_request(req_method, file_name);
        if (req_method == POST) {
            get_response(POST, file_name);
        } else {
            pipelined_requests_mutex.lock();
            pipelined_requests.push(file_name);
            pipelined_requests_mutex.unlock();
        }
    }
    running_mutex.lock();
    running = false;
    running_mutex.unlock();

    pipelining_thread.join();
}

int Client::get_client_socket_fd() const {
    return socket->get_socket_fd();
}

std::string Client::get_requests_file() const {
    return requests_file;
}

bool Client::is_dry_run() const {
    return dry_run;
}

std::string Client::read_file(std::string &file_name) {
    std::string file_path = "." + file_name;
    std::ifstream input_stream(file_path);
    std::string contents(std::istreambuf_iterator<char>(input_stream), {});
    input_stream.close();
    return contents;
}

long Client::get_file_size(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

void Client::write_file(std::string &file_name, std::string &contents) {
    std::string file_path = "." + file_name;
    std::ofstream output_stream(file_path);
    output_stream << contents;
    output_stream.close();
}

void Client::make_request(RequestMethod method, std::string &file_name) {
    std::stringstream request_stream;
    const static std::string req_method[] = {"GET", "POST", "INVALID"};
    request_stream << req_method[method] << ' ' << file_name << ' ' << HTTP_VERSION << "\r\n";
    if (method == POST) {
        request_stream << "Content-Length: " << get_file_size("." + file_name) << "\r\n\r\n";
        
    } else {
        request_stream << "\r\n";
    }
    std::string request = request_stream.str();
    socket->send_http_msg(request);
}

std::string Client::get_response(RequestMethod method, std::string &file_name) {
    std::string headers = socket->recieve_http_msg_headers();
    std::string file_path = "." + file_name;
    if (method == POST) {
        if (headers.find("200 OK") != std::string::npos) {
            std::string file_contents = read_file(file_path);
            socket->send_http_msg(file_contents);
        }
        return headers;
    }
    HttpRequest request(headers); // Hacky implementation to parse the options
    std::string content_length_str = request.get_options()["Content-Length"];
    int content_length = atoi(content_length_str.c_str());
    std::string body = socket->recieve_http_msg_body(content_length);
    if (!dry_run) {
        write_file(file_name, body);
    }
    return headers + "\r\n\r\n" + body;
}