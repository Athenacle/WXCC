#ifndef LEX_PRINT_H_
#define LEX_PRINT_H_

#include "system.h"

#include <fmt/core.h>

#include <unordered_map>

#include "lex/constants.h"
#include "lex/token.h"

namespace lex
{
    namespace print
    {
        extern std::unordered_map<constants::KEYWORD, std::string> keywordToStringMap;
        extern std::unordered_map<constants::OP, std::string> operatorToStringMap;

        void buildKeymap();
        void buildOperatorMap();
    }  // namespace print
}  // namespace lex

namespace fmt
{
    template <>
    struct formatter<lex::constants::OP> {
        constexpr auto parse(fmt::format_parse_context &ctx)
        {
            return ctx.begin();
        }


        template <typename FormatContext>
        auto format(const lex::constants::OP &op, FormatContext &ctx) const
        {
            auto &m = ::lex::print::operatorToStringMap;
            if (unlikely(m.size() == 0)) {
                lex::print::buildOperatorMap();
            }

            return fmt::format_to(ctx.out(), m[op]);
        }
    };

    template <>
    struct formatter<lex::constants::KEYWORD> {
        constexpr auto parse(fmt::format_parse_context &ctx)
        {
            return ctx.begin();
        }


        template <typename FormatContext>
        auto format(const lex::constants::KEYWORD &k, FormatContext &ctx) const
        {
            auto &m = ::lex::print::keywordToStringMap;
            if (unlikely(m.size() == 0)) {
                lex::print::buildKeymap();
            }

            return fmt::format_to(ctx.out(), m[k]);
        }
    };

}  // namespace fmt

#endif
