#include "precomp.h"

extern "C" int __cdecl main(int argc, char **argv)
{
    CIniFile cif;

    if(argc < 2)
        return 1;

    if(!cif.FReadFile(argv[1], FALSE)) {
        fprintf(stderr, "Couldn't read %s\n", argv[1]);
        return 1;
    }

    if(!cif.FWriteFile(argv[2])) {
        fprintf(stderr, "Couldn't write %s\n", argv[1]);
        return 1;
    }

    return 0;
}
