#ifndef UTILS_ERROR_MANAGER_H_
#define UTILS_ERROR_MANAGER_H_

#include "system.h"

#include <fmt/core.h>

#include "lex/lexer.h"


namespace utils
{
    class ErrorManager
    {
        using Position = lex::types::Position;

        int err_;
        int warning_;
        int info_;

    public:
        static constexpr int FATAL   = 0;
        static constexpr int ERROR   = 1;
        static constexpr int WARNING = 2;
        static constexpr int INFO    = 3;

    protected:
        virtual void output(int level, const Position&, std::string&&);
        void output(int level, std::string&&);

        void addCounter(int level);


    public:
        int errorCount() const
        {
            return err_;
        }
        int warningCount() const
        {
            return warning_;
        }
        int infoCount() const
        {
            return info_;
        }

        ErrorManager();
        virtual ~ErrorManager();

        template <class... T>
        void error(const Position& pos, T&&... args)
        {
            output(ERROR, pos, fmt::format(std::forward<T>(args)...));
        }
        template <class... T>
        void info(const Position& pos, T&&... args)
        {
            output(INFO, pos, fmt::format(std::forward<T>(args)...));
        }
        template <class... T>
        void info(T&&... args)
        {
            output(INFO, fmt::format(std::forward<T>(args)...));
        }
        template <class... T>
        void fatal(T&&... args)
        {
            output(FATAL, fmt::format(std::forward<T>(args)...));
        }
        template <class... T>
        void fatal(const Position& pos, T&&... args)
        {
            output(FATAL, pos, fmt::format(std::forward<T>(args)...));
        }
        template <class... T>
        void warning(const Position& pos, T&&... args)
        {
            output(WARNING, pos, fmt::format(std::forward<T>(args)...));
        }
    };
}  // namespace utils


#endif
