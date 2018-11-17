#include "strutil.h"

bool strutil::iequals(std::string a, std::string b) {
    return std::equal(a.begin(), a.end(), b.begin(),
    [] (const char& a, const char& b)
    {
        return (std::tolower(a) == std::tolower(b));
    });
}
