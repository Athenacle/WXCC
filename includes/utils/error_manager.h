#ifndef UTILS_ERROR_MANAGER_H_
#define UTILS_ERROR_MANAGER_H_

#include <fmt/core.h>

#include "lex/lexer.h"

#include "system.h"


namespace utils
{
    class ErrorManager
    {
        using Position = lex::types::Position;

        int err_;
        int warning_;
        int info_;

        static constexpr int FATAL = 0;
        static constexpr int ERROR = 1;
        static constexpr int WARNING = 2;
        static constexpr int INFO = 3;

        void output(int level, const Position&, std::string&&);
        void output(int level, std::string&&);
        void addCounter(int level);


    public:
        ErrorManager();

        template <class... T>
        void error(const Position& pos, const char* fmt, T&&... args)
        {
            output(ERROR, pos, fmt::format(fmt, std::forward<T>(args)...));
        }
        template <class... T>
        void info(const Position& pos, const char* fmt, T&&... args)
        {
            output(INFO, pos, fmt::format(fmt, std::forward<T>(args)...));
        }
        template <class... T>
        void info(const char* fmt, T&&... args)
        {
            output(INFO, fmt::format(fmt, std::forward<T>(args)...));
        }
        template <class... T>
        void fatal(const char* fmt, T&&... args)
        {
            output(FATAL, fmt::format(fmt, std::forward<T>(args)...));
        }
        template <class... T>
        void fatal(const Position& pos, const char* fmt, T&&... args)
        {
            output(FATAL, pos, fmt::format(fmt, std::forward<T>(args)...));
        }

        template <class... T>
        void warning(const Position& pos, const char* fmt, T&&... args)
        {
            output(WARNING, pos, fmt::format(fmt, std::forward<T>(args)...));
        }
    };
}  // namespace utils


#endif
