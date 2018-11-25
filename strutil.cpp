#include "strutil.h"
#include <cstring>
#include <iostream>
#include <boost/algorithm/string/predicate.hpp>

bool strutil::iequals(std::string a, std::string b) {
    return boost::iequals(a, b);
}

char *strutil::time_cstr(time_t *t) {
    char *time_str = ctime(t);
    time_str[strlen(time_str) - 1] = '\0';
    return time_str;
}
