/* useful tools.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
*/

#include "system.h"

#include <cstring>
#include <numeric>

#include "utils/utils.h"

USING_V2
using namespace std;

FILE* wxccErr;

void fatalError(const char* message)
{
    fprintf(stderr, "fatal error: %s.\n", message);

    exit(EXIT_FAILURE);
}


unsigned long utils::strHash(const char* st)
{
    // Knuth[1973b]: TAOCP Vol2
    unsigned long hash = 1315423911UL;
    while (unsigned long ch = static_cast<unsigned char>(*st++)) {
        hash = ((hash << 13) ^ (hash >> 3)) ^ ch;
    }
    return hash;
}

char* utils::strdup(const char* s)
{
    auto l   = strlen(s);
    auto ret = new char[l + 1];
    strcpy(ret, s);
    return ret;
}


string utils::join(const vector<string>& in, const string& deli)
{
    if (in.empty()) {
        return "";
    }
    string out;
    accumulate(
        next(in.begin()), in.end(), out, [&deli](auto&& a, auto&& b) -> auto& {
            a += deli;
            a += b;
            return a;
        });
    return out;
}
