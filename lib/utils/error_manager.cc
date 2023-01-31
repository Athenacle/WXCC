#include "utils/error_manager.h"

#include <fmt/color.h>

#include <vector>

USING_V2

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
            return fmt::format_to(ctx.out(), FMT("(<{}>:{}.{})"), *t.filename, t.line, t.place);
        }
    };
}  // namespace fmt

namespace
{

    const std::string& levelString(int level, bool color)
    {
        static std::vector<std::string> l = {
            {fmt::format("{}", fmt::styled("fatal", fmt::fg(fmt::color::red)))},
            {fmt::format("{}", fmt::styled("error", fmt::fg(fmt::color::red)))},
            {fmt::format("{}", fmt::styled("warning", fmt::fg(fmt::color::yellow)))},
            {fmt::format("{}", fmt::styled("info", fmt::fg(fmt::color::blue)))}};
        static std::vector<std::string> nc = {"fatal", "error", "warning", "info"};

        if (color) {
            return l[level];
        }
        return nc[level];
    }
}  // namespace


void ErrorManager::output(int level, std::string&& msg)
{
    fmt::print(stderr, FMT(" {}: {}\n"), levelString(level, color_), msg);

    if (unlikely(level == FATAL)) {
        std::exit(1);
    }
    addCounter(level);
}


void ErrorManager::output(int level, const Position& pos, std::string&& msg)
{
    fmt::print(stderr, FMT(" {}:{}: {}\n"), levelString(level, color_), pos, msg);

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
    color_                  = true;
}

ErrorManager::~ErrorManager() = default;
