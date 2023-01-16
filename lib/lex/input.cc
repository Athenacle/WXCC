/* lexer input source file.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
*/

#include "lex/input.h"

#include <fmt/core.h>

#include "lex/lexer.h"

#include "symbol.h"
#include "tools.h"

using namespace lex;


LexInputSource::~LexInputSource() {}

MemoryLexInputSource::~MemoryLexInputSource()
{
    if (buf != nullptr) {
        delete[] buf;
    }
    buf = end = pointer = nullptr;
}

MemoryLexInputSource::MemoryLexInputSource()
{
    bufsize = 0;
    buf = end = pointer = nullptr;
}


FileLexInputSource::~FileLexInputSource()
{
    if (fp) {
        fclose(fp);
    }
    if (file) {
        delete[] file;
    }
}

FileLexInputSource::FileLexInputSource() : fp(nullptr) {}

const char *MemoryLexInputSource::filename()
{
    return "memory";
}

const char *FileLexInputSource::filename()
{
    return file;
}

bool FileLexInputSource::fillBuffer()
{
    const uint32_t BUFFSIZE = 4096;
    char buf[BUFFSIZE];
    auto r = fread(buf, 1, BUFFSIZE, fp);
    fill(buf, r);
    return r > 0;
}

bool FileLexInputSource::openFile(const char *fn)
{
    fp = fopen(fn, "rb");
    if (fp == nullptr) {
        auto msg = fmt::format("open file {} failed: {}", fn, strerror(errno));
        fatalError(msg.c_str());
    }
    fseek(fp, 0, SEEK_SET);
    file = utils::strdup(fn);
    return true;
}

char MemoryLexInputSource::next(LexInputSource::GET_TYPE type)
{
    const char nul = '\0';

    if (unlikely(buf == nullptr)) {
        return nul;
    }
    auto c = *(pointer);

    if (c && type == LexInputSource::GET_TYPE::GET_CHAR) {
        pointer += 1;
    }
    if (type == LexInputSource::GET_TYPE::PUSH_BACK && pointer > buf) {
        pointer -= 1;
    }

    return c;
}

char FileLexInputSource::next(LexInputSource::GET_TYPE type)
{
    auto ret = MemoryLexInputSource::next(type);
    if (ret == '\0' && !feof(fp)) {
        fillBuffer();
        return MemoryLexInputSource::next(type);
    } else {
        return ret;
    }
}

void MemoryLexInputSource::fill(const char *in, size_t s)
{
    uint32_t nbs = 0;
    if (unlikely(buf == nullptr)) {
        buf = pointer = new char[s + 1];
        memcpy(pointer, in, s);
        nbs = s;
    } else if (unlikely(pointer == buf)) {
        auto nb = new char[bufsize + s + 1];
        nbs = bufsize + s;
        memcpy(nb, buf, bufsize);
        memcpy(nb + bufsize, in, s);
        delete[] buf;
        buf = pointer = nb;
        bufsize = bufsize + s;
        end = buf + bufsize;

    } else {
        auto c = *pointer;

        auto saved = end - pointer;
        nbs = saved + s;
        auto nb = new char[nbs + 1];
        memcpy(nb, pointer, saved);
        memcpy(nb + saved, in, s);

        assert(memcmp(pointer, nb, saved) == 0);
        assert(memcmp(in, nb + saved, s) == 0);

        pointer = nb;

        delete[] buf;
        buf = nb;
        buf[nbs] = 0;
        if (c) {
            assert(c == *pointer);
        }
    }

    buf[nbs] = '\0';
    end = pointer + nbs;
    bufsize = nbs;
}