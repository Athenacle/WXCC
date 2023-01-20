#ifndef LEX_INPUT_H_
#define LEX_INPUT_H_

#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>

#include "lex/token.h"

namespace lex
{
    class LexInputSource
    {
        using Position = types::Position;

    protected:
        int line;
        int column;
        std::shared_ptr<std::string> file;

    public:
        LexInputSource();
        virtual ~LexInputSource();

        enum class GET_TYPE { GET_CHAR, PUSH_BACK, LOOK_AHEAD };

        virtual char next(GET_TYPE) = 0;

        virtual bool fillBuffer()
        {
            return true;
        }

        virtual const char *filename() = 0;

        int getColum() const
        {
            return column;
        }

        int getLineno() const
        {
            return line;
        }

        Position position() const;
    };

    class MemoryLexInputSource : public LexInputSource
    {
        char *buf;
        char *end;
        char *pointer;
        uint32_t bufsize;
        int lastCol;

    public:
        MemoryLexInputSource();

        virtual ~MemoryLexInputSource();

        virtual char next(GET_TYPE) override;

        void fill(const char *, size_t);

        virtual const char *filename() override;
    };

    class FileLexInputSource : public MemoryLexInputSource
    {
        FILE *fp;

    public:
        FileLexInputSource();

        virtual ~FileLexInputSource();

        bool openFile(const char *);

        bool openStdin();

        virtual char next(GET_TYPE) override;

        virtual bool fillBuffer() override;

        virtual const char *filename() override;
    };

}  // namespace lex

#endif
