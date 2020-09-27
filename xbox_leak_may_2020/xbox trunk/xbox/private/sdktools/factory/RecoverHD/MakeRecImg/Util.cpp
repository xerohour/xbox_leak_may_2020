// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      Util.cpp
// Contents:  Some utility functions for the MakeRecImg app
// Revisions: 21-Sep-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  MakePathAbsolute
// Purpose:   Ensures that the specified path is an absolute path, not a
//            local one.
// Arguments: szSourcePath      -- The path to make absolute
//            szAbsolutePath    -- The buffer to hold the result
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MakePathAbsolute(char *szSourcePath, char *szAbsolutePath)
{

    // Is the specified source path a local or absolute path?  If it's
    // local, then convert to absolute so that we don't have to worry about
    // it later.  Valid path types:
    //  X:\...      Absolute
    //  \\...       Absolute (UNC)
    //  \...        Local (since temp dir may be on different drive)
    //  ...         Local
    if ((szSourcePath[1] == ':'  && szSourcePath[2] == '\\') ||
        (szSourcePath[0] == '\\' && szSourcePath[1] == '\\'))
        strcpy(szAbsolutePath, szSourcePath);
    else
    {
        // Must be local case; combine with current directory
        char szCurDir[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, szCurDir);
        if (szSourcePath[0] == '\\')
            sprintf(szAbsolutePath, "%c:%s", szCurDir[0], szSourcePath);
        else
            sprintf(szAbsolutePath, "%s\\%s", szCurDir, szSourcePath);
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GetTempDirName
// Purpose:   Generates a temporary directory name.  Rooted in doc&settings\
//            <user>\localsettings\temp.
// Arguments: szTempDir     -- Buffer to hold temporary directory name.
// Return:    'true' if successful, 'false' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL GetTempDirName(char *szTempDir)
{
	char szTempPath[MAX_PATH];

    GetTempPath(MAX_PATH, szTempPath);
    if (GetTempFileName(szTempPath, "mri", 0, szTempDir) == 0)
    {
        // Couldn't create the temporary directory
        printf("Error - Failed to create temp directory.  Ensure that your "
               "system drive ('%c':\\) has enough free space available and "
               "that you have the appropriate administrative permissions.",
               szTempPath[0]);
        return FALSE;
    }

    // GetTempFileName forces creation of a file with the temporary name; we
    // want a directory, so delete the created file.
    if (!DeleteFile(szTempDir))
    {
        // Couldn't delete the newly created file for some reason.  This
        // shouldn't happen, but just in case, we fail
        printf("Error: Failed to delete temporary file '%s'.\n", szTempDir);
        return FALSE;
    }

    // Finally, create the directory itself
    if (!CreateDirectory(szTempDir, NULL))
    {
        printf("Error: Failed to create temporary directory '%s'.\n",
               szTempDir);
        return FALSE;
    }

    return TRUE;  
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RecursiveRemoveDirectory
// Purpose:   Recursively deletes the contents of a directory.
// Arguments: szDir         -- Name of the directory to delete.
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void RecursiveRemoveDirectory(char *szFolder)
{
    if (SetCurrentDirectory(szFolder))
    {
        WIN32_FIND_DATA wfd;
        HANDLE hfile = FindFirstFile("*.*", &wfd);
        while (hfile != INVALID_HANDLE_VALUE)
        {
            char *szFile = wfd.cFileName;
            if (strcmp(szFile, ".") && strcmp(szFile, ".."))
            {
                // It's neither '.' nor '..' - remove it!  If it's a file,
                // delete it; if it's a directory, then recurse down into it
                if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    RecursiveRemoveDirectory(szFile);
                else
                    DeleteFile(szFile);
            }

            if (!FindNextFile(hfile, &wfd))
                break;
        }
        FindClose(hfile);

        // Set our current directory back to the original directory so that
        // we can remove 'this' directory
        SetCurrentDirectory("..");

        // Finally, remove the directory itself
        RemoveDirectory(szFolder);
    }
}