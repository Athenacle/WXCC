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

#if defined HAVE_CONFIG_H && !defined WIN32
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

class Function;

#endif