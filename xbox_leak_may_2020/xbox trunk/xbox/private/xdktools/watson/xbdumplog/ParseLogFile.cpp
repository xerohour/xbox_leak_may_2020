// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     ParseLogFile.cpp
// Contents: Contains code to parse and output a user-specified log file.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// MAX_STACK_DEPTH      -- Maximum depth of a stack trace in the dump file that we'll output.  Any
//                         stack addresses deeper than this are ignored.
#define MAX_STACK_DEPTH 40

// MAX_FUNC_NAME_LEN    -- Maximum length of a function name we'll store (anything else is ignored).
#define MAX_FUNC_NAME_LEN 255


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ LOCAL GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// gs_pthreadinfoRead           -- This variable is necessary since the ImageHlp.dll 'StackWalk()'
//                                 function doesn't support user-data.  This variable points at
//                                 the 'current' thread info structure for when StackWalk() calls
//                                 'ReadStackMem()'.
static sThreadInfo *gs_pthreadinfoRead;

// EXPLICIT_IMAGEHLP            -- Force explicit loading of imagehlp.dll functionality
#define EXPLICIT_IMAGEHLP

#ifdef EXPLICIT_IMAGEHLP
// We explicitly load all of the ImageHlp functions so that we won't fail to load on a machine
// without ImageHlp.dll.  In order to do this, we define function pointers for all of the ImageHlp
// functions that we need, and use GetProcAddress to obtain those addresses.
typedef BOOL   (__stdcall *SYMINITIALIZEPROC)(HANDLE, LPSTR, BOOL);
typedef BOOL   (__stdcall *SYMCLEANUPPROC)(HANDLE);
typedef BOOL   (__stdcall *STACKWALKPROC)(DWORD, HANDLE, HANDLE, LPSTACKFRAME, LPVOID,
                                          PREAD_PROCESS_MEMORY_ROUTINE,PFUNCTION_TABLE_ACCESS_ROUTINE,
                                          PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE);
typedef LPVOID (__stdcall *SYMFUNCTIONTABLEACCESSPROC)(HANDLE, DWORD);
typedef DWORD  (__stdcall *SYMGETMODULEBASEPROC)(HANDLE, DWORD);
typedef BOOL   (__stdcall *SYMGETSYMFROMADDRPROC)(HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL);
typedef BOOL   (__stdcall *SYMGETMODULEINFOPROC)(HANDLE, DWORD, PIMAGEHLP_MODULE);
typedef BOOL   (__stdcall *SYMGETLINEFROMADDRPROC)(HANDLE, DWORD, PDWORD, PIMAGEHLP_LINE);
typedef BOOL   (__stdcall *SYMLOADMODULEPROC)(HANDLE, HANDLE, LPSTR, LPSTR, DWORD, DWORD);
typedef BOOL   (__stdcall *SYMSETOPTIONSPROC)(DWORD);
typedef PIMAGE_NT_HEADERS (__stdcall *IMAGENTHEADERPROC)(LPVOID);
 
static SYMINITIALIZEPROC           _SymInitialize;
static SYMCLEANUPPROC              _SymCleanup;
static STACKWALKPROC               _StackWalk;
static SYMFUNCTIONTABLEACCESSPROC  _SymFunctionTableAccess;
static SYMGETMODULEBASEPROC        _SymGetModuleBase;
static SYMGETSYMFROMADDRPROC       _SymGetSymFromAddr;
static SYMLOADMODULEPROC           _SymLoadModule;
static SYMGETLINEFROMADDRPROC      _SymGetLineFromAddr;
static SYMGETMODULEINFOPROC        _SymGetModuleInfo;
static SYMSETOPTIONSPROC           _SymSetOptions;
static IMAGENTHEADERPROC           _ImageNtHeader;

// g_hlibImageHlp       -- ImageHlp.dll loaded library
HMODULE g_hlibImageHlp;

#else

