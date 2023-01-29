
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <functional>
#include <queue>

#include "utils/error_manager.h"

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

class ErrorManagerMock : public V2::utils::ErrorManager
{
    std::queue<std::tuple<int, std::string>> msg;

public:
    virtual ~ErrorManagerMock();

    MOCK_METHOD(void, output, (int, const lex::types::Position&, std::string&&), (override));
};

// using OutputMethod = std::function<void(int, const lex::types::Position&, std::string&&)>;
typedef void OutputMethod(int, const lex::types::Position&, std::string&&);

class ErrorManagerRegexMatcherAction : public ::testing::ActionInterface<OutputMethod>
{
    std::string regex;
    int level;

public:
    void Perform(const std::tuple<int, const lex::types::Position&, std::string&&>& args) override;

    ErrorManagerRegexMatcherAction(int l, const std::string& reg);
};

testing::Action<OutputMethod> ErrorMessageMatcher(int l, const std::string& reg);
