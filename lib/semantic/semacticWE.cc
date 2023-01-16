/*
* semantic analyzer warnings and errors.
* Wang Xiaochun
* zjjhwxc(at)gmail.com
* March 7, 2013
*
*/
#include <cstdarg>

#include "semantic.h"
#include "type.h"

namespace
{
    //char *semWarnings[] =
    const char *semErrors[] = {"NULL",

                               "SE: Too many types.\n"
                               //SEM_ERR_TOO_TYPE

                               "SE02 : redeclaration of %s."


    };
};  // namespace

int semErrorCount = 0;

void semError(int errType, ...)
{
    semErrorCount++;
#if defined SEM_DEBUG_ASM_BP && defined WIN32
    __asm {
		int 3
    }
#endif
    //fprintf(wxccErr, "Error:line: %d.>> ", line);
    switch (errType)
    {
        case SEM_ERR_TOO_TYPE:
            fprintf(wxccErr, "%s", semErrors[errType]);
            break;
        case USE_MY_STR_SSD: {
            va_list ap;
            va_start(ap, errType);
            const char *str1 = va_arg(ap, const char *);
            const char *str2 = va_arg(ap, const char *);
            int i1 = va_arg(ap, int);
            int i2 = va_arg(ap, int);
            fprintf(wxccErr, str1, str2, i1, i2);
            va_end(ap);
            break;
        }
        default:
            assert(0);
    }
}

void semWarning(int errType, ...);
