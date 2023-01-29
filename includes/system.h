/* system configure.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
* This file should be included in each .h files.
*
*/

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "config.h"

#define USING_SYSTEM_MM_CALL
#define USING_STDERR

#ifdef WIN32
#pragma warning(disable : 4996) /* MS deprecation		*/
#pragma warning(disable : 4710) /* un-inlined function		*/
#pragma warning(disable : 4820) /* struct padding		*/
#pragma warning(disable : 4514) /* unused inline remove		*/
#pragma warning(disable : 4061) /* unprocessed enum s in switch	*/
#pragma warning(disable : 4986) /* unmatched exception		*/
#pragma warning(disable : 4350) /* unmatched exception		*/
#endif

/// system includes.
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <climits>
#include <string>

extern FILE *wxccErr;
extern int errCount;
extern int warningCount;
void fatalError(const char *);


#define V1 wxcc_v1

#define NAMESPACE_V1_BEGIN \
    namespace V1           \
    {

#define USING_V1      using namespace V1;

#define NAMESPACE_END }

NAMESPACE_V1_BEGIN
class Function;
NAMESPACE_END

#define V2 wxcc_v2


#define NAMESPACE_V2_BEGIN \
    namespace V2           \
    {
#define NAMESPACE_BEGIN NAMESPACE_V2_BEGIN

#define USING_V2        using namespace V2;

class NoneCopyable
{
    NoneCopyable(const NoneCopyable &)            = delete;
    NoneCopyable &operator=(const NoneCopyable &) = delete;

protected:
    NoneCopyable()  = default;
    ~NoneCopyable() = default;
};

#endif
