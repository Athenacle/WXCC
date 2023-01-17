#ifndef WXCC_DRIVER_H_
#define WXCC_DRIVER_H_

#include <string>

struct CommandOptions {
    /// lexer
    bool lexerDebug;

    ///
    std::string filename;
    std::string output;
};


bool parseCommandOptions(CommandOptions&, int, const char*[]);

#endif
