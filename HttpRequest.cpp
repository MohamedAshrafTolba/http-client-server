#include "HttpRequest.h"
#include "strutil.h"
#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>

using boost::algorithm::trim;

HttpRequest::HttpRequest(std::string &request) {
    parse(request);
}

HttpRequest::~HttpRequest() {

}

std::string HttpRequest::get_file_url() {
    return file_url;
}

std::unordered_map<std::string, std::string> &HttpRequest::get_options() {
    return options;
}

RequestMethod HttpRequest::get_request_method() {
    return method;
}

std::string HttpRequest::get_client_version() {
    return client_version;
}

std::string HttpRequest::get_body() {
    return body;
}

void HttpRequest::parse(std::string &request) {
    typedef boost::tokenizer<boost::char_separator<char> > 
    tokenizer;
    boost::char_separator<char> line_sep("\r\n");
    tokenizer lines(request, line_sep);
    int line_no = 0;
    for (std::string line : lines) {
        line_no++;
        trim(line);
        if (line_no == 1) {
            boost::char_separator<char> space_sep(" ");
            tokenizer fields(line, space_sep);
            int idx = 0;
            for (std::string field : fields) {
                idx++;
                switch(idx) {
                case 1:
                if (strutil::iequals(field, std::string("GET"))) {
                    method = GET;
                } else if (strutil::iequals(field, std::string("POST"))) {
                    method = POST;
                } else {
                    method = NOP;
                }
                break;
                case 2:
                    file_url = field;
                    break;
                case 3:
                    client_version = field;
                    break;
                } 
            }
         } else {
            auto colon_idx = line.find_first_of(":");
            if (colon_idx != std::string::npos) {
                std::string name = line.substr(0, colon_idx);
                trim(name);
                std::string value = line.substr(colon_idx + 1);
                trim(value);   
                options[name] = value;
            } else if (line.length() != 0) {
                body += line + "\n";
            }
        }
    }
    if (body.length() > 0) {
        body = body.substr(0, body.length() - 1);
    }
}
