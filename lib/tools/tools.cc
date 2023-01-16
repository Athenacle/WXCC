/* useful tools.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
*/

#include "system.h"
#include "tools.h"

FILE* wxccErr;

void fatalError(const char* message)
{
    fprintf(stderr, "fatal error: %s.\n", message);
#if defined TOOL_DEBUG && defined WIN32
    __asm {
		int 3
    }
#endif
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

const char* utils::strdup(const char* s)
{
    auto l = strlen(s);
    auto ret = new char[l + 1];
    strcpy(ret, s);
    return ret;
}