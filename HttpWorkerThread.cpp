#include "HttpWorkerThread.h"
#include "HttpRequest.h"
#include "strutil.h"
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>

using std::chrono::duration_cast;
using std::chrono::steady_clock;
using boost::filesystem::exists;
using boost::filesystem::last_write_time;
using boost::filesystem::path;
using boost::filesystem::file_size;
using boost::filesystem::load_string_file;
using boost::filesystem::save_string_file;

HttpWorkerThread::HttpWorkerThread(int socket_fd, std::string &http_version, int timeout) {
    std::cout << "Started new worker for socket: " << socket_fd << std::endl;
    done = false;
    this->socket = new Socket(socket_fd);
    this->timeout = timeout;
    this->http_version = http_version;
    start();
}

HttpWorkerThread::~HttpWorkerThread(){
    std::cout << "A worker just terminated" << std::endl; 
    if (worker != nullptr) {
        worker->join();
    }
    delete worker;
    if (socket != nullptr) {
        socket->close();
    } 
    delete socket;
}

bool HttpWorkerThread::is_done() {
    return done;
}

void HttpWorkerThread::start() {
    start_time = steady_clock::now();
    worker = new std::thread(&HttpWorkerThread::execute, this);
}

void HttpWorkerThread::execute() {
    while (!done) {
        std::string headers = socket->receive_http_msg_headers(false);
        if (!headers.empty()) {
            std::cout << "\n\nHeaders of Received Request\n" << headers << "\n\n";
            handle_http_request(headers);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        auto now = steady_clock::now();
        int64_t time_diff = duration_cast<std::chrono::seconds>(now - start_time).count();
        if (time_diff >= timeout) {
            done = true;
        }
    }
    std::cout << "A worker just timed out" << std::endl; 
}

void HttpWorkerThread::handle_http_request(std::string &http_request) {
    HttpRequest request(http_request);
    std::stringstream response_stream;
    std::string file_name = "." + request.get_file_url();
    path file_path = path(file_name);
    std::string response_code = "200 OK";
    
    if (request.get_request_method() == GET) {
        if (!exists(file_path)) {
            response_stream << http_version << ' ' << "404 Not Found" << "\r\n\r\n";
            std::string response = response_stream.str();
            socket->send_http_msg(response);
            return;
        }
        response_stream << http_version << ' ' << response_code << "\r\n";
        time_t now;
        time(&now);
        response_stream << "Date: " << strutil::time_cstr(&now) << "\r\n";
        response_stream << "Server: " << "Dummy" << "\r\n";
        time_t last_modified = last_write_time(file_path);
        response_stream << "Last-Modified: " << strutil::time_cstr(&last_modified) << "\r\n";
        response_stream << "Content-Length: " << file_size(file_path) << "\r\n";
        response_stream << "Content-Type: " << get_content_type(file_name) << "\r\n";
        response_stream << "\r\n";
        std::string response = response_stream.str();
        socket->send_http_msg(response);
        
        std::string data;
        load_string_file(file_path, data);
        socket->send_http_msg(data);

    } else if (request.get_request_method() == POST) {
        std::string length_str = request.get_options()["Content-Length"];
        int length = atoi(length_str.c_str());
        if (length <= 0) {
            response_stream << http_version << ' ' << "400 Bad Request" << "\r\n\r\n";
            std::string response = response_stream.str();
            socket->send_http_msg(response);
            std::cout << "Sent Response:\n" << response << std::endl;
            
            return;
        } else {
            response_stream << http_version << ' ' << response_code << "\r\n\r\n";
            std::string response = response_stream.str();
            socket->send_http_msg(response);

            std::cout << "Sent Response:\n" << response << std::endl;
            
            std::string body = socket->receive_http_msg_body(length);
            save_string_file(file_path, body);
        }
    }
}

std::string HttpWorkerThread::get_content_type(std::string url) {
	std::string contentType;
    std::string extension = url.substr(url.find_last_of(".") + 1);

	if (strutil::iequals(extension, "html")) {
		contentType = "text/html";
	} else if (strutil::iequals(extension, "txt")) {
		contentType = "text/txt";
	} else if (strutil::iequals(extension, "css")) {
		contentType = "text/css";
	} else if (strutil::iequals(extension, "js")) {
		contentType = "text/javascript";
	}else if (strutil::iequals(extension, "png")) {
		contentType = "image/png";
	} else if (strutil::iequals(extension, "jpg")) {
		contentType = "image/jpg";
	} else if (strutil::iequals(extension, "jpeg")) {
		contentType = "image/jpg";
	} else if (strutil::iequals(extension, "gif")) {
		contentType = "image/gif";
	} else {
		contentType = "text/txt";
	}
	return contentType;
}