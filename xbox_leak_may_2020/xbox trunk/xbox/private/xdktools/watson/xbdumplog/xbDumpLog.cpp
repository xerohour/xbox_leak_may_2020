// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     xbDumpLog.cpp
// Contents: contains main entry point for xbDumpLog application.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ LOCAL GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DumpUsage
// Purpose:   Displays usage information to the user
// Arguments: None
// Return:    Always false.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool DumpUsage()
{
    // Pop up a message box explaining proper xbWatson parameters for the user.
    // UNDONE: convert from messagebox to print
    printf("Perform a post-mortem analysis of an xbWatson crash log.\n\n");
    printf("XBDUMPLOG -f crashlog [-p symbolpath]\n\n");
    printf("  -f        Specifies logfile to analyze.\n");
    printf("  -p        Specifies symbol path containing .exe and .pdb file\n\n");

    printf("If no symbol path is specified, the current path is assumed.\n");
    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ParseCommandLine
// Purpose:   Parses the user-specified command line looking for arguments that affect our functionality.
// Arguments: szCmdLine     -- Contains the parameter list passed to this application
//            szLogFile     -- String filled with the name of the source log file if specified
//            szSymPath     -- String filled with the name of the symbol path if specified
// Return:    true if successfully parsed; false if an invalid parameter was specified.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool ParseCommandLine(int argc, char *argv[], char *szLogFile, char *szSymPath)
{
    bool fLogSpecified = false;

    for (int i = 1; i < argc; i++)
    {
        char *psz = argv[i];

        // First character of each token must be '-' or '/'
        if (*psz != '-' && *psz != '/')
            return DumpUsage();
        switch(*(++psz))
        {
        case 'f':
        case 'F':
            // User is specifying a log file that we will want to dump.  Get the name of the
            // source log file
            strcpy(szLogFile, argv[++i]);
            fLogSpecified = true;
            break;

        case 'p':
        case 'P':
            // User is specifying the symbol path
            strcpy(szSymPath, argv[++i]);
            break;

        case 'h':
        case 'H':
            // User wants to see usage information
            return DumpUsage();

        default:
            // Invalid argument specified - display usage information to the user.
            return DumpUsage();
        }
    }

    if (!fLogSpecified)
        return DumpUsage();
    
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  main
// Purpose:   This is the default entry point into a console application.
// Arguments: argc			-- The number of arguments passed in the command line.
//            argv			-- The list of arguments passed in the command line.
// Return:    Zero for normal termination, non-zero for abnormal termination.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int _cdecl main(int argc, char *argv[])
{
    // szLogFile      -- Name of the output file that contains the log dump to parse and display.
    char szLogFile[MAX_PATH];

    // szSymPath      -- Location of user App symbols and executable.  If no symbol path is
    //                   specified, then current directory is assumed.
    char szSymPath[MAX_PATH] = ".";

    // Parse the specified command line.
    if (!ParseCommandLine(argc, argv, szLogFile, szSymPath))
        return 0;

    // Dump the specified log file to the console.
    if (!ParseLogFile(szLogFile, szSymPath))
        return 1;
    
    // Return that everything went fine.
	return 0;
}
