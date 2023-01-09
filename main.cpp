/* main function
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
*/

#include "system.h"
#include "lex.h"
#include "tools.h"
#include "parser.h"
#include "symbol.h"
#include "type.h"

int errorCount = 0;
int warningCount = 0;
FILE *wxccErr;

void HandleArg(int argc, char *argv[])
{
    wxccErr = NULL;
    if (argc == 1)
        fatalError("no input file.");
    else if (argc == 2) {
        char *outDest = argv[1];
        if (*outDest == '-' && *(outDest + 1) == 'o') {
            wxccErr = fopen(outDest + 2, "w+");
        }
    }
}

void initCompiler(void)
{
    Type::initTO2c();
    NS_BASE_TYPE::InitBaseTypes();
}


int main(int argc, char *argv[])
{
    HandleArg(argc, argv);
    if (wxccErr == NULL)
        wxccErr = stdout;
    initCompiler();
    Lex lexer(argv[1]);
    Parser par(lexer);
    Env *globalEnv = new Env();
    globalEnv->setGlobal();
    par.begin_parse(*globalEnv);
    return 0xdeadbeef;
}
