/* system configure.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
* This file should be included in each .h files.
*
*/

#ifndef CONFIG_H
#define CONFIG_H

#if defined USE_CONFIG_H && !defined WIN32
#define DEBUG
#define _DEBUG
#include "config.h"
#endif

/// debug macros.
#ifdef _DEBUG
//#define LEX_DEBUG
#define TOOL_DEBUG
#define PARSER_DEBUG
#define IR_DEBUG
#endif

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
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <ctype.h>
#include <cstdarg>
#include <climits>
#include <cfloat>
#include <cerrno>
#include <assert.h>

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <iterator>
#include <vector>
class WXCC
{
};

extern FILE *wxccErr;
extern int errCount;
extern int warningCount;
void fatalError(const char *);

#define GD_OUTPUT

#endif