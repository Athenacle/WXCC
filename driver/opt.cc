#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <argparse/argparse.hpp>
#include "driver.h"


bool parseCommandOptions(CommandOptions& opt, int argc, const char* argv[])
{
    argparse::ArgumentParser prog(PROJECT_NAME,
                                  PROJECT_VERSION_STRING
#ifdef PROJECT_GIT_SHA
                                  "-" PROJECT_GIT_SHA
#endif
    );
    prog.add_description("Simple C Compiler");

    prog.add_argument("--lex-debug")
        .help("increase output verbosity")
        .default_value(false)
        .implicit_value(true);

    prog.add_argument("-o", "--output").help("Result file").implicit_value("a.out");


    prog.add_argument("filename").required().remaining().nargs(1);

    try {
        prog.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << prog;
        std::exit(1);
    }

#define IF(para, dest, type)                 \
    do {                                     \
        if (prog.is_used(para)) {            \
            opt.dest = prog.get<type>(para); \
        }                                    \
    } while (false)

    try {
        auto files = prog.get<std::vector<std::string>>("filename");

        opt.filename = files.at(0);

        opt.lexerDebug = prog["--lex-debug"] == true;

        IF("--output", output, std::string);

    } catch (const std::logic_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << prog;
        std::exit(1);
    }

    return true;
}