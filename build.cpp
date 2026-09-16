/* not including `vendor/cbp` since we are in the repo itself */
#include "src/cbp.cpp"

void build_main(int argc, char *argv[])
{
    /* instead of getting the files from stdin / cmdline,
     * use predefined files */
    char *my_files[] = {
        argv[0], /* support for not doing this will come out in the near future !! */
        (char *)"examples/src.main.c",
    };

    CBuildP::file_t inf = CBuildP::get_files(2, my_files);

    CBuildP::file_t outf = "build/ex.main.raylibex";
    CBuildP::optimize({
        .compiler = CBuildP::compilers::c::clang,
        .level = CBuildP::optimization::max,
        .debug = true,
    });
    CBuildP::include({
        "examples"
    });
    CBuildP::specs(inf, outf);
    CBuildP::link({
        "raylib"
    }, {});
    CBuildP::compile(inf, outf);
}
