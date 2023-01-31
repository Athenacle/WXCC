#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include "system.h"

#include <vector>

NAMESPACE_BEGIN

namespace utils
{
    template <class... Args>
    CONSTEVAL uint32_t maskAnd(Args... args)
    {
        return (... & static_cast<uint32_t>(args));
    }

    template <class Arg1, class Arg2>
    CONSTEVAL bool maskMatch(Arg1 &&value, Arg2 &&mask)
    {
        return maskAnd(value, mask) == mask;
    }

    template <class... Args>
    CONSTEVAL uint32_t maskOr(Args... args)
    {
        return (... | static_cast<uint32_t>(args));
    }

    unsigned long strHash(const char *);

    char *strdup(const char *);

    std::string join(const std::vector<std::string> &, const std::string &);


}  // namespace utils

NAMESPACE_END

#endif
