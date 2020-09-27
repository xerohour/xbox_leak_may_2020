// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      xbpremaster.cpp
// Contents:  Main entry point for the xbPremaster application
// Revisions: 31-May-2001: Created (jeffsim)
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

char g_szDefaultFLD[MAX_PATH], g_szDefaultPKG[MAX_PATH], g_szDefaultOutputFile[MAX_PATH];
int g_iDefaultTapeDrive;
char g_szPackageExtractFolder[MAX_PATH];


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ParseCommandLine
// Purpose:   Determine what, if anything, the user specified on the command
//            line.
// Arguments: szCmdLine     -- The command line string.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ParseCommandLine(char *szCmdLine)
{
    char *szToken;

    // Set defaults
    strcpy(g_szDefaultFLD, "");
    strcpy(g_szDefaultPKG, "");
    strcpy(g_szDefaultOutputFile, "");
    strcpy(g_szPackageExtractFolder, "");
    g_iDefaultTapeDrive = -1;

    // Create local copy so that the strtok doesn't destroy the passed-in string
    char szCmd[MAX_PATH * 2 + 10];
    strncpy(szCmd, szCmdLine, min(strlen(szCmdLine)+1, sizeof(szCmd)));

    if (*szCmd == '"')
        szToken = strtok(szCmd, "\"");
    else
        szToken = strtok(szCmd, " ");
    if (!szToken)
        return;

    char *szFileExt = szToken + strlen(szToken) - 4;
    if (!lstrcmpi(szFileExt, ".fld"))
        strcpy(g_szDefaultFLD, szToken);
    else if (!lstrcmpi(szFileExt, ".pkg"))
    {
        strcpy(g_szDefaultPKG, szToken);

        // If a PKG was specified, then a tape drive must have been specified, and
        // an output file may have been specified
        szToken = strtok(NULL, " ");
        if (!szToken)
            goto dumpusage;
        g_iDefaultTapeDrive = atoi(szToken);

        // User could have specified a default output log file
        szToken = strtok(NULL, " ");
        if (szToken)
        {
            strcpy(g_szDefaultOutputFile, szToken);

            // User could have specified a tempdir
            szToken = strtok(NULL, " ");
            if (szToken)
                strcpy(g_szPackageExtractFolder, szToken);
        }
    }
    else
    {
dumpusage:
        MessageBox(NULL, "xbPremaster Command Line options:\nxbPremaster.exe "
                         "[source fld or pkg file] [outputtape] [output "
                         "logfile] [tempdir]\n* If fld file is specified, outputtape, "
                         "logfile, and tempdir are ignored\n* If pkg file specified, "
                         "outputtape is mandatory, while logfile and tempdir are optional.\n"
                         "NOTE: Order must match above.",
                         "Invalid Command Line", MB_ICONEXCLAMATION | MB_OK);
        exit(-1);
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WinMain
// Purpose:   This is the default entry point into a Windows application.  When the application is
//            started, Windows calls into this function - when this function exits, the application
//            ends.  This function creates the Win32 class that represents this application, sets
//            up the Win32 Window in which the shell will be rendered, and then goes into the Win32
//            message loop that drives the application.
// Arguments: hinst         -- The handle to this instance of the application
//            hinstPrev     -- Obsolete; throwback to Win16 -- ignore
//            szCmdLine     -- Contains the parameter list passed to this application
//            nCmdShow      -- Default visibility of the application (maximized, minimized, etc)
// Return:    Value application returns to the operating system specifying result of execution.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR szCmdLine, int nCmdShow)
{
    // In this wizard-like application, program flow is contained in and controlled by objects
    // called 'steps'.  Each step represents a state of the program; the program can only be on
    // one step at any time.  A CStep contains the UI and processing code for the step.  The CStep
    // also determines which step should follow it.  The CDlg is the object which the step displays
    // it's UI on.

#ifndef _DEBUG   // Don't do exception handling on debug builds so that we can debug exceptions
    try
#endif
    {
        // Check if the user specified a source fld or pkg, and tape drive and output
        ParseCommandLine(szCmdLine);

        // Create the dialog box in which the steps will be displayed.
        CDlg *pdlg = new CDlg(hinst);
        if (!pdlg->IsInited())
            ReportError(ERROR_INIT);

        // Create the first step that the user will perform.
        CStep *pstepCur;
        if (g_iDefaultTapeDrive == -1)
            pstepCur = new CStep_UserAction(pdlg);
        else
            pstepCur = new CStep_DestSource(pdlg);

        if (!pstepCur->IsInited())
            ReportError(ERROR_INIT);

        // Loop over steps until the user chooses to exit the application or an error occurs
        while (pstepCur)
        {
            // Run the current step.  This will display its interface and handle user input and
            // processing.  The return value is the pointer to the next step to perform.  The
            // pointer will be NULL if there are no more steps to perform (ie the user exited the
            // app or an unrecoverable error happened in the CDlg::Run call)
            CStep *pstepNext = pdlg->Run(pstepCur);

            // Delete the current step and point to the next one
            delete pstepCur;
            pstepCur = pstepNext;
        }
    }
#ifndef _DEBUG
    catch(...)
    {
        // An unexpected error has occurred.  Report it to the user and exit
        ReportError(ERROR_UNEXPECTED);
    }
#endif

    return 0;
}
