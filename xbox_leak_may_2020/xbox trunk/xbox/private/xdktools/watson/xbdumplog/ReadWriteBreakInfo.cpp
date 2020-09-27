// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     ReadWriteBreakInfo.cpp
// Contents: Contains code to read/write a break info structure from/to a file
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"

// UNDONE: This is the same file as found in xbDumpLog.  Should share code (common lib?) to avoid
//         making a change in one place but not the other...


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// gs_szSignature   -- Specifies the file type.  If the file signature in a specified file doesn't
//                     match this signature, then it is considered an incorrect or mangled file.
//                     In the future, this signature will be used for backwards compatibility.
static char gs_szSignature[] = "XBW1.0";


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ReadBytes
// Purpose:   Reads bytes from a file and verifies the read succeeded.
// Arguments: hfile             -- The file to read the bytes from.
//            pvBuffer          -- The buffer to read the bytes to.
//            cBytes            -- The number of bytes to read.
// Returns:   'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool ReadBytes(HANDLE hfile, void *pvBuffer, DWORD cBytes)
{
    DWORD dwRead;

    // Read the requested number of bytes from the file.
    if (!ReadFile(hfile, pvBuffer, cBytes, &dwRead, NULL))
        return false;

    // Make sure we read all the bytes we expected to read
    if (dwRead != cBytes)
        return false;

    // Read successful!
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WriteBytes
// Purpose:   Writes bytes from a file and verifies the write succeeded.
// Arguments: hfile             -- The file to write the bytes to.
//            pvBuffer          -- The buffer to read the bytes from.
//            cBytes            -- The number of bytes to write.
// Returns:   'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool WriteBytes(HANDLE hfile, void *pvBuffer, DWORD cBytes)
{
    DWORD dwWritten;

    // Write the requested number of bytes to the file.
    if (!WriteFile(hfile, pvBuffer, cBytes, &dwWritten, NULL))
        return false;

    // Make sure we wrote all the bytes we expected to write
    if (dwWritten != cBytes)
        return false;

    // Write successful!
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WriteBreakInfo
// Purpose:   Outputs a breakinfo structure to the specified file.  This function does a DEEP copy.
// Arguments: hfile             -- The file to write the breakinfo structure to.
//            pbreakinfo        -- The breakinfo structure to write out
// Returns:   'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool WriteBreakInfo(HANDLE hfile, sBreakInfo *pbreakinfo)
{
    // Write out our signature
    if (!WriteBytes(hfile, gs_szSignature, sizeof(gs_szSignature)))
        return false;

    // Write out the Xbox name, time, and application name to the file
    if (!WriteBytes(hfile, &pbreakinfo->szXboxName, sizeof(pbreakinfo->szXboxName)) || 
        !WriteBytes(hfile, &pbreakinfo->systime, sizeof(pbreakinfo->systime))       ||
        !WriteBytes(hfile, &pbreakinfo->szAppName, sizeof(pbreakinfo->szAppName)))
        return false;

    // Write out the type of event that occurred, as well as the thread id in which it occurred.
    if (!WriteBytes(hfile, &pbreakinfo->dwEventType, sizeof(pbreakinfo->dwEventType)))
        return false;
    if (!WriteBytes(hfile, &pbreakinfo->dwBrokenThreadId, sizeof(pbreakinfo->dwBrokenThreadId)))
        return false;
    
    // If the event was a RIP, then write out the RIP string.
    if (pbreakinfo->dwEventType == IDD_RIP)
    {
        if (!WriteBytes(hfile, pbreakinfo->szRIP, sizeof(pbreakinfo->szRIP)))
            return false;
    }
    else
    {
        // It was an exception - write out the exception info
        if (!WriteBytes(hfile, &pbreakinfo->dwEventCode, sizeof(pbreakinfo->dwEventCode)))
            return false;
        if (!WriteBytes(hfile, &pbreakinfo->fWriteException, sizeof(pbreakinfo->fWriteException)))
            return false;
        if (!WriteBytes(hfile, &pbreakinfo->dwAVAddress, sizeof(pbreakinfo->dwAVAddress)))
            return false;
    }

    // Write out the module information to the file
    if (!WriteBytes(hfile, &pbreakinfo->cModules, sizeof(pbreakinfo->cModules)))
        return false;
    if (!WriteBytes(hfile, pbreakinfo->prgdmnml, sizeof(DMN_MODLOAD) * pbreakinfo->cModules))
        return false;
    
    // Write out thread informaton to the file
    if (!WriteBytes(hfile, &pbreakinfo->cThreads, sizeof(pbreakinfo->cThreads)))
        return false;

    for (DWORD i = 0; i < pbreakinfo->cThreads; i++)
    {
        sThreadInfo *pthreadinfo = &pbreakinfo->prgthreadinfo[i];
        if (pthreadinfo->fValid)
        {
            if (!WriteBytes(hfile, &pthreadinfo->cr,          sizeof(pthreadinfo->cr))          ||
                !WriteBytes(hfile, &pthreadinfo->dwThreadId,  sizeof(pthreadinfo->dwThreadId))  ||
                !WriteBytes(hfile, &pthreadinfo->dwStackBase, sizeof(pthreadinfo->dwStackBase)) ||
                !WriteBytes(hfile, &pthreadinfo->dwStackSize, sizeof(pthreadinfo->dwStackSize)))
                return false;
            if (!WriteBytes(hfile, pthreadinfo->rgbyStack, sizeof(BYTE) * pthreadinfo->dwStackSize))
                return false;
        }
    }

    // Finally, write out the RELOCATED base address of the first section
    if (!WriteBytes(hfile, &pbreakinfo->dwFirstSectionBase, sizeof(DWORD)))
        return false;

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ReadBreakInfo
// Purpose:   Reads a breakinfo structure from the specified file.
// Arguments: hfile             -- The file to read the breakinfo structure from.
//            pbreakinfo        -- The breakinfo structure to read in.
// Returns:   'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool ReadBreakInfo(HANDLE hfile, sBreakInfo *pbreakinfo)
{
    // szSignature      -- Used to verify the specified file is the correct type
    char szSignature[7];

    // Read in the file signature and verify it's the version we expect
    if (!ReadBytes(hfile, szSignature, sizeof(szSignature)))
        return false;
    if (strcmp(szSignature, gs_szSignature))
    {
        // Signatures don't match!
        return false;
    }

    // Read in the Xbox name, time, and application name from the file
    if (!ReadBytes(hfile, &pbreakinfo->szXboxName, sizeof(pbreakinfo->szXboxName)) || 
        !ReadBytes(hfile, &pbreakinfo->systime, sizeof(pbreakinfo->systime))        ||
        !ReadBytes(hfile, &pbreakinfo->szAppName, sizeof(pbreakinfo->szAppName)))
        return false;

    // Read in the type of event that occurred, as well as the thread id in which it occurred.
    if (!ReadBytes(hfile, &pbreakinfo->dwEventType, sizeof(pbreakinfo->dwEventType)))
        return false;
    if (!ReadBytes(hfile, &pbreakinfo->dwBrokenThreadId, sizeof(pbreakinfo->dwBrokenThreadId)))
        return false;
    
    // If the event was a RIP, then read in the RIP string.
    if (pbreakinfo->dwEventType == IDD_RIP)
    {
        if (!ReadBytes(hfile, pbreakinfo->szRIP, sizeof(pbreakinfo->szRIP)))
            return false;
    }
    else
    {
        // It was an exception - read in the exception info
        if (!ReadBytes(hfile, &pbreakinfo->dwEventCode, sizeof(pbreakinfo->dwEventCode)))
            return false;
        if (!ReadBytes(hfile, &pbreakinfo->fWriteException, sizeof(pbreakinfo->fWriteException)))
            return false;
        if (!ReadBytes(hfile, &pbreakinfo->dwAVAddress, sizeof(pbreakinfo->dwAVAddress)))
            return false;
    }

    // Read in the number of modules from the file
    if (!ReadBytes(hfile, &pbreakinfo->cModules, sizeof(pbreakinfo->cModules)))
        return false;
    
    // Allocate space for the modules.
    pbreakinfo->prgdmnml = new DMN_MODLOAD[pbreakinfo->cModules];
    if (pbreakinfo->prgdmnml == NULL)
        return false;

    // Read in the actual module information
    if (!ReadBytes(hfile, pbreakinfo->prgdmnml, sizeof(DMN_MODLOAD) * pbreakinfo->cModules))
        return false;
    
    // Read the number of threads in this break event.
    if (!ReadBytes(hfile, &pbreakinfo->cThreads, sizeof(pbreakinfo->cThreads)))
        return false;

    // Allocate space for the threads
    pbreakinfo->prgthreadinfo = new sThreadInfo[pbreakinfo->cThreads];
    if (pbreakinfo->prgthreadinfo == NULL)
        return false;

    // Read in thread informaton from the file
    for (DWORD i = 0; i < pbreakinfo->cThreads; i++)
    {
        sThreadInfo *pthreadinfo = &pbreakinfo->prgthreadinfo[i];
        if (!ReadBytes(hfile, &pthreadinfo->cr,          sizeof(pthreadinfo->cr))          ||
            !ReadBytes(hfile, &pthreadinfo->dwThreadId,  sizeof(pthreadinfo->dwThreadId))  ||
            !ReadBytes(hfile, &pthreadinfo->dwStackBase, sizeof(pthreadinfo->dwStackBase)) ||
            !ReadBytes(hfile, &pthreadinfo->dwStackSize, sizeof(pthreadinfo->dwStackSize)))
            return false;

        // Allocate space for the stack
        pthreadinfo->rgbyStack = new BYTE[pthreadinfo->dwStackSize];

        // Read in the actual bytes of the stack
        if (!ReadBytes(hfile, pthreadinfo->rgbyStack, sizeof(BYTE) * pthreadinfo->dwStackSize))
            return false;

        // The 'fValid' flag is really only used in writing out a break info file, but we'll set
        // it here for completeness' sake.
        pthreadinfo->fValid = true;
    }

    // Finally, get the RELOCATED base address of the first section
    if (!ReadBytes(hfile, &pbreakinfo->dwFirstSectionBase, sizeof(DWORD)))
        return false;

    return true;
}
