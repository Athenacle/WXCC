#ifndef LEX_OUTPUT_H_
#define LEX_OUTPUT_H_

#include "lex/lexer.h"

namespace lex
{
    class LexOutput
    {
        Lex& lexer;
        FILE* fp;

    public:
        LexOutput(Lex& lexer, FILE* file);
        int output(void);
    };
}  // namespace lex

#endif