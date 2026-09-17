/* not including `vendor/cbp` since we are in the repo itself */
#include "src/cbp.cpp"

void build_main(int argc, char *argv[])
{
    /* instead of getting the files from stdin / cmdline,
     * use predefined files */
    char *my_files[] = {
        argv[0], /* support for not doing this will come out in the near future !! */
        (char *)"examples/src.calc.c",
    };

    CBuildP::file_t inf = CBuildP::get_files(2, my_files);

    CBuildP::file_t outf = "build/ex.calc.raylibex";
    CBuildP::optimize({
        .compiler = CBuildP::compilers::c::clang,
        .level = CBuildP::optimization::max,
        .debug = true,
    });
    CBuildP::include({
        "examples"
    });
    CBuildP::compile_flags({
        CBuildP::execcmd("pkg-config --cflags lua5.4")
    });
    CBuildP::compile_link({
        CBuildP::execcmd("pkg-config --libs lua5.4")
    });
    CBuildP::specs(inf, outf);
    CBuildP::link({
        "raylib", "m",
    }, {});
    CBuildP::compile_flags({
        "-pthread"
    });
    CBuildP::compile(inf, outf);
}
