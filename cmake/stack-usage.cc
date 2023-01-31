#include "config.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <sys/wait.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifndef CPP_FILT
#error CPP_FILT Marco does not exists.
#endif

#ifndef ALL_SU
#error ALL_SU Macro does not exists.
#endif

using namespace std;


vector<string> readSomeFile(const string& f)
{
    struct stat st;
    int fd = open(f.c_str(), O_RDONLY);
    int ac = fstat(fd, &st);
    vector<string> ret;
    if (ac == 0) {
        auto ptr        = new char[st.st_size + 1];
        ptr[st.st_size] = '\0';

        if (auto r = read(fd, ptr, st.st_size); r == st.st_size) {
            istringstream iss(ptr);
            for (string line; getline(iss, line);) {
                ret.emplace_back(std::move(line));
            }
        }
        delete[] ptr;
    }
    close(fd);
    return ret;
}

bool ends_with(std::string const& value, std::string const& ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

struct Function {
    string file;
    string rawName;
    string deName;
    string type;

    static size_t longest;

    int line;
    int size;

    Function() = default;

    Function(const string& f, int l, const string& rn, int s, const string& ty)
    {
        if (f.find_first_of(SOURCE_DIR) == 0) {
            file = f.substr(sizeof(SOURCE_DIR));
        } else {
            file = f;
        }

        line    = l;
        rawName = rn;
        size    = s;
        type    = ty;

        if (longest < file.length()) {
            longest = file.length();
        }
    }
};

size_t Function::longest = 0;

bool checkPrint(Function& f)
{
    static auto first  = [](const string& in) { return in.find_first_of("fmt::") == 0; };
    static auto second = [](const string& in) { return in.find_first_of("std::") == 0; };
    static auto third  = [](const string& in) { return in.find_first_of("__gnu_cxx") == 0; };

    static vector<std::function<bool(const string&)>> funcs = {first, second, third};

    for (const auto& func : funcs) {
        if (func(f.deName)) {
            return true;
        }
    }
    return false;
}

void split(std::vector<std::string>& out, const std::string& in, const std::string& sep)
{
    std::string tmp(in);
    do {
        auto pos = tmp.find_first_of(sep);
        if (pos == std::string::npos) {
            if (tmp.length() > 0) {
                out.emplace_back(tmp);
            }
            break;
        } else {
            std::string sub(tmp, 0, pos);
            tmp = tmp.substr(pos + sep.length());
            if (sub.length() > 0) {
                out.emplace_back(sub);
            }
        }

    } while (true);
}
vector<Function> processing(vector<string>& lines)
{
    // /home/wangxiao/gits/wxcc/lib/lex/input.cc:2320:_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE5c_strEv   24  static
    // regex re("^((/.*)+):(\\d+):([\\d\\w_]+)\\s+(\\d+)\\s+(\\w+)$");
    vector<Function> fs;

    for (auto& l : lines) {
        vector<string> cols;
        // /home/wangxiao/gits/wxcc/lib/lex/input.cc:29:1:virtual lex::MemoryLexInputSource::~MemoryLexInputSource()   32  static
        const char *col1, *col2, *col3;
        col1 = col2 = l.c_str();
        while (*col1 && *col1 != ':') {
            col1++;
        }
        for (col2 = col1 + 1; *col2 && *col2 != ':'; col2++) {
        }
        for (col3 = col2 + 1; *col3 && *col3 != ':'; col3++) {
        }
        if (!*col3) {
            col3 = col2;
        }
        auto file = string(l.c_str(), col1);
        auto line = stoi(string(col1 + 1, col2));
        auto last = string(col3 + 1);

        cols.clear();
        split(cols, last, "\t");
        if (cols.size() != 3) {
            assert(false);
        }
        auto name = cols[0];
        auto size = stoi(cols[1]);
        auto type = cols[2];
        Function f(file, line, name, size, type);
        /*
        cerr << file << " - " << line << " - " << name << " - " << size << " - " << type << endl
             << endl
             << endl;
        */

        fs.emplace_back(std::move(f));
    }

    /*
    int i = 0;
    for (auto& line : lines) {
        std::smatch math;
        if (regex_search(line, math, re)) {
            if (math.size() == 7) {
                Function c(math[2].str(),
                           stoi(math[3].str()),
                           math[4].str(),
                           stoi(math[5].str()),
                           math[6].str());
                if (checkPrint(c)) {
                    fs.emplace_back(std::move(c));
                }
            }
        }
    }
    */
    return fs;
}

void demangle(vector<Function>& fs)
{
    if (fs.empty()) {
        cerr << "none function stack usage data\n";
        exit(4);
    }
    int pip_in[2];
    int pip_out[2];

    int ret  = pipe(pip_in);
    int ret1 = pipe(pip_out);

    if (ret < 0 || ret1 < 0) {
        cerr << "pipe failed: " << strerror(errno) << endl;
        exit(2);
    }

    pid_t pid = fork();
    if (pid < 0) {
        cerr << "fork failed: " << strerror(errno) << endl;
        exit(3);
    }

    if (pid == 0) {
        // child
        close(STDIN_FILENO);
        close(STDOUT_FILENO);

        dup2(pip_out[0], STDIN_FILENO);
        close(pip_out[1]);
        dup2(pip_in[1], STDOUT_FILENO);
        close(pip_in[0]);
        execl(CPP_FILT, CPP_FILT, nullptr);
    } else {
        // parent
        close(pip_out[0]);
        close(pip_in[1]);

#define BUFS 2056

        auto buf = new char[BUFS];
        for (auto& f : fs) {
            f.rawName += '\n';
            auto w    = write(pip_out[1], f.rawName.c_str(), f.rawName.size());
            if (!(w > 0 && static_cast<size_t>(w) == f.rawName.size())) {
                cerr << "write " << f.rawName.substr(0, f.rawName.size() - 1)
                     << " failed: " << strerror(errno) << endl;
                exit(4);
            }

            auto r = read(pip_in[0], buf, BUFS);

            if (r > 0 && r < BUFS) {
                string in(buf, r);
                f.deName = in.substr(0, in.size() - 1);
            } else {
                cerr << "error!" << endl;
            }
        }
        close(pip_in[0]);
        close(pip_out[1]);
        waitpid(pid, nullptr, WUNTRACED);
        delete[] buf;
    }
}


int main(int argc, const char* argv[])
{
    vector<string> all;

    auto lines = readSomeFile(ALL_SU);
    for (auto& l : lines) {
        if (ends_with(l, ".su")) {
            auto ll = readSomeFile(l);
            for (auto& li : ll) {
                if (!ends_with(li, ".su")) {
                    all.emplace_back(li);
                } else {
                    auto lll = readSomeFile(li);
                    for (auto& llli : lll) {
                        if (!ends_with(llli, ".su")) {
                            all.emplace_back(llli);
                        }
                    }
                }
            }
        } else {
            all.emplace_back(l);
        }
    }

    auto fs = processing(all);
    demangle(fs);

    sort(fs.begin(), fs.end(), [](const auto& first, const auto& second) -> bool {
        return first.size < second.size;
    });


    for (auto& f : fs) {
        if (checkPrint(f)) {
            cout << std::setw(6) << left << f.size << setw(3) << left
                 << (f.type == "static" ? "S" : "?") << setw(Function::longest + 5) << left
                 << f.file << f.deName << endl;
        }
    }
}
