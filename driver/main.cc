#include "lex/output.h"
#include "parser/parser.h"

#include "driver.h"

USING_V2

using namespace lex;
using namespace utils;
using namespace parser;
using namespace syntax;

int main(int argc, const char* argv[])
{
    CommandOptions opt;
    parseCommandOptions(opt, argc, argv);

    FileLexInputSource input;
    if (opt.filename == "-") {
        input.openStdin();
    } else {
        input.openFile(opt.filename.c_str());
    }

    ErrorManager mgr;
    Lex lexer(&input);

    if (opt.lexerDebug) {
        auto fp = openOutput(opt, mgr);
        LexOutput out(lexer, fp);
        out.output();
        fclose(fp);
    } else {
        TranslationUnit unit;
        Parser par(lexer, mgr);
        par.parse(unit);
    }
}
