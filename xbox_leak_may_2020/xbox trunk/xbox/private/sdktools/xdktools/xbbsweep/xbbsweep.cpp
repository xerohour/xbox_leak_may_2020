#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <xboxdbg.h>

//
// Name of the IDF data file on the xbox
//
#define XBOX_IRTSWEEP_FILENAME "E:\\irtsweep.dat"

CHAR* programname;

//
// Error message spews
//
VOID usage()
{
    fprintf(stderr, "usage: %s [-x xboxname] IDF-filename\n", programname);
    exit(1);
}

VOID checkerr(BOOL err, CHAR* format, ...)
{
    if (!err) return;

    va_list arglist;

    va_start(arglist, format);
    vfprintf(stderr, format, arglist);
    va_end(arglist);

    exit(-1);
}

INT __cdecl main(INT argc, CHAR** argv)
{
    // Parse command line options

    HRESULT hr;
    programname = *argv++;
    argc--;

    while (argc && **argv == '-') {
        if (argc > 1 && _stricmp(*argv, "-x") == 0) {
            hr = DmSetXboxName(argv[1]);
            checkerr(FAILED(hr), "Error setting xbox name: %x\n", hr);
            argv += 2; argc -= 2;
        } else {
            usage();
        }
    }
    if (argc != 1) usage();

    DmUseSharedConnection(TRUE);

    // Send irtsweep command to the xbox
    hr = DmSendCommand(NULL, "IRTSWEEP", NULL, 0);
    checkerr(FAILED(hr), "Error sending command to xbox: %x\n", hr);

    hr = DmReceiveFile(*argv, XBOX_IRTSWEEP_FILENAME);
    checkerr(FAILED(hr), "Error retrieving IDF data: %x\n", hr);

    DmUseSharedConnection(FALSE);

    return 0;
}
