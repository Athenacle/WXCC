
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>

namespace test
{
    const char* randomString(uint32_t length);

    uint32_t random();

    uint32_t random(uint32_t mod);

    void split(std::vector<std::string>& out, const std::string& in, const std::string& sep);

    bool openFile(std::ifstream&, const std::string&);

    bool openFile(std::ofstream&, const std::string&);

#ifdef HAVE_FLEX_BISON
    const char* randomSourceCode(uint32_t&);
#endif
}  // namespace test
