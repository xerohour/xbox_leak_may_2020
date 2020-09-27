/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbcap.cpp

Abstract:

    Start / stop profiling on the xbox

--*/

#include "xbcap.h"
#include <xboxdbg.h>

//
// Name of the profile data file on the xbox
//
#define XBOX_CAP_FILENAME "E:\\xboxcap.dat"

//
// Supported commands
//
enum {
    INVALIDCMD,
    STARTCMD,
    STOPCMD,
    DUMPCMD
};


//
// Global variables
//

CHAR* programName;
INT cmd;

//
// Dump usage information and exit
//
VOID
usage()
{
    fprintf(stderr,
        "usage: %s options ...\n"
        "-x xboxname\n"
        "    Specifies the target xbox name.\n"
        "start\n"
        "    Start profile data collection on the xbox.\n"
        "stop cap-filename\n"
        "    Stop profile data collection on the xbox, and retrieve\n"
        "    the profile data and store it in the specified file.\n"
        "dump cap-filename dump-filename\n"
        "    Dump the profile data to a human-readable text file.\n"
        "-s symbol-path\n"
        "    Specifies the symbol search path.You can specify multiple\n"
        "    directories by separating them with a semicolon. If -s option\n"
        "    is not present, _NT_SYMBOL_PATH environment variable is used.\n"
        "-idle\n"
        "    Do not dump the time spent in the idle thread.\n"
        "    Instead, the time is attributed to the thread that was running\n"
        "    before the system switched to the idle thread.\n"
        "-dpc\n"
        "    Whether to dump DPC calls in the output.\n"
        "-raw\n"
        "    Produce raw text dump instead of .OPF file.\n",
        programName);

    exit(1);
}

//
// Check error and exit
//
VOID
checkerr(
    BOOL err,
    CHAR* format,
    ...
    )
{
    if (!err) return;

    va_list arglist;

    va_start(arglist, format);
    vfprintf(stderr, format, arglist);
    va_end(arglist);

    exit(-1);
}

VOID
errmsg(
    CHAR* format,
    ...
    )
{
    va_list arglist;

    va_start(arglist, format);
    vfprintf(stderr, format, arglist);
    va_end(arglist);
}


//
// Main program entrypoint
//
#define optequ(s1, s2) (_stricmp(s1, s2) == 0)

INT __cdecl
main(
    INT argc,
    CHAR** argv
    )
{
	HRESULT hr;
    INT cmd = INVALIDCMD;
    char* cap_filename = NULL;
    char* dump_filename = NULL;

    programName = *argv++;
    argc--;

	// Process arguments
    while (argc--) {
        char* opt = *argv++;
        int params = 0;

        if (optequ(opt, "-x") && argc >= 1) {
            //
            // -x xboxname
            //
            hr = DmSetXboxName(*argv);
            checkerr(FAILED(hr), "Error setting xbox name: %x\n", hr);
            params += 1;
        } else if (optequ(opt, "start") && !cmd) {
            cmd = STARTCMD;
        } else if (optequ(opt, "stop") && !cmd && argc >= 1) {
            cmd = STOPCMD;
            cap_filename = *argv;
            params += 1;
        } else if (optequ(opt, "dump") && !cmd && argc >= 2) {
            cmd = DUMPCMD;
            cap_filename = argv[0];
            dump_filename = argv[1];
            params += 2;
        } else if (optequ(opt, "-s") && !symbolpath && argc >= 1) {
            symbolpath = *argv;
            params += 1;
        } else if (optequ(opt, "-v")) {
            verbose = TRUE;
        } else if (optequ(opt, "-idle")) {
            noidle = TRUE;
        } else if (optequ(opt, "-raw")) {
            rawdump = TRUE;
        } else if (optequ(opt, "-dpc")) {
            dumpdpc = TRUE;
        } else {
            usage();
        }

        argv += params;
        argc -= params;
    }

    switch (cmd) {
    case STARTCMD:
    case STOPCMD:
        // Cut down on repeated connection requests
        DmUseSharedConnection(TRUE);

        // Compose the command and send it
        hr = DmCAPControl(cmd == STARTCMD ? "start" : "stop");
        checkerr(FAILED(hr), "Error sending command to xbox: %x\n", hr);
    
        if (cmd == STOPCMD) {
            hr = DmReceiveFile(cap_filename, XBOX_CAP_FILENAME);
            checkerr(FAILED(hr), "Error retrieving profile data: %x\n", hr);
        }

        DmUseSharedConnection(FALSE);
        break;

    case DUMPCMD:
        capdump(cap_filename, dump_filename);
        break;

    default:
        usage();
    }

    return 0;
}
