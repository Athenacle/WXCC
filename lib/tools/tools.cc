/* useful tools.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
*/

#include "tools.h"

#include <cstring>

#include "system.h"

FILE* wxccErr;

void fatalError(const char* message)
{
    fprintf(stderr, "fatal error: %s.\n", message);

    exit(EXIT_FAILURE);
}


unsigned long utils::strHash(const char* st)
{
    // Knuth[1973b]: TAOCP Vol2
    unsigned long hash = 1315423911ul;
    while (size_t ch = (size_t)*st++) {
        hash = ((hash << 13) ^ (hash >> 3)) ^ ch;
    }
    return hash;
}

char* utils::strdup(const char* s)
{
    auto l = strlen(s);
    auto ret = new char[l + 1];
    strcpy(ret, s);
    return ret;
}