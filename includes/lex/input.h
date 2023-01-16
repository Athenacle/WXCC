#ifndef LEX_INPUT_H_
#define LEX_INPUT_H_

#include <cstdint>
#include <cstdio>

namespace lex
{
    class LexInputSource
    {
    public:
        LexInputSource() = default;
        virtual ~LexInputSource();

        enum class GET_TYPE { GET_CHAR, PUSH_BACK, LOOK_AHEAD };

        virtual char next(GET_TYPE) = 0;

        virtual bool fillBuffer()
        {
            return true;
        }

        virtual const char *filename() = 0;
    };

    class MemoryLexInputSource : public LexInputSource
    {
        char *buf;
        char *end;
        char *pointer;
        uint32_t bufsize;

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
        const char *file;


    public:
        FileLexInputSource();

        virtual ~FileLexInputSource();

        bool openFile(const char *);

        virtual char next(GET_TYPE) override;

        virtual bool fillBuffer() override;

        virtual const char *filename() override;
    };

}  // namespace lex

#endif