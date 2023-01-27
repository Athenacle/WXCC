#include "utils/error_manager.h"

#include <fmt/color.h>

#include <vector>

using namespace utils;
using namespace lex::types;

namespace fmt
{
    template <>
    struct formatter<Position> {
        static constexpr auto parse(fmt::format_parse_context& ctx)
        {
            return ctx.begin();
        }


        template <typename FormatContext>
        auto format(const Position& t, FormatContext& ctx) const
        {
            return fmt::format_to(ctx.out(), "(<{}>:{}.{})", *t.filename, t.line, t.place);
        }
    };
}  // namespace fmt

namespace
{
    /*
        static constexpr int FATAL = 0;
        static constexpr int ERROR = 1;
        static constexpr int WARNING = 2;
        static constexpr int INFO = 3;
    */
    const std::string& levelString(int level)
    {
        static std::vector<std::string> l = {
            {fmt::format("{}", fmt::styled("FATAL", fmt::fg(fmt::color::red)))},
            {fmt::format("{}", fmt::styled("ERROR", fmt::fg(fmt::color::red)))},
            {fmt::format("{}", fmt::styled("WARNING", fmt::fg(fmt::color::yellow)))},
            {fmt::format("{}", fmt::styled("INFO", fmt::fg(fmt::color::blue)))}};

        return l[level];
    }
}  // namespace


void ErrorManager::output(int level, std::string&& msg)
{
    fmt::print(stderr, " {}: {}\n", levelString(level), msg);

    if (unlikely(level == FATAL)) {
        std::exit(1);
    }
    addCounter(level);
}


void ErrorManager::output(int level, const Position& pos, std::string&& msg)
{
    fmt::print(stderr, " {}:{}: {}\n", levelString(level), pos, msg);

    if (unlikely(level == FATAL)) {
        std::exit(1);
    }
    addCounter(level);
}

void ErrorManager::addCounter(int l)
{
    switch (l) {
        case ERROR:
            err_ += 1;
            break;
        case WARNING:
            warning_ += 1;
            break;
        case INFO:
            info_ += 1;
            break;
        default:
            assert(0);
    }
}

ErrorManager::ErrorManager()
{
    err_ = warning_ = info_ = 0;
}

ErrorManager::~ErrorManager() = default;
