
#include <cstring>
#include <fstream>

#include "fb_fwd.h"
#include "flex_bison.h"
#include "flex_bison_lexer.h"

using namespace test;
using namespace std;

namespace
{
    NORETURN void usage(const char* bin)
    {
        cerr << "usage: " << bin << " <filename>" << endl
             << "        if <filename> is -, read from stdin(cin)" << endl;
        exit(1);
    }
}  // namespace

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        usage(argv[0]);
    } else {
        if (strcmp(argv[1], "-") == 0) {
            FlexBison_Lexer flex(&cin);
            FlexBison_Parser bison(flex);

            bison.parse();
        } else {
            ifstream ifs(argv[1], ios::binary);

            if (!ifs.is_open()) {
                cerr << "open file " << argv[1] << " failed: " << strerror(errno) << std::endl;
                exit(1);
            }

            FlexBison_Lexer flex(&ifs);
            FlexBison_Parser bison(flex);

            bison.parse();
        }
    }
}
