/* lexer input source file.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
*/

#include "lex/lexer.h"
#include "tools.h"
#include "symbol.h"

FILE *sourceFile;
using namespace lex;

char lex::Lex::buffer[LEX_BUFFER_SIZE + 4];
char lex::Lex::tmpBuffer[TMPBUFFER_SIZE];
lex::types::Token *Lex::currentToken = nullptr;
lex::types::Position *Lex::currentPos = nullptr;

int Lex::fillBuffer(void)
{
    static int isFinish = 0;
    static int readNum = 0;
    int readSize;

    readSize = (int)fread(buffer, sizeof(char), LEX_BUFFER_SIZE, sourceFile);
    readNum += readSize;
    if (readSize < LEX_BUFFER_SIZE && feof(sourceFile)) {
        isFinish = 1;
        /* all context has been read into buffer that means
		the source file is smaller
		* than 4kb
		*/
        buffer[readSize] = '\0';
        buffer[readSize + 1] = EOF;
    }
    return isFinish;
}

int Lex::initLex(const char *_filename)
{
    char *name = new char[(strlen(_filename) + 1)];
    sourceFile = fopen(_filename, "r");

    strcpy(name, _filename);
    /*
	* in order to re-syn line number using #line token.
	* try to avoid memory leak.
	*/
    currentPos->filename = name;
    if (sourceFile == nullptr) {
        sprintf(tmpBuffer, "open file %s error.", _filename);
        fatalError(tmpBuffer);
    }
    fillBuffer();
    currentToken->token_pos = currentPos;
    currentPos->line = 1;
    currentPos->place = 0;
    currentToken->token_type = constants::T_NONE;
    return 0;
}