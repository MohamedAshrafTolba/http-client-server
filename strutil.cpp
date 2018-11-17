#include "strutil.h"
#include <cstring>

bool strutil::iequals(std::string a, std::string b) {
    return std::equal(a.begin(), a.end(), b.begin(),
    [] (const char& a, const char& b)
    {
        return (std::tolower(a) == std::tolower(b));
    });
}

char *strutil::time_cstr(time_t *t) {
    char *time_str = ctime(t);
    time_str[strlen(time_str) - 1] = '\0';
    return time_str;
}
