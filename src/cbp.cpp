#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <ctime>
#include <string>
#include <vector>

#define MINIMAL
#include "../backend/calias.h"
#include "../backend/cstr.h"
#include "../backend/colors.c"
#include "../backend/cstr.c"

// colors, you may/maynot modify this
#define COLOR_INFO    "#9c1bef" /* pacman / zig info purple */
#define COLOR_SUCCESS "#2ecc71" /* emerald green for success/done */
#define COLOR_WARNING "#f1c40f" /* amber yellow for warnings */
#define COLOR_ERROR   "#e74c3c" /* vibrant red for errors */
#define COLOR_MUTED   "#7f8c8d" /* cool grey for metadata/stats */
#define RESET         "\033[0m" /* reset color */

// necessary non mangled stuff, you aren't allowed to modify
// the art of be safe: encapsulation -- OOP
int _code = 0;
str_t _arn;
extern "C" { void build_main(int argc, char *argv[]); void conf_build_cmd(int argc, char *argv[]) { std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Building with command\n" << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer; for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; } std::cout << RESET << std::endl;   }; void conf_build_end() { std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << (_code == 0 ? RESET : hex_to_ansi(COLOR_ERROR, &_arn,false).pointer) << "Build finished" << RESET << std::endl; /* immediately flush */ } }
namespace  CBuildP {
    struct opt_t {
        const char  * compiler="clang";
        int level = 0; /* no optimization, like in Zed */
        bool debug = true; /* add debug symbols */

        static void prints(const opt_t &opt);
    };
    void opt_t::prints(const opt_t &opt) {
        std::cout << "   - opt level: " << opt.level << "\n";
        std::cout << "   - debug: " << opt.debug << std::endl;
    }
    opt_t _stdopt = {};
}
namespace CBuildP {
    using file_t = std::string;
    namespace libtype {
        /* encrypt using jumbled keyboard mess */
        const char* STATIC = "ASFUOASOF@#&HDlib.a#QBDKASB";
        const char* SHARED = "NAIUSDFNUA!#*&^!#lib.soH&*#FHB";
    }

    file_t _link = "";
    file_t _flags = "";

    inline std::string execcmd(const std::string &cmd) {
        std::string result = "";
        FILE *pipe = popen(cmd.c_str(), "r");
        if (!pipe) return result;
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
        if (!result.empty() && result.back() == '\n') result.pop_back();
        return result;
    }

    bool is_file_C_CXX(const std::string &filename) {
        return  filename.find(".c") != std::string::npos ||
                filename.find(".cxx") != std::string::npos ||
                filename.find(".cpp") != std::string::npos;
    }

    void check_file(const std::string &filename) {
        std::cout << "[ERROR] : Non C/C++ file included, are you sure?\n"
                << "        Included src file is non C/C++: " << filename <<
                std::endl;
        _code = 1;
    }

    std::string get_files(int argc, char *argv[]) {
        if (argc < 2) {
            return "";
        }

        if (!is_file_C_CXX(std::string(argv[1]))) {
            std::cout << "[ERROR] : Non C/C++ file included, are you sure?\n"
                    << "        First file/arg is non C/C++: " << argv[0] << " " << argv[1] <<
                std::endl;
            _code = 1;
            return "";
        }

        std::string result = std::string(argv[1]);
        for (int i = 2; i < argc; ++i) {
            if (is_file_C_CXX(std::string(argv[i])))
                result.append(" " + std::string(argv[i]));
            else {
                std::cout << "[ERROR] : Non C/C++ file included, are you sure?" << std::endl;
                _code = 1;
                return "";
            }
        }
        return result;
    }

    void specs(const file_t &file_names, const file_t &out_file) {
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Build info:\n";
        std::cout << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "   - C/C++ files : " << RESET << file_names << "\n";
        std::cout << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "   - output file : " << RESET << out_file << std::endl;
        std::cout << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "   - optimization: " << RESET << _stdopt.level << std::endl;
        std::cout << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "   - debug       : " << RESET << _stdopt.debug << std::endl;
        std::cout << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "   - compiler    : " << RESET << _stdopt.compiler << std::endl;
    }

    void optimize(const opt_t &opt) {
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Optimization stats... \n";
        opt.prints(opt);
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Optimizing the standard";

        const char *loading_state[] = {
            // "⢿", "⣻", "⣽"
            ".  ", ".. ", "..."
        };

        for (int i = 0; i < 12; ++i) {
            std::cout << "\r" << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Optimizing the standard " << loading_state[i % 3] << "\033[K" << std::flush;
            clock_t start_time = clock();
            while (clock() < start_time + (CLOCKS_PER_SEC / 4));
        }

        _stdopt = opt;

        std::cout << "\n" << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << ":: " << RESET << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << "DONE" << std::endl;
    }

    void link(const std::vector<file_t>& libraries, const std::vector<file_t> &look_for_dirs) {
        file_t _stor = "";
        file_t _dirl = "";

        if (libraries.empty()) return;

        // look for the libraries in the dirs
        for (int i = 0; i < look_for_dirs.size(); ++i) {
            _dirl += "-L" + look_for_dirs[i] + " ";
            // if (i < look_for_dirs.size() - 1) _dirl += " ";
        }

        if (!look_for_dirs.empty())
            _link += _dirl;

        // iterate over the libraries and append them.
        for (int i = 0; i < libraries.size(); ++i) {
            _link += "-l" + libraries[i] + " ";
            _stor += libraries[i] + (i < libraries.size() - 1 ? " " : "");
        }

        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Linking libraries: " << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << _stor << RESET << std::endl;
    }

    void include(const std::vector<file_t> &dirs) {
        for (size_t i = 0; i < dirs.size(); ++i)
            _flags += "-I" + dirs[i] + " ";
    }

    // compile the C/CXX flags like `-pthread`, `-Iinclude`, etc.
    // this is dangerous...
    void compile_flags(const std::vector<file_t> &flags) {
        for (size_t i = 0; i < flags.size(); ++i) {
            if (!flags[i].empty() && (flags[i][1] == 'p' || flags[i][1] == 'I'))
                _flags += flags[i] + " ";
        }
    }

    void compile_link(const std::vector<std::string> &link_cmd_comp) {
        for (size_t i = 0; i < link_cmd_comp.size(); ++i) {
            _link += link_cmd_comp[i] + " ";
        }
    }

    void compile_lib(const std::vector<file_t> &file_names, const file_t &libname,
                    const file_t &dir = "cbp.build/lib",
                    const std::vector<file_t> &libtypes = {},
                    const bool &clean = true)
    {
        if (file_names.empty() || libname.empty()) return;

        /* safety: ensure that the directory exists */
        std::string __ensure_dir__mkdir_c = "mkdir -p " + dir;
        system(__ensure_dir__mkdir_c.c_str());

        /* stats */
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Compiling library: " << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer;
        for (size_t i = 0; i < file_names.size(); ++i)
            std::cout << file_names[i] << (i + 1 < file_names.size() ? ", " : "");
        std::cout << RESET << std::endl;

        /* first compile the object files */
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Compiling object files" << RESET << std::endl;

        /* compile the object files */
        bool _succ = true;
        for (size_t i = 0; i < file_names.size(); ++i) {
            std::string command = _stdopt.compiler + std::string(" -O") + std::to_string(_stdopt.level) + std::string(" -c ") +
                                file_names[i] + " -o " +
                                dir + "/" + file_names[i].substr(0, file_names[i].find_last_of('.')) + ".o";

            int result = system(command.c_str());
            if (result != 0) {
                std::cout << hex_to_ansi(COLOR_ERROR, &_arn, false).pointer << ":: " << RESET << "[ERROR] : Library Compilation " << RESET << "failed" << std::endl;
                _code = 1;
                _succ = false;
            } else {
                std::cout << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << ":: " << RESET << "Library Compilation " << RESET << "succeeded" << std::endl;
            }
        }

        /* if compilation failed, exit early */
        if (!_succ) return; // this is a bit of bloat since you may just slap that at the
                            // result check in the for loop iteration

        /* link the object files */
        std::string ar_command = "ar rcs " + dir + "/lib" + libname + ".a";
        for (size_t i = 0; i < file_names.size(); ++i) {
            ar_command += " " + dir + "/" + file_names[i].substr(0, file_names[i].find_last_of('.')) + ".o";
        }
        int result = system(ar_command.c_str());
        if (result != 0) {
            std::cout << hex_to_ansi(COLOR_ERROR, &_arn, false).pointer << ":: " << RESET << "[ERROR] : Library Linking " << RESET << "failed" << std::endl;
            _code = 1;
        } else {
            std::cout << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << ":: " << RESET << "Library Linking " << RESET << "succeeded" << std::endl;
            std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Library " << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "lib" << libname << ".a" << RESET << " created" << std::endl;
            std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << "DONE" << RESET << std::endl;
        }

        /* clean: optional */
        if (clean) {
            for (size_t i = 0; i < file_names.size(); ++i) {
                std::string obj_file = dir + "/" + file_names[i].substr(0, file_names[i].find_last_of('.')) + ".o"; // get the object file path
                std::string rm_cmd = "rm -f " + obj_file; // command
                system(rm_cmd.c_str()); // new shell with the rm command executed.
            }
        }
    }

    void compile(const file_t &file_names, const file_t & outfile) {
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Compiling (with linkage): " << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << file_names << RESET << std::endl;
        std::string command = std::string(_stdopt.compiler) + " " + _flags + " " + file_names + " " + _link +  " -o " + outfile;
        int result = system(command.c_str());
        if (result != 0) {
            std::cout << hex_to_ansi(COLOR_ERROR, &_arn, false).pointer << ":: " << RESET << "[ERROR] : Compilation " << RESET << "failed" << std::endl;
            _code = 1;
        } else {
            std::cout << "\n" << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << "Compilation successful\n";
        }
    }

    namespace compilers {
        namespace c {
            const char* clang = "clang";
            const char* gcc = "gcc";
        }
        namespace cpp {
            const char* clang = "clang++";
            const char* gcc = "g++";
        }
        namespace cxx {
            const char* clang = "clang++";
            const char* gcc = "g++";
        }
        std::vector<const char*> others = {};
    }

    namespace optimization {
        u8 none = 0;
        u8 level1 = 1;
        u8 level2 = 2;
        u8 level3 = 3;

        /* nice aliases */
        u8 max = 3;
        u8 fast = 1;
    }

}
int main(int argc, char *argv[]) { str_initArena(&_arn, NULL); conf_build_cmd(argc, argv); build_main(argc, argv); std::cout << hex_to_ansi(_code == 0 ? COLOR_SUCCESS : COLOR_ERROR, &_arn, false).pointer << " CODE " << _code << RESET <<  std::endl; conf_build_end(); return _code; }
