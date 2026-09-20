#include "libcbp.cpp"
#include <cstring>
#include <filesystem>


void build_main(int argc, char **argv) {
    __offset = 1;

    // Dynamic file getter:
    // - If argc > 2 (subcommand + specific files provided), gather them from argv.
    // - If argc == 2 (just "build"), search current working directory (CWD) for C/C++ source files.
    CBuildP::file_t files = "";

    if (argc > 2) {
        files = CBuildP::get_files(argc, argv);
    } else {
        for (const auto &entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string path_str = entry.path().string();
                // strip leading "./" if present for cleaner output
                if (path_str.rfind("./", 0) == 0) {
                    path_str = path_str.substr(2);
                }
                // CHORE: update this check to be less sensitive
                if (CBuildP::is_file_C_CXX(path_str) && (path_str == "build.cpp" || path_str == "build.cbp.cpp")) {
                    if (files.empty()) {
                        files = path_str;
                    } else {
                        files += " " + path_str;
                    }
                }
            }
        }
    }

    if (files.empty()) {
        std::cout << hex_to_ansi(COLOR_WARNING, &_arn, false).pointer << ":: " << RESET << "[WARNING] : No C/C++ source files found.\n";
        return;
    }

    if (0 != strcmp(argv[1], "build"))
      return;

#ifdef CBP_USE_DEP
    if (argc == 2)
        files.append(" stdstr.o");
#endif

    CBuildP::optimize({
        .compiler = CBuildP::compilers::cpp::clang,
        .level = CBuildP::optimization::level1,
        .debug = true,
    });

    CBuildP::compile(files, "bpp");
}
