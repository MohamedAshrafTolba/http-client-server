#ifndef STRUTIL_H
#define STRUTIL_H

#include <string>

namespace strutil {
    bool iequals(std::string a, std::string b);
    char *time_cstr(time_t *t);
}

#endif
