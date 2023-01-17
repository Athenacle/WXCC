#include <random>

#include "test.h"
#include "tools.h"

namespace
{
    std::random_device rd;
}

namespace test
{
    const char* randomString(uint32_t length)
    {
        char* mem = new char[length + 1];
        mem[length] = 0;

        for (uint32_t i = 0; i < length; i++) {
            char c = '\n';
            for (; c == '\n';)
                c = (rd() % ('~' - '!')) + '!';
            mem[i] = c;
        }

        return mem;
    }

    uint32_t random()
    {
        return rd();
    }

    uint32_t random(uint32_t mod)
    {
        return test::random() % mod;
    }

    void split(std::vector<std::string>& out, const std::string& in, const std::string& sep)
    {
        std::string tmp(in);
        do {
            auto pos = tmp.find_first_of(sep);
            if (pos == std::string::npos) {
                if (tmp.length() > 0) {
                    out.emplace_back(tmp);
                }
                break;
            } else {
                std::string sub(tmp, 0, pos);
                tmp = tmp.substr(pos + sep.length());
                out.emplace_back(sub);
            }

        } while (true);
    }
}  // namespace test

#ifdef HAVE_FLEX_BISON
#include <fmt/core.h>

namespace
{
    const char* randomKeyword()
    {
        static std::vector<const char*> keys = {
            "break",  "case",     "char",   "const",    "continue", "default",  "do",     "double",
            "else",   "enum",     "extern", "float",    "for",      "goto",     "if",     "int",
            "long",   "register", "return", "short",    "signed",   "sizeof",   "static", "struct",
            "switch", "typedef",  "union",  "unsigned", "void",     "volatile", "while"};
        auto r = test::random(keys.size());
        return keys[r];
    }
}  // namespace

namespace test
{
    const char* randomSourceCode(uint32_t& count)
    {
        char buffer[64];
        uint32_t total;

        auto randID = [&]() {
            auto leng = (rd() % 60) + 1;

            for (uint32_t i = 0; i < leng; i++) {
                char c = '\n';
                for (; c == '\n';)
                    c = (rd() % ('z' - 'a')) + 'a';
                buffer[i] = c;
            }
            buffer[leng] = 0;
        };

        std::string out;
        out.reserve(30 * count);

        for (uint32_t i = 0; i < count; i++) {
            auto r = test::random(100);
            if (r < 30) {
                total++;
                out = out.append(randomKeyword());
            } else if (r < 60) {
                // number;
                auto rn = test::random();
                total++;
                if (auto rm = rn % 100; rm < 60) {
                    // int
                    if (rm < 50) {
                        // dec
                        out = out.append(fmt::format("{}", rn >> 8));
                    } else if (rm < 60) {
                        // oct
                        out = out.append(fmt::format("{0:#o}", rn >> 8));
                    } else if (rm < 70) {
                        // hex
                        out = out.append(fmt::format("{0:#x}", rn >> 8));
                    }
                }
            } else if (r < 90) {
                // id
                randID();
                out = out.append(buffer);
                total++;
            } else if (r < 95) {
                // str
                randID();
                out.append("\"");
                out.append(buffer);
                out.append("\"");
            } else {
            }
            if (r < 40) {
                out.append(" ");
            } else if (r < 50) {
                out.append("\t");
            } else {
                out.append("\n");
            }
        }
        count = total;

        return utils::strdup(out.c_str());
    }
}  // namespace test

#endif