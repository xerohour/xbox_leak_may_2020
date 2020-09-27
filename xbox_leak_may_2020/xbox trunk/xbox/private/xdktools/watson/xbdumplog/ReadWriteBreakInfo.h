// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     ReadWriteBreakInfo.h
// Contents: Contains declarations of functions and variables defined in ReadWriteBreakInfo.cpp
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WriteBreakInfo
// Purpose:   Outputs a breakinfo structure to the specified file.  This function does a DEEP copy.
// Arguments: hfile             -- The file to write the breakinfo structure to.
//            pbreakinfo        -- The breakinfo structure to write out
// Returns:   'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool WriteBreakInfo(HANDLE hfile, sBreakInfo *pbreakinfo);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ReadBreakInfo
// Purpose:   Reads a breakinfo structure from the specified file.
// Arguments: hfile             -- The file to read the breakinfo structure from.
//            pbreakinfo        -- The breakinfo structure to read in.
// Returns:   'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool ReadBreakInfo(HANDLE hfile, sBreakInfo *pbreakinfo);
