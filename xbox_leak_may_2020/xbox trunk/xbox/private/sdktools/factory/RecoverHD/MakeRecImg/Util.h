// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      Util.h
// Contents:  
// Revisions: 21-Sep-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  MakePathAbsolute
// Purpose:   Ensures that the specified path is an absolute path, not a
//            local one.
// Arguments: szSourcePath      -- The path to make absolute
//            szAbsolutePath    -- The buffer to hold the result
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MakePathAbsolute(char *szSourcePath, char *szAbsolutePath);

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GetTempDirName
// Purpose:   Generates a temporary directory name.  Rooted in doc&settings\
//            <user>\localsettings\temp.
// Arguments: szTempDir     -- Buffer to hold temporary directory name.
// Return:    'true' if successful, 'false' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL GetTempDirName(char *szTempDir);

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RecursiveRemoveDirectory
// Purpose:   Recursively deletes the contents of a directory.
// Arguments: szDir         -- Name of the directory to delete.
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void RecursiveRemoveDirectory(char *szFolder);