// Implicitly referencing ImageHlp.dll functionality.  Call the functions directly
#define _SymInitialize          SymInitialize
#define _SymCleanup             SymCleanup
#define _StackWalk              StackWalk
#define _SymFunctionTableAccess SymFunctionTableAccess
#define _SymGetModuleBase       SymGetModuleBase
#define _SymGetSymFromAddr      SymGetSymFromAddr
#define _SymLoadModule          SymLoadModule
#define _SymGetLineFromAddr     SymGetLineFromAddr
#define _SymGetModuleInfo       SymGetModuleInfo
#define _SymSetOptions          SymSetOptions
#define _ImageNtHeader          ImageNtHeader
#endif




// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ STRUCTURES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// sCallStack           -- Contains a stack backtrace.
typedef struct
{
    // dwThreadId       -- Id of thread cooresponding to this stack backtrace.
    DWORD dwThreadId;

    // nStackDepth      -- Number of elements in the FuncAddr array
    int nStackDepth;
    
    // rgdwFuncAddr     -- The list of function addresses
    DWORD rgdwFuncAddr[MAX_STACK_DEPTH];

    // rgdwFuncParam    -- the parameters to each function.
    DWORD rgdwFuncParam[MAX_STACK_DEPTH][4];

    // rgszFuncName     -- Name of the function with the matching-index address in rgdwFuncAddr.
    char rgszFuncName[MAX_STACK_DEPTH][MAX_FUNC_NAME_LEN];
} sCallStack;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GetModuleBaseAddress
// Purpose:   We need to account for base relocation of the user's module.  To do this, we query
//            the user's application for it's code's unrelocated base address and compare it to
//            the relocated base address (stored in the breakinfo structure at time of crash) in
//            order to calculate the proper offset.  Note that we don't need to do this for Xbox
//            system modules since they are forced to a fixed location (ie aren't allowed to
//            relocate).
// Arguments: szAppPath      -- Path/name of the application to examine
//            dwRelocatedBaseAddress -- The address at which the first section was loaded.
//            pdwBaseAddress  -- Buffer that will contain the final base address of the app to use.
// Return:    'true' if the base address was successfully obtained; 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool GetModuleBaseAddress(char *szAppPath, DWORD dwRelocatedBaseAddress, DWORD *pdwBaseAddress)
{
    // Open a handle to the file.  This is necessary for the memory mapped file view we'll create
    // further below.
    HANDLE hfile = CreateFile(szAppPath, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        // Failed to open the specified log file.  Try to figure out why.
        DWORD dwErr = GetLastError();
        switch(dwErr)
        {
        case ERROR_FILE_NOT_FOUND:
            OutputMsg("Error: Cannot find the user application (%s).  It must be in the specified symbol path.\r\n", szAppPath);
            return false;

        case ERROR_ACCESS_DENIED:
            OutputMsg("Error: Cannot access application '%s'.\r\n", szAppPath);
            return false;

        default:
            OutputMsg("Error: An unexpected error occurred trying to access the application '%s'.\r\n", szAppPath);
            return false;
        }
    }

    // Obtain the unrelocated base address of the first section from the application's executable.
    // To do this, we need to create a memory mapped file view of the application to pass to
    // ImageNtHeader.

    // CreateFileMapping is the first step in created a memory mapped file.  It points at the file
    // handle that we opened above.
    HANDLE hfileMap = CreateFileMapping(hfile, NULL, PAGE_READONLY, 0, 0, "xbWatson-file-map");
    if (!hfileMap)
        return false;

    // Now that we've got a file handle, map a view of it into our address space.
    VOID *pvFile = MapViewOfFile(hfileMap, FILE_MAP_READ, 0, 0, 0);
    if (!pvFile)
        return false;

    // Get a pointer to the actual NT Header structure
    IMAGE_NT_HEADERS *pnth = _ImageNtHeader(pvFile);
    if (!pnth)
        return false;
    
    // Get a pointer to the first section in the NT Header
    PIMAGE_SECTION_HEADER pish = (PIMAGE_SECTION_HEADER) ((BYTE*)pnth + sizeof(IMAGE_NT_HEADERS));
    
    // Calculate the final base address
    *pdwBaseAddress = dwRelocatedBaseAddress - pish->VirtualAddress;

    // Clean up after ourselves
    UnmapViewOfFile(pvFile);
    CloseHandle(hfile);

    // Return that we successfully obtained the base address offset.
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  FileExists
// Purpose:   Checks if the specified file exists.  
// Arguments: szFullPathFileName -- Full path and file name of the file to check
// Return:    'true' if the file exists.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool FileExists(char *szFullPathFileName)
{
    // Try opening the file.
    HANDLE hfile = CreateFile(szFullPathFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
        return false;

    // File was opened successfully (so it exists).  Close the handle since we don't actually use it
    CloseHandle(hfile);
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  InitImageHlp
// Purpose:   Initializes ImageHlp.dll symbol-lookup functionality and loads the appropriate modules
//            so that we can later map addresses to function names.
// Arguments: szSymPath     -- Path to the app's exe and pdb files (ie symbol information).
//            pbreakinfo    -- Structure containing module information at time of crash.
// Return:    'true' if the symbol-lookup functionality was successfully initialized.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool InitImageHlp(char *szSymPath, sBreakInfo *pbreakinfo)
{
    char szXBDMPath[MAX_PATH];
    char szXboxKrnlPath[MAX_PATH];
    char szUserAppName[MAX_PATH];
    char szUserAppPath[MAX_PATH];
    char szXboxSDKPath[MAX_PATH];
    bool fLoaded;

#ifdef EXPLICIT_IMAGEHLP
    // Verify that ImageHlp exists on this user's machine
    g_hlibImageHlp = LoadLibrary("ImageHlp.dll");
    if (!g_hlibImageHlp)
        return false;

    // Get function addresses to each of the functions in ImageHlp that we'll need
    _SymInitialize = (SYMINITIALIZEPROC)GetProcAddress(g_hlibImageHlp, "SymInitialize");
    if (!_SymInitialize)
        return false;

    _SymCleanup = (SYMCLEANUPPROC)GetProcAddress(g_hlibImageHlp, "SymCleanup");
    if (!_SymCleanup)
        return false;

    _StackWalk = (STACKWALKPROC)GetProcAddress(g_hlibImageHlp, "StackWalk");
    if (!_StackWalk)
        return false;

    _SymFunctionTableAccess = (SYMFUNCTIONTABLEACCESSPROC)GetProcAddress(g_hlibImageHlp, "SymFunctionTableAccess");
    if (!_SymFunctionTableAccess)
        return false;

    _SymGetModuleBase = (SYMGETMODULEBASEPROC)GetProcAddress(g_hlibImageHlp, "SymGetModuleBase");
    if (!_SymGetModuleBase)
        return false;

    _SymGetSymFromAddr = (SYMGETSYMFROMADDRPROC)GetProcAddress(g_hlibImageHlp, "SymGetSymFromAddr");
    if (!_SymGetSymFromAddr)
        return false;

    _SymLoadModule = (SYMLOADMODULEPROC)GetProcAddress(g_hlibImageHlp, "SymLoadModule");
    if (!_SymLoadModule)
        return false;

    _SymGetModuleInfo = (SYMGETMODULEINFOPROC)GetProcAddress(g_hlibImageHlp, "SymGetModuleInfo");
    if (!_SymGetModuleInfo)
        return false;

    _SymSetOptions = (SYMSETOPTIONSPROC)GetProcAddress(g_hlibImageHlp, "SymSetOptions");
    if (!_SymSetOptions)
        return false;

    _ImageNtHeader = (IMAGENTHEADERPROC)GetProcAddress(g_hlibImageHlp, "ImageNtHeader");
    if (!_ImageNtHeader)
        return false;

    // NOTE: "SymGetLineFromAddr()" was introduced in WinNT5.  We will still run if the user has
    //       an older version of ImageHlp, but they won't get actual line numbers, just function names
    //       and offsets.
    _SymGetLineFromAddr = (SYMGETLINEFROMADDRPROC)GetProcAddress(g_hlibImageHlp, "SymGetLineFromAddr");

    // We successfully loaded each function pointer.  Initialize ImageHlp now.
#endif

    // Initialize the ImageHlp.dll symbol-lookup functionality.
    if (!_SymInitialize((HANDLE)1, NULL, false))
    {
        // ImageHlp.dll doesn't appear to be on this machine.  Warn the user but continue anyways
        // so that they can see the callstack addresses.
        OutputMsg("Warning: ImageHlp.dll was not successfully loaded.  xbWatson will continue but cannot determine function names\r\n");
        return false;
    }

    // Set options so that we can get line numbers.  Also undecorate names.
    _SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

    // Get the XBox SDK installation point from the Registry
    HKEY  hkey;
    DWORD dwByte = 1024;
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\XboxSDK"), 0, KEY_READ, &hkey);
    if (RegQueryValueEx(hkey, "InstallPath", NULL,
                        NULL, (LPBYTE) szXboxSDKPath, &dwByte) != ERROR_SUCCESS)
    {
        // The SDK doesn't appear to be properly installed.  We don't force failure because the
        // user could still run this on a non-XDK machine (although they shouldn't).  The warnings
        // below will suffice to notify the user of the lack of xbox system dll symbols.
        sprintf(szXboxSDKPath, "");
    }
    else
        strcat(szXboxSDKPath, "\\Xbox\\Symbols");
    
    RegCloseKey(hkey);

    // Generate the names of the Xbox kernel and XBDM executables so that we can load them.
#define EXTERNAL
#ifdef EXTERNAL
    sprintf(szXBDMPath, "%s\\XBDM.dll", szXboxSDKPath);
    sprintf(szXboxKrnlPath, "%s\\XboxKrnl.exe", szXboxSDKPath);
#else
    sprintf(szXBDMPath, "%s\\XBDM.dll", "C:\\xboxbins\\devkit");
    sprintf(szXboxKrnlPath, "%s\\XboxKrnl.exe", "C:\\xboxbins");
#endif

    // Load the Xbox modules (this sets up the Sym functions so that they can find function addresses.
    // UNDONE-WARN: I'm unable to load symbols for BOTH XBDM and the user's app simultaneously.  I'm not
    //              sure why (conflicting base addresses would be my guess).  For now, the user's app is
    //              more important than XBDM symbols, so XBDM is out of here...
#if 0
    fLoaded = _SymLoadModule((HANDLE)1, NULL, szXBDMPath, NULL, 0, 0) ? 1 : 0;
#endif

    fLoaded = _SymLoadModule((HANDLE)1, NULL, szXboxKrnlPath, NULL, 0, 0) ? 1 : 0;
    if (!fLoaded)
    {
        // Failed to load the Xbox symbols.  Warn the user, but continue on regardless
        OutputMsg("Warning: could not load Xbox system symbols from '%s'.  Please verify installation.\r\n",
                  szXboxSDKPath);
    }

    // Output the stack dump header
    OutputMsg("* Stack backtrace for log file created at %d/%d/%d @ %d:%d:%d.%d:\r\n",
              pbreakinfo->systime.wMonth, pbreakinfo->systime.wDay, pbreakinfo->systime.wYear,
              pbreakinfo->systime.wHour, pbreakinfo->systime.wMinute, pbreakinfo->systime.wSecond,
              pbreakinfo->systime.wMilliseconds);

    // Now load the user-specific module.
    // Given pbreakinfo->szAppName="path\app.xbe", Search priority ==
    //    (a) from specified symbol path
    //    (b) from full path stored in appname

    // Verify a valid szAppName
    if (!strrchr(pbreakinfo->szAppName, '\\'))
    {
        _SymCleanup((HANDLE)1);
        return false;
    }

    // Determine the .exe name
    strcpy(szUserAppName, strrchr(pbreakinfo->szAppName, '\\')+1);
    *(strrchr(szUserAppName, '.')) = '\0';
    strcat(szUserAppName, ".exe");

    // See if the user's app's exe is in the specified symbol path.
    sprintf(szUserAppPath, "%s\\%s", szSymPath, szUserAppName);
    if (!FileExists(szUserAppPath))
    {
        // Not in the specified symbol path; check if it's in the path specified in the app itself
        // Couldn't find it there either
        OutputMsg(" (Warning: \"%s\" not found at specified symbol path (\"%s\")).\r\n", szUserAppName, szSymPath);
        sprintf(szUserAppPath, "%s", pbreakinfo->szAppName);
        if (!FileExists(szUserAppPath))
        {
            // Couldn't find it there either.  User doesn't get symbols!
            OutputMsg(" (Warning: \"%s\" not found at embedded path (\"%s\")).\r\n", szUserAppName,
                      pbreakinfo->szAppName);

            // Return that imagehlp was successfully initialized - the user just doesn't see symbols
            return true;
        }
    }

    OutputMsg(" (User app found at: %s)\r\n", szUserAppPath);

    // We need to account for base relocation of the user's module.  To do this, we query the user's
    // application for it's code's unrelocated base address and compare it to the relocated base address
    // (stored in the breakinfo structure at time of crash) in order to calculate the proper offset.
    // Note that we don't need to do this for Xbox system modules since they are forced to a fixed
    // location (ie aren't allowed to relocate).
    DWORD dwUserAppBaseAddress;

    if (!GetModuleBaseAddress(szUserAppPath, pbreakinfo->dwFirstSectionBase, &dwUserAppBaseAddress))
    {
        OutputMsg("Error: Unable to load user application from '%s'.\r\n", szUserAppPath);
        return false;
    }

    // Load the user's application's symbols
    if (!_SymLoadModule((HANDLE)1, NULL, szUserAppPath, NULL, dwUserAppBaseAddress, 0))
    {
        // Failed to load the user's executable.  Warn them but continue (they can still see addresses)
        OutputMsg("Warning: '%s' not found at specified symbol path (%s).\r\n", szUserAppName, szSymPath);
    }
    
    // Return that we successfully initialized ImageHlp.dll.
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  UninitImageHlp
// Purpose:   Uninitializes ImageHlp.dll symbol-lookup functionality.
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void UninitImageHlp()
{
    _SymCleanup((HANDLE)1);

#ifdef EXPLICIT_IMAGEHLP
    // Release our handle to the imagehlp library
    FreeLibrary(g_hlibImageHlp);
#endif
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  LoadLogFile
// Purpose:   Loads the specified log file and passes it's contents back in the specified buffer.
// Arguments: szLogFileName     -- Name of the log file to load
//            pbreakinfo        -- The breakinfo structure we'll fill in here.
// Return:    'true' if successfully loaded; 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool LoadLogFile(char *szLogFileName, sBreakInfo *pbreakinfo)
{
    // Open the specified log file
    HANDLE hfileLog = CreateFile(szLogFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfileLog == INVALID_HANDLE_VALUE)
    {
        // Failed to open the specified log file.  Try to figure out why.
        DWORD dwErr = GetLastError();
        switch(dwErr)
        {
        case ERROR_FILE_NOT_FOUND:
            OutputMsg("Error: Cannot find the specified log file (\"%s\").  It must be in the current directory or specified path.\r\n", szLogFileName);
            return false;

        case ERROR_ACCESS_DENIED:
            OutputMsg("Error: Cannot access specified log file (\"%s\").\r\n", szLogFileName);
            return false;

        default:
            OutputMsg("Error: An unexpected error occurred trying to access the specified log file (\"%s\").\r\n", szLogFileName);
            return false;
        }
    }

    if (!ReadBreakInfo(hfileLog, pbreakinfo))
    {
        // Failed to read the info for some reason.
        OutputMsg("Error: Invalid log file (\"%s\") specified.\r\n", szLogFileName);
        CloseHandle(hfileLog);
        return false;
    }

    // We successfully read from the file.  Close the file and we're done!
    CloseHandle(hfileLog);

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ReadStackMem
// Purpose:   Reads a block of memory from the stored Xbox stack.  Called by the StackWalk function
//            when walking the stack.
// Arguments: hProcess          -- Ignored since the Xbox only has one process.
//            pvBaseAddress     -- Address to start reading from.
//            pvBuffer          -- Destination buffer to copy the bits to.
//            nSize             -- Number of bytes to read.
//            pdwNumBytesRead   -- Number of bytes read.
// Returns:   'true' if memory successfully read; 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL __stdcall ReadStackMem(HANDLE hProcess, DWORD pvBaseAddress, void *pvBuffer, DWORD nSize,
                            DWORD *pdwNumBytesRead)
{
    // Since the StackWalk function doesn't allow passing of user-data, we need to go through a
    // global to track the 'current' threadinfo that is being read.

    // Calculate the real address being requested
    DWORD dwAddrReal = (DWORD)pvBaseAddress - gs_pthreadinfoRead->dwStackBase;

    // Verify the caller requested a valid address range
    if ((int)dwAddrReal < 0 || (int)dwAddrReal > (int)gs_pthreadinfoRead->dwStackSize)
    {
        // Uh oh, an out-of-range address was requested.  While ya might think returning 'false'
        // would be the right thing to do, you'd be wrong.  Instead we pretend we actually were
        // able to read from the requested location, only we indicate that no bytes were actually
        // read.  Confusing?  Sure, but it's what StackWalk needs to be happy.
        *pdwNumBytesRead = 0;
        memset(pvBuffer, 0, nSize);
        return true;
    }

    *pdwNumBytesRead = min(nSize, gs_pthreadinfoRead->dwStackSize - dwAddrReal);

    // Copy the requested memory
    memcpy(pvBuffer, &gs_pthreadinfoRead->rgbyStack[dwAddrReal], *pdwNumBytesRead);
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GenerateCallstack
// Purpose:   Given a set of thread stack information, uses ImageHlp.dll to walk through the stack
//            and determine the stack back trace.
// Arguments: pcallstack        -- Pointer to the call stack structure we'll fill in.
//            pthreadinfo       -- Structure containing stack information (stack dump, Esp, etc).
// Return:    'true' if callback stack successfully generated; 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool GenerateCallstack(sCallStack *pcallstack, sThreadInfo *pthreadinfo)
{
    // sf           -- The structure containing context information for StackWalk
    STACKFRAME sf;

    // Initialize the stackframe structure with values that are consistent across all addresses
    memset (&sf, 0, sizeof(sf));
    sf.AddrPC.Mode    = AddrModeFlat;
    sf.AddrStack.Mode = AddrModeFlat;
    sf.AddrFrame.Mode = AddrModeFlat;

    sf.AddrPC.Offset    = pthreadinfo->cr.Eip;
    sf.AddrStack.Offset = pthreadinfo->cr.Esp;
    sf.AddrFrame.Offset = pthreadinfo->cr.Ebp;

    // Initialize the callstack
    pcallstack->nStackDepth = 0;

    // Store a global pointer to the 'current' threadinfo structure.  This is necessary since
    // StackWalk doesn't support user-data
    gs_pthreadinfoRead = pthreadinfo;

    // Walk the stack
    while (true)
    {
        if (!_StackWalk(IMAGE_FILE_MACHINE_I386, (HANDLE) 1, (HANDLE)1, &sf, 0, &ReadStackMem,
                       _SymFunctionTableAccess, _SymGetModuleBase, 0))
            break;

        // Copy the backtrace information
        pcallstack->rgdwFuncAddr[pcallstack->nStackDepth] = sf.AddrPC.Offset;
        pcallstack->rgdwFuncParam[pcallstack->nStackDepth][0] = sf.Params[0];
        pcallstack->rgdwFuncParam[pcallstack->nStackDepth][1] = sf.Params[1];
        pcallstack->rgdwFuncParam[pcallstack->nStackDepth][2] = sf.Params[2];
        pcallstack->rgdwFuncParam[pcallstack->nStackDepth][3] = sf.Params[3];
        pcallstack->nStackDepth++;
    }

    // Set misc thread info
    pcallstack->dwThreadId = pthreadinfo->dwThreadId;

    return true;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GetSymNames
// Purpose:   Enumerate the list of addresses in the specified callstack and try to assign function
//            names to them using the ImageHlp.dll symbol-lookup functionality
// Arguments: pcallstack        -- Pointer to the call stack structure with addresses to convert.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void GetSymNames(sCallStack *pcallstack)
{
    char szBuffer[1024];

    // UNDONE-WARN: Comment this better.

    // Iterate over each address in the recorded call stack, and try to determine the actual symbol
    // name and offset of each stack address.
    for (int i = 0; i < pcallstack->nStackDepth; i++)
    {
        DWORD            dwSymDisp = 0;
        BYTE             bySymBuf[sizeof(IMAGEHLP_SYMBOL) + 512];
        PIMAGEHLP_SYMBOL pihs = (PIMAGEHLP_SYMBOL)bySymBuf;
        IMAGEHLP_MODULE mi;
        memset(&mi, 0, sizeof(mi));
        mi.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
        pihs->SizeOfStruct  = sizeof(bySymBuf);
        pihs->MaxNameLength = 512;

        strcpy(pcallstack->rgszFuncName[i], "    ");

        if (_SymGetModuleInfo((HANDLE)1, pcallstack->rgdwFuncAddr[i] - dwSymDisp, &mi))
        {
            strcat(pcallstack->rgszFuncName[i], mi.ModuleName);
            strcat(pcallstack->rgszFuncName[i], "!");
        }
        else
            strcat(pcallstack->rgszFuncName[i], "Unknown!");

        // Lookup the next function address in the callstack and see if we can match a function name
        // to it.
        if (_SymGetSymFromAddr((HANDLE)1, pcallstack->rgdwFuncAddr[i], &dwSymDisp, pihs))
            sprintf(szBuffer, "%hs + 0x%X\r\n    ", pihs->Name, dwSymDisp);
        else
            sprintf(szBuffer, "Unknown Function\r\n    ");

        strcat(pcallstack->rgszFuncName[i], szBuffer);
        strcat(pcallstack->rgszFuncName[i], "(");

        // If the user has a new-enough ImageHlp.dll, then they have 'SymGetLineFromAddr()' access.
        if (_SymGetLineFromAddr)
        {
            IMAGEHLP_LINE stLine;

            // Initialize the stLine structure as SymGetLineFromAddr needs.
            memset(&stLine, 0, sizeof(IMAGEHLP_LINE));
            stLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);
            
            // Get the line number of the specified address.
            if (_SymGetLineFromAddr((HANDLE)1, pcallstack->rgdwFuncAddr[i], &dwSymDisp, &stLine))
            {
                // Successfully got the line number.  Need to append it to the buffer
                sprintf(szBuffer, "File: '%s'  Line#: %d)", stLine.FileName, stLine.LineNumber);
                strcat(pcallstack->rgszFuncName[i], szBuffer);
            }
            else
                strcat(pcallstack->rgszFuncName[i], "Unknown file)");
        }
        else
            strcat(pcallstack->rgszFuncName[i], "Unknown file)");
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ParseLogFile
// Purpose:   Parses the xbWatson log file, determines the function names of the callstack(s)  (using
//            ImageHlp.dll) and dumps it to the command prompt.
// Arguments: szLogFileName     -- Name of the log file to dump
//            szSymPath         -- Path containing symbols related to the specified log file's app.
// Return:    '1' if successful, '0' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int ParseLogFile(char *szLogFileName, char *szSymPath)
{
    // nRet             -- Returned value; by default we assume everything went fine.
    int nRet = 1;

    // iThread          -- Used to walk the array of threads
    DWORD iThread;

    // callstack        -- Contains information pertaining to a thread's callstack.
    sCallStack callstack;

    // breakinfo        -- Information about the break.
    sBreakInfo breakinfo;


    // Load the user-specified dump log.
    if (!LoadLogFile(szLogFileName, &breakinfo))
    {
        // Failed to load the log file.  LoadLogFile already notified the user of the error, so
        // don't bother printing anything here.
        return 0;
    }

    // Initalize ImageHlp.dll so that we can walk the stack and determine function names from
    // addresses.  Note that we require ImageHlp to do this, so if it failed to load we're hosed.
    if (!InitImageHlp(szSymPath, &breakinfo))
    {
        // Failed to initialize ImageHlp.dll.  InitImageHlp already notified the user of the error,
        // so don't bother printing anything here.
        return 0;
    }

    OutputMsg("\r\n- Loaded Modules:\r\n");
    
    // Dump the module list
    for (DWORD iModule = 0; iModule < breakinfo.cModules; iModule++)
    {
        OutputMsg("    (%08x - %08x)  %s\r\n", breakinfo.prgdmnml[iModule].BaseAddress, 
                                               (DWORD)breakinfo.prgdmnml[iModule].BaseAddress +
                                                      breakinfo.prgdmnml[iModule].Size,
                                               breakinfo.prgdmnml[iModule].Name);
    }

    // Given the breakinfo, now determine the actual callstack addresses for each thread
    for (iThread = 0; iThread < breakinfo.cThreads; iThread++)
    {
        // Generate the callback stack from the thread's stack dump
        if (!GenerateCallstack(&callstack, &breakinfo.prgthreadinfo[iThread]))
        {
            // Failed to generate the back trace callstack for this thread, so just ignore it
            OutputMsg("\r\n-Backtrace for thread 0x%08x:\r\n", callstack.dwThreadId);
            OutputMsg("    Failed to generate callstack from stack dump.\r\n");
        }
        else
        {
            // Output the thread header
            OutputMsg("\r\n-Thread 0x%08x", callstack.dwThreadId);
            if (callstack.dwThreadId == breakinfo.dwBrokenThreadId)
            {
                // This is the thread in which the break event occurred.  Output information about
                // the break event.
                switch(breakinfo.dwEventType)
                {
                case IDD_EXCEPTION:
                    OutputMsg(", Exception 0x%08x", breakinfo.dwEventCode);
                    switch(breakinfo.dwEventCode)
                    {
                    case 0x80000003:
                        // Breakpoint
                        OutputMsg(" (Breakpoint)");
                        break;

                    case 0xC0000005:
                        // Access Violation
                        if (breakinfo.fWriteException)
                            OutputMsg(" (Write violation at \"0x%08X\")", breakinfo.dwAVAddress);
                        else
                            OutputMsg(" (Read violation at \"0x%08X\")", breakinfo.dwAVAddress);
                        break;
                    }
                    break;

                case IDD_RIP:
                    OutputMsg(", RIP -- \"%s\"", breakinfo.szRIP);
                    break;
                }
            }
            else
                OutputMsg("\r\n");

            // Dump register information for this thread
            OutputMsg("\r\n  Registers:\r\n  eax:0x%08x ebx:0x%08x ecx:0x%08x edx:0x%08x eip:0x%08x\r\n" \
                      "  esi:0x%08x edi:0x%08x esp:0x%08x ebp:0x%08x eflags:0x%08x\r\n",
                      breakinfo.prgthreadinfo[iThread].cr.Eax, breakinfo.prgthreadinfo[iThread].cr.Ebx,
                      breakinfo.prgthreadinfo[iThread].cr.Ecx, breakinfo.prgthreadinfo[iThread].cr.Edx,
                      breakinfo.prgthreadinfo[iThread].cr.Eip, breakinfo.prgthreadinfo[iThread].cr.Esi,
                      breakinfo.prgthreadinfo[iThread].cr.Edi, breakinfo.prgthreadinfo[iThread].cr.Esp,
                      breakinfo.prgthreadinfo[iThread].cr.Ebp, breakinfo.prgthreadinfo[iThread].cr.EFlags);

            // Get the symbol names
            GetSymNames(&callstack);

            // Dump the obtained callstack
            OutputMsg("\r\n  Callstack:\r\n  Addr       Param1       Param2      Param3      Param4\r\n");
            for (int i = 0; i < callstack.nStackDepth; i++)
            {
                // Output the information related to this address in the callstack.
                OutputMsg("  0x%08x (0x%08x, 0x%08x, 0x%08x, 0x%08x)\r\n%s\r\n\r\n",
                          callstack.rgdwFuncAddr[i], callstack.rgdwFuncParam[i][0], 
                          callstack.rgdwFuncParam[i][1], callstack.rgdwFuncParam[i][2],
                          callstack.rgdwFuncParam[i][3], callstack.rgszFuncName[i]);
            }
        }
        OutputMsg("\r\n");
    }

    // Clean up after ourselves
    UninitImageHlp();

    return 1;
}
