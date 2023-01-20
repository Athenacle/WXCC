#ifndef WXCC_DRIVER_H_
#define WXCC_DRIVER_H_

#include <cstdio>
#include <string>

#include "utils/error_manager.h"

struct CommandOptions {
    /// lexer
    bool lexerDebug;

    ///
    std::string filename;
    std::string output;
};


FILE* openOutput(const CommandOptions& opt, utils::ErrorManager& mgr);
bool parseCommandOptions(CommandOptions&, int, const char*[]);

#endif
