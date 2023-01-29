#include <random>
#include <regex>

#include "utils/utils.h"

#include "test.h"

USING_V2
namespace
{
    std::random_device rd;
}

namespace test
{
    bool openFile(std::ifstream& ifs, const std::string& file)
    {
        std::ifstream istrm(file, std::ios::binary);
        ifs = std::move(istrm);
        return ifs.is_open();
    }

    const char* randomString(uint32_t length)
    {
        char* mem   = new char[length + 1];
        mem[length] = 0;

        for (uint32_t i = 0; i < length; i++) {
            char c = '\n';
            for (; c == '\n';) {
                c = (rd() % ('~' - '!')) + '!';
            }
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
        auto r = test::random(static_cast<uint32_t>(keys.size()));
        return keys[r];
    }
}  // namespace

namespace test
{

    const char* randomSourceCode(uint32_t& count)
    {
        char buffer[64];
        uint32_t total = 0;

        auto randID    = [&]() {
            auto leng = (rd() % 60) + 1;

            for (uint32_t i = 0; i < leng; i++) {
                char c = '\n';
                for (; c == '\n';) {
                    c = (rd() % ('z' - 'a')) + 'a';
                }
                buffer[i] = c;
            }
            buffer[leng] = 0;
        };

        std::string out;
        out.reserve(30U * count);

        for (uint32_t i = 0; i < count; i++) {
            auto r = test::random(100);
            if (r < 30) {
                out = out.append(randomKeyword());
            } else if (r < 60) {
                // number;
                auto rn = test::random();
                if (auto rm = rn % 100; rm < 60) {
                    // int
                    if (rm < 50) {
                        // dec
                        out = out.append(fmt::format(FMT("{}"), rn >> 8));
                    } else if (rm < 60) {
                        // oct
                        out = out.append(fmt::format(FMT("{0:#o}"), rn >> 8));
                    } else if (rm < 70) {
                        // hex
                        out = out.append(fmt::format(FMT("{0:#x}"), rn >> 8));
                    }
                }
            } else if (r < 90) {
                // id
                randID();
                out = out.append(buffer);
            } else if (r < 95) {
                // str
                randID();
                out.append("\"");
                out.append(buffer);
                out.append("\"");
            } else {
            }

            if (r < 95) {
                total++;
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

        return V2::utils::strdup(out.c_str());
    }
}  // namespace test

#endif


ErrorManagerMock::~ErrorManagerMock() = default;

void ErrorManagerRegexMatcherAction::Perform(
    const std::tuple<int, const lex::types::Position&, std::string&&>& args)
{
    auto level = std::get<0>(args);
    auto msg   = std::get<2>(args);


    EXPECT_EQ(this->level, level);

    if (regex.length() > 0) {
        std::regex r(regex);
        std::smatch match;
        ASSERT_TRUE(std::regex_search(msg, match, r));
        ASSERT_EQ(match.position(0), 0);
    }
}


testing::Action<OutputMethod> ErrorMessageMatcher(int l, const std::string& reg)
{
    return ::testing::MakeAction(new ErrorManagerRegexMatcherAction(l, reg));
}

ErrorManagerRegexMatcherAction::ErrorManagerRegexMatcherAction(int l, const std::string& reg)
{
    level = l;
    regex = reg;
}
