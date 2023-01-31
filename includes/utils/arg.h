#ifndef UTILS_ARG_H_
#define UTILS_ARG_H_

#include "system.h"

#include <vector>
NAMESPACE_BEGIN

namespace utils
{
    class CommandLineArgumentParser
    {
    public:
        std::string help();

        std::string parseArgument(std::vector<std::string> &);

        ~CommandLineArgumentParser() = default;

        CommandLineArgumentParser()  = default;
    };
}  // namespace utils
NAMESPACE_END

#endif
