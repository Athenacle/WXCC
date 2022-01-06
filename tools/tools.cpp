/* useful tools.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
*/

#include "system.h"

void fatalError(const char *message)
{
    fprintf(stderr, "fatal error: %s.\n", message);
#if defined TOOL_DEBUG && defined WIN32
    __asm {
		int 3
    }
#endif
    exit(EXIT_FAILURE);
}