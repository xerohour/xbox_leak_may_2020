// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      xbdumpfst.cpp
// Contents:  
// Revisions: 2-Jan-2002: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- stdafxiled header file
#include "stdafx.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL HandleCommandLine(int argc, void *argv[])
{
    if (argc != 2)
    {
        printf("Dump contents of Xbox FST file.  Specified file must be in "\
               "current directory.\n\nXBDUMPFST <filename>.\n\n");
        return FALSE;
    }
    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  main
// Purpose:   This is the default entry point into a console application.  
//            When the application is started, Windows calls into this
//            function - when this function exits, the application ends.
// Arguments: argc          -- The number of arguments on the command line
//            argv          -- The list of arguments on the command line
// Return:    Result value application returns to the operating system
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int __cdecl main(int argc, void *argv[])
{
    if (!HandleCommandLine(argc, argv))
        return -1;

    // Open the file for parsing
    CFST fst((char*)argv[1]);
    fst.Dump();
    
    // Return success
    return 0;
}
