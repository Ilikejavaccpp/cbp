#include "src/cbp.cpp"

void build_main(int argc, char **argv) {
    CBuildP::file_t files = CBuildP::add_files({ "backend/binit/build.init.cpp"});
    CBuildP::optimize({
        .compiler = CBuildP::compilers::cxx::gcc,
        .level = CBuildP::optimization::level1,
        .debug = true,
    });
    CBuildP::file_t out_file = "bpp";
    CBuildP::compile(files, out_file);
}
