/* lexer input source file.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
*/

#include "../includes/lex.h"
#include "../includes/tools.h"
#include "../includes/symbol.h"

FILE *sourceFile;

char Lex::buffer[LEX_BUFFER_SIZE + 4];
char Lex::tmpBuffer[TMPBUFFER_SIZE];
NS_LEX_PODS::Token *Lex::currentToken = NULL;
NS_LEX_PODS::Position *Lex::currentPos = NULL;

int Lex::fillBuffer(void)
{
	static int isFinish = 0;
	static int readNum = 0;
	int readSize;

	readSize = (int)fread(buffer, sizeof(char), LEX_BUFFER_SIZE, sourceFile);
	readNum += readSize;
	if (readSize < LEX_BUFFER_SIZE && feof(sourceFile))
	{
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
	if (sourceFile == NULL)
	{
		sprintf(tmpBuffer, "open file %s error.", _filename);
		fatalError(tmpBuffer);
	}
	fillBuffer();
	currentToken->token_pos = currentPos;
	currentPos->line = 1;
	currentPos->place = 0;
	currentToken->token_type = NS_LEX_CONSTANTS::T_NONE;
	return 0;
}