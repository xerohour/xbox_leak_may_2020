//  CUSTOM.CPP
//
//      This file is the only file that normally needs to be modified to make
//      modifications for a particular setup instance. Any changes outside
//      this file should be made to keep the rest of the code a general-purpose
//      engine.
//
//  Created 30-Mar-2001 [JonT]
//  Separated into its own DLL 23-Aug-2001 [MitchdD]


#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <custom.h>
#include <settings.h>
#include <resource.h>
#include <helper.h>
#include <global.h>
#include <parse.h>
#include <xboxverp.h>

#define XDK_BUILD_REGKEY _TEXT("SOFTWARE\\Microsoft\\XboxSDK")
#define XDK_BUILD_REGVALUE _TEXT("Build")
#define XDK_FULL_BUILD_REGVALUE _TEXT("InstalledVersion")
#define XDK_BUILDTIME_REGVALUE _TEXT("LastInstallDate")
#define XDK_OLD_INSTALL_REGVALUE _TEXT("SDK Doc Path")
#define XDK_NEW_INSTALL_REGVALUE _TEXT("InstallPath")

#define VC_SP_REGKEY _TEXT("SOFTWARE\\Microsoft\\VisualStudio\\6.0\\ServicePacks")

typedef BOOL (*LPGETVERSIONEX)(OSVERSIONINFOEX*);

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

//---------------------------------------------------------------------
//  Declare CXdkCustom implementing CCustom

class CXdkCustom : public CCustom
{
public:
    BOOL DoPreSetupChecks();
    BOOL ValidateDirectory(
            LPCSTR lpDefaultDir,    // Default dir: set on upgrade or by the user
            LPCSTR lpID,            // Target directory ID string. Unique.
            LPSTR lpCommand,        // Command that was used to fill in the target path
            LPSTR lpData,           // Data used to fill in the target path (after : in .INI file)
            LPSTR lpPath            // Path files will be copied to with this ID
            );
    VOID AfterCopy();
    VOID PostSetup(int nRet);
};


//---------------------------------------------------------------------
//  Globals
CXdkCustom   g_XdkCustom;
CCustomSite *g_pCustomSite = NULL;
HINSTANCE    g_hInstance = NULL;

CCustom * __stdcall GetCustom(CCustomSite *pSetupSite)
{
    g_pCustomSite = pSetupSite;
    return dynamic_cast<CCustom *>(&g_XdkCustom);
}

//---------------------------------------------------------------------
//  DllMain
BOOL DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
    if(dwReason==DLL_PROCESS_ATTACH)
    {
        g_hInstance = hInstance;
    }

    return TRUE;
}

inline static HINSTANCE GetInstanceHandle() {return g_hInstance;}

//---------------------------------------------------------------------
//  Helper functions


void XdkLastError(UINT uResourceId, DWORD dwErrorCode, ...)
{
    if(g_pCustomSite)
    {
        char szFormat[MAX_PATH];
        char szText[MAX_PATH];
        LoadString(GetInstanceHandle(), uResourceId, szFormat, sizeof(szFormat));
        va_list vl;
        va_start(vl, dwErrorCode);
        vsprintf(szText, szFormat, vl);
        va_end(vl);
        g_pCustomSite->SetLastErrorString(szText, dwErrorCode);
    }
}

BOOL
CheckForMSDev()
{
    TCHAR szPath[MAX_PATH];
    TCHAR szTitle[MAX_PATH];
    TCHAR szText[MAX_PATH];

    // Get the path from the registry
    if (GetRegistryString(szPath, MAX_PATH, HKEY_LOCAL_MACHINE,
        _TEXT("SOFTWARE\\Microsoft\\VisualStudio\\6.0\\Setup"), _TEXT("VsCommonDir")) == FALSE)
    {
        XdkLastError(IDS_ERRORNOMSDEV, 2001);
        return FALSE;
    }

    // Add the rest of the path to msdev.exe
    int nLen = lstrlen(szPath);
    lstrcpyn(&(szPath[nLen]), _TEXT("\\MSDev98\\Bin\\msdev.exe"), ARRAYSIZE(szPath) - nLen);

    // Loop until the file is not busy anymore or the user tell us to stop trying
    while (TRUE)
    {
        // Open the file. This checks for existence and for the file being in use
        HANDLE hfile = CreateFile(szPath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    
        // If this succeeded, we're good to go
        if (hfile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hfile);
            return TRUE;
        }

        // Any error besides file busy means msdev isn't installed
        if (GetLastError() != ERROR_SHARING_VIOLATION)
        {
            XdkLastError(IDS_ERRORNOMSDEV, 2002);
            return FALSE;
        }
    
        // Give the user a chance to exit MSDEV before bailing on setup
        LoadString(GetInstanceHandle(), IDS_XDK_TITLE, szTitle, MAX_PATH);
        LoadString(GetInstanceHandle(), IDS_MB_MSDEVINUSE, szText, MAX_PATH);
        if (MessageBox(NULL, szText, szTitle, MB_YESNO) == IDNO)
        {
            XdkLastError(IDS_ERRORMSDEVINUSE, 2003);
            return FALSE;
        }
    }
}


BOOL
CheckForProgramInUse(
    LPCTSTR lpExeName
    )
{
    TCHAR szSearch[MAX_PATH];
    TCHAR szTitle[MAX_PATH];
    TCHAR szText[MAX_PATH];
    TCHAR szOutput[MAX_PATH];
    LPTSTR lp;

    // Search the path for the tool. If we don't find it, we're good.
    if (!SearchPath(NULL, lpExeName, NULL, MAX_PATH, szSearch, &lp))
        return TRUE;

    // Loop until the file is not busy anymore or the user tell us to stop trying
    while (TRUE)
    {
        // Open the file. This checks for existence and for the file being in use
        HANDLE hfile = CreateFile(szSearch, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    
        // If this succeeded, we're good to go
        if (hfile != INVALID_HANDLE_VALUE || GetLastError() != ERROR_SHARING_VIOLATION)
        {
            CloseHandle(hfile);
            return TRUE;
        }

        // Give the user a chance to exit the application before bailing on setup
        LoadString(GetInstanceHandle(), IDS_XDK_TITLE, szTitle, MAX_PATH);
        LoadString(GetInstanceHandle(), IDS_MB_APPINUSE, szText, MAX_PATH);
        sprintf(szOutput, szText, lpExeName);
        if (MessageBox(NULL, szOutput, szTitle, MB_YESNO) == IDNO)
        {
            XdkLastError(IDS_ERRORAPPINUSE, 2016, lpExeName);
            return FALSE;
        }
    }
}


BOOL
CheckForSPAndProcPack()
{
    TCHAR szPath[MAX_PATH];

    // See if the service pack is installed
    if (!GetRegistryString(szPath, MAX_PATH, HKEY_LOCAL_MACHINE, VC_SP_REGKEY, _TEXT("SP4")) &&
        !GetRegistryString(szPath, MAX_PATH, HKEY_LOCAL_MACHINE, VC_SP_REGKEY, _TEXT("SP5")) &&
        !GetRegistryString(szPath, MAX_PATH, HKEY_LOCAL_MACHINE, VC_SP_REGKEY, _TEXT("SP6"))) // doesn't exist yet
    {
        XdkLastError(IDS_ERRORNOTVCSP4, 2013);
        return FALSE;
    }

    // Get the path to the VC Product directory
    if (GetRegistryString(szPath, MAX_PATH, HKEY_LOCAL_MACHINE,
        _TEXT("SOFTWARE\\Microsoft\\DevStudio\\6.0\\Products\\Microsoft Visual C++"),
        _TEXT("ProductDir")) == FALSE)
    {
        XdkLastError(IDS_ERRORNOMSDEV, 2004);
        return FALSE;
    }

    // Get the path to C2.DLL and compare version information against the processor pack version
    int nLen = lstrlen(szPath);
    lstrcpyn(&(szPath[nLen]), _TEXT("\\Bin\\c2.dll"), ARRAYSIZE(szPath) - nLen);
    int nRet;
    if (!CompareFileVersion(szPath, (13 << 16), (8942 << 16), &nRet))
    {
        XdkLastError(IDS_ERRORNOMSDEV, 2005);
        return FALSE;
    }

    // If the version is < what we expect, we have to bail
    if (nRet < 0)
    {
        XdkLastError(IDS_ERRORNOPROCPACK, 2006);
        return FALSE;
    }

    return TRUE;
}


//  PrependPathElement
//      Returns a new path with the element in question added in.
//      The new path is malloc'ed, caller's responsibility to free it.

LPSTR
PrependPathElement(
    LPSTR lpPath,
    LPSTR lpComponent
    )
{
    TCHAR szFullComponent[MAX_PATH];
    LPSTR lp;

    // Get the full path of the component passed in
    strcpy(szFullComponent, g_pCustomSite->GetDefaultDir());
    ::AppendSlash(szFullComponent);
    strcat(szFullComponent, lpComponent);
    DWORD dwcComponent = strlen(szFullComponent);

    // Search for this component
    for (lp = lpPath ; *lp ;)
    {
        // Did we find the component?
        if (_strnicmp(lp, szFullComponent, dwcComponent) == 0)
        {
            // We found it, return an unchanged copy of the string
            return _strdup(lpPath);
        }

        // Scan to the next component
        for (; *lp && *lp != ';' ; ++lp)
            ;
        if (*lp == ';')
            ++lp;
    }

    // Allocate some memory for the new path
    lp = (LPSTR)malloc(strlen(lpPath) + strlen(szFullComponent) + 2);

    // Make the new path
    strcpy(lp, szFullComponent);
    strcat(lp, ";");
    strcat(lp, lpPath);

    return lp;
}

//  MungeMSDEVPaths
//      This is yucky stuff we have to do to support the Xbox Target type. Add the
//      correct directories to their 'path' registry values
//      WARNING!! If you change this code, make sure you change the uninstaller, too!!
//      The uninstaller reverses the effect of this code.

VOID
MungeMSDEVPaths()
{
    LPCSTR lpWin32Directories = _TEXT("Software\\Microsoft\\Devstudio\\6.0\\Build System\\Components\\Platforms\\Win32 (x86)\\Directories");
    LPCSTR lpXboxDirectories = _TEXT("Software\\Microsoft\\Devstudio\\6.0\\Build System\\Components\\Platforms\\Xbox\\Directories");
    LPCSTR lpPathDirs = _TEXT("Path Dirs");
    LPSTR lpDefaultDir = g_pCustomSite->GetDefaultDir();
    HKEY hkey;

    // Does a Xbox target platform "Path Dirs" exist?
    LPSTR lpXboxPlatformPath = GetRegistryStringAlloc(HKEY_CURRENT_USER, lpXboxDirectories, lpPathDirs);
    LPSTR lpWin32PlatformPath = GetRegistryStringAlloc(HKEY_CURRENT_USER, lpWin32Directories, lpPathDirs);
    if (lpXboxPlatformPath)
    {
        // Yes, so add %xdk%\bin\vc7 and %xdk%\bin
        LPSTR lpPath1 = PrependPathElement(lpXboxPlatformPath, _TEXT("Xbox\\Bin"));
        LPSTR lpPath2 = PrependPathElement(lpPath1, _TEXT("Xbox\\Bin\\VC7"));
        WriteRegistryString(HKEY_CURRENT_USER, lpXboxDirectories, lpPathDirs, lpPath2);
        free(lpPath1);
        free(lpPath2);
    }

    // Does the Win32 target platform path exist instead?
    else if (lpWin32PlatformPath)
    {
        // Yes, so add %xdk%\bin\vc7 and %xdk%\bin
        LPSTR lpPath1 = PrependPathElement(lpWin32PlatformPath, _TEXT("Xbox\\Bin"));
        LPSTR lpPath2 = PrependPathElement(lpPath1, _TEXT("Xbox\\Bin\\VC7"));
        WriteRegistryString(HKEY_CURRENT_USER, lpXboxDirectories, lpPathDirs, lpPath2);
        free(lpPath1);
        free(lpPath2);
    }

    // No, create from scratch
    else
    {
        // Get the path to some VC components
        LPSTR lpMSDEVPath = GetRegistryStringAlloc(HKEY_LOCAL_MACHINE,
            _TEXT("Software\\Microsoft\\DevStudio\\6.0\\Products\\Microsoft Visual C++"),
            _TEXT("ProductDir"));

        // Get the path environment variable
        LPCSTR szPathStr = _TEXT("PATH");
        DWORD dwEnvSize = GetEnvironmentVariable(szPathStr, "", 0);
        LPSTR lpPathEnvVar = (LPSTR)malloc(dwEnvSize);
        GetEnvironmentVariable(szPathStr, lpPathEnvVar, dwEnvSize);

        // Build the new VC Win32 target platform path
        LPSTR lpNewPath = (LPSTR)malloc(dwEnvSize + 5 * MAX_PATH);
        *lpNewPath = 0;

        // Add in the Xbox parts of the path
        strcat(lpNewPath, lpDefaultDir);
        ::AppendSlash(lpNewPath);
        strcat(lpNewPath, "Xbox\\Bin\\VC7;");
        strcat(lpNewPath, lpDefaultDir);
        ::AppendSlash(lpNewPath);
        strcat(lpNewPath, "Xbox\\Bin;");

        // Add in the VC parts
        if (lpMSDEVPath)
        {
            strcat(lpNewPath, lpMSDEVPath);
            ::AppendSlash(lpNewPath);
            strcat(lpNewPath, "..\\Common\\MSDev98\\Bin;");
            strcat(lpNewPath, lpMSDEVPath);
            ::AppendSlash(lpNewPath);
            strcat(lpNewPath, "Bin;");
            strcat(lpNewPath, lpMSDEVPath);
            ::AppendSlash(lpNewPath);
            strcat(lpNewPath, "..\\Common\\Tools;");
            free(lpMSDEVPath);
        }

        // Add in the path from the environment variable
        strcat(lpNewPath, lpPathEnvVar);
        free(lpPathEnvVar);

        // All done, write out the registry string
        WriteRegistryString(HKEY_CURRENT_USER, lpXboxDirectories, lpPathDirs, lpNewPath);
        free(lpNewPath);
    }
    if (lpXboxPlatformPath)
        free(lpXboxPlatformPath);
    if (lpWin32PlatformPath)
        free(lpWin32PlatformPath);

    // Just like we did for the "Path Dirs," do for the "Include Dirs"
    LPCSTR lpIncludeDirs = _TEXT("Include Dirs");
    LPSTR lpXboxPlatformInclude = GetRegistryStringAlloc(HKEY_CURRENT_USER, lpXboxDirectories, lpIncludeDirs);
    if (lpXboxPlatformInclude)
    {
        // Path exists, include the Xbox directories
        LPSTR lpPath = PrependPathElement(lpXboxPlatformInclude, _TEXT("Xbox\\Include"));
        WriteRegistryString(HKEY_CURRENT_USER, lpXboxDirectories, lpIncludeDirs, lpPath);
        free(lpPath);
        free(lpXboxPlatformInclude);
    }

    // Otherwise build a new one from scratch
    else
    {
        TCHAR szIncludePath[MAX_PATH];
        strcpy(szIncludePath, lpDefaultDir);
        ::AppendSlash(szIncludePath);
        strcat(szIncludePath, _TEXT("Xbox\\Include"));
        WriteRegistryString(HKEY_CURRENT_USER, lpXboxDirectories, lpIncludeDirs, szIncludePath);
    }

    // And for the "Lib dirs"
    LPCSTR lpLibDirs = _TEXT("Library Dirs");
    LPSTR lpXboxPlatformLib = GetRegistryStringAlloc(HKEY_CURRENT_USER, lpXboxDirectories, lpLibDirs);
    if (lpXboxPlatformLib)
    {
        // Path exists, include the Xbox directories
        LPSTR lpPath = PrependPathElement(lpXboxPlatformLib, _TEXT("Xbox\\Lib"));
        WriteRegistryString(HKEY_CURRENT_USER, lpXboxDirectories, lpLibDirs, lpPath);
        free(lpPath);
        free(lpXboxPlatformLib);
    }

    // Otherwise, build a new one from scratch
    else
    {
        TCHAR szLibPath[MAX_PATH];
        strcpy(szLibPath, lpDefaultDir);
        ::AppendSlash(szLibPath);
        strcat(szLibPath, _TEXT("Xbox\\Lib"));
        WriteRegistryString(HKEY_CURRENT_USER, lpXboxDirectories, lpLibDirs, szLibPath);
    }

    // And for the "source dirs"
    LPCSTR lpSourceDirs = _TEXT("Source Dirs");
    LPSTR lpXboxPlatformSource = GetRegistryStringAlloc(HKEY_CURRENT_USER, lpXboxDirectories, lpSourceDirs);
    if (lpXboxPlatformSource)
    {
        // Path exists, include the Xbox directories
        LPSTR lpPath = PrependPathElement(lpXboxPlatformSource, _TEXT("Source"));
        WriteRegistryString(HKEY_CURRENT_USER, lpXboxDirectories, lpSourceDirs, lpPath);
        free(lpPath);
        free(lpXboxPlatformSource);
    }

    // Otherwise, build a new one from scratch
    else
    {
        TCHAR szSourcePath[MAX_PATH];
        strcpy(szSourcePath, lpDefaultDir);
        ::AppendSlash(szSourcePath);
        strcat(szSourcePath, _TEXT("Source"));
        WriteRegistryString(HKEY_CURRENT_USER, lpXboxDirectories, lpSourceDirs, szSourcePath);
    }

    // Add our directories to the Win32 platform as well
    LPSTR lpWin32PlatformInclude = GetRegistryStringAlloc(HKEY_CURRENT_USER, lpWin32Directories, lpIncludeDirs);
    if (lpWin32PlatformInclude)
    {
        // Path exists, include the Xbox directories
        LPSTR lpPath = PrependPathElement(lpWin32PlatformInclude, _TEXT("Include"));
        WriteRegistryString(HKEY_CURRENT_USER, lpWin32Directories, lpIncludeDirs, lpPath);
        free(lpPath);
        free(lpWin32PlatformInclude);
    }

    // And for the "Lib dirs" (Win32)
    LPSTR lpWin32PlatformLib = GetRegistryStringAlloc(HKEY_CURRENT_USER, lpWin32Directories, lpLibDirs);
    if (lpWin32PlatformLib)
    {
        // Path exists, include the Xbox directories
        LPSTR lpPath = PrependPathElement(lpWin32PlatformLib, _TEXT("Lib"));
        WriteRegistryString(HKEY_CURRENT_USER, lpWin32Directories, lpLibDirs, lpPath);
        free(lpPath);
        free(lpWin32PlatformLib);
    }

    // Update the system path
    LPCSTR lpRegEnvironmentKey = _TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment");
    LPCSTR lpRegEnvironmentValue = _TEXT("PATH");
    LPSTR lpEnvironmentPath = GetRegistryStringAlloc(HKEY_LOCAL_MACHINE, lpRegEnvironmentKey, lpRegEnvironmentValue);
    if (lpEnvironmentPath == NULL)
        lpEnvironmentPath = _strdup("");
    LPSTR lpNewPath = PrependPathElement(lpEnvironmentPath, _TEXT("Xbox\\Bin"));
    free(lpEnvironmentPath);
    if (RegOpenKey(HKEY_LOCAL_MACHINE, lpRegEnvironmentKey, &hkey) == ERROR_SUCCESS)
    {
        RegSetValueEx(hkey, lpRegEnvironmentValue, 0, REG_EXPAND_SZ, (LPBYTE)lpNewPath, strlen(lpNewPath) + 1);

        // Make an XDK env. variable with the path to the XDK in it
        RegSetValueEx(hkey, _TEXT("XDK"), 0, REG_EXPAND_SZ, (LPBYTE)lpDefaultDir, strlen(lpDefaultDir) + 1);
        RegCloseKey(hkey);
    }
    free(lpNewPath);

    // Send the message so the new environment variables are picked up
    LPCSTR lpEnvFlag = _TEXT("Environment");
    SendMessage(HWND_BROADCAST, WM_WININICHANGE, 0, (LPARAM)lpEnvFlag);
}

//---------------------------------------------------------------------
//  CXdkCustom methods

//  CXdkCustom::DoPreSetupChecks
//      Here is where we have a chance in code to stop setup because
//      something isn't right. We need to test that we're not installing
//      an older build over a newer one, make sure that the SPs we
//      insist on are already installed, and that nothing we depend on is running.
//      It's important to call XdkLastError with an appropriate error message
//      before returning FALSE.

BOOL
CXdkCustom::DoPreSetupChecks()
{
    // Check to make sure MSDEV is installed
    if (!CheckForMSDev() || !CheckForSPAndProcPack())
        return FALSE;

    // Check to make sure any XDK tools aren't running
    if (!CheckForProgramInUse("xbExplorer.exe") ||
        !CheckForProgramInUse("xbWatson.exe"))
        return FALSE;

    return TRUE;
}


//  CXdkCustom::CustomDirectory
//      Here is where we're given a chance to see and/or update target directories
//      as they get set. Normally, the target directory roots are set by
//      looking up regkeys or by prompting the user for one default directory.
//      This callback is called for each directory target. If any are still empty
//      (and any files are in the manifest pointing to it), the engine will fail,
//      so some error should be displayed here or the empty directory should
//      be resolved somehow. This function will be called once for each target
//      directory.
//      If you take the (radical) step of returning FALSE here, this means that
//      we jump to the error wizard page and terminate setup. As with DoPreSetupChecks,
//      make sure that you've called WizSetLastError with a descriptive error message.

BOOL
CXdkCustom::ValidateDirectory(
    LPCSTR lpDefaultDir,    // Default dir: set on upgrade or by the user
    LPCSTR lpID,            // Target directory ID string. Unique.
    LPSTR lpCommand,        // Command that was used to fill in the target path
    LPSTR lpData,           // Data used to fill in the target path (after : in .INI file)
    LPSTR lpPath            // Path files will be copied to with this ID
    )
{
    return TRUE;
}


//  CXdkCustom::AfterCopy
//      Called just after copying files are complete. Gives a chance to write regkeys
//      and still add things to the uninstall information.

VOID
CXdkCustom::AfterCopy()
{
    // Make sure that MSDEV has the right paths set for the Xbox target type
    MungeMSDEVPaths();
}


//  CXdkCustom::PostSetup
//      Called when setup is complete and the UI has been dismissed.
//      Gives a chance to do any last actions before exiting.

VOID
CXdkCustom::PostSetup(int /*nRet*/)
{
    // Start up the release notes. These will appear after setup goes away
    // This is hardcoded and pretty yucky.
    TCHAR szPath[MAX_PATH];
    strcpy(szPath, g_pCustomSite->GetDefaultDir());
    AppendSlash(szPath);
    strcat(szPath, _TEXT("relnotes.htm"));
    ShellExecute(NULL, NULL, szPath, NULL, NULL, SW_SHOWNORMAL); 
}

VOID
StripPathComponent(
    LPCTSTR pszUninstallPath,
    HKEY hkeyroot,
    LPCTSTR lpSubkey,
    LPCTSTR lpValuename
    );

void __stdcall PostUninstall(LPCTSTR pszUninstallPath)
{
    // Whack the msdev xbox platform key
    LPCSTR lpXboxPlatform = TEXT("Software\\Microsoft\\Devstudio\\6.0\\Build System\\Components\\Platforms\\Xbox");
    SHDeleteKey(HKEY_CURRENT_USER, lpXboxPlatform);

    // Whack out msdev and environment path information
    LPCSTR lpWin32Directories = TEXT("Software\\Microsoft\\Devstudio\\6.0\\Build System\\Components\\Platforms\\Win32 (x86)\\Directories");
    LPCSTR lpPathDirs = TEXT("Path Dirs");
    LPCSTR lpIncludeDirs = TEXT("Include Dirs");
    LPCSTR lpLibDirs = TEXT("Library Dirs");
    LPCSTR lpRegEnvironmentKey = TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment");
    LPCSTR lpRegEnvironmentValue = TEXT("PATH");
    StripPathComponent(pszUninstallPath, HKEY_CURRENT_USER, lpWin32Directories, lpIncludeDirs);
    StripPathComponent(pszUninstallPath, HKEY_CURRENT_USER, lpWin32Directories, lpLibDirs);
    StripPathComponent(pszUninstallPath, HKEY_LOCAL_MACHINE, lpRegEnvironmentKey, lpRegEnvironmentValue);
    StripPathComponent(pszUninstallPath, HKEY_CURRENT_USER, lpRegEnvironmentKey, lpRegEnvironmentValue);

    // Nuke the XDK key in the environment
    HKEY hkey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRegEnvironmentKey, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS)
    {
        RegDeleteValue(hkey, "XDK");
        RegCloseKey(hkey);
    }

    // Send the message to say that the environment has changed
    LPCSTR lpEnvFlag = TEXT("Environment");
    SendMessage(HWND_BROADCAST, WM_WININICHANGE, 0, (LPARAM)lpEnvFlag);
}

BOOL __stdcall PreUninstall(LPCTSTR pszUninstallPath)
{
    // If MSDev, xbExplorer.exe, xbWatson.exe, 
    //
    if (!CheckForMSDev()||
        !CheckForProgramInUse("xbExplorer.exe") ||
        !CheckForProgramInUse("xbWatson.exe"))
        return FALSE;

    return TRUE;
}

//  StripPathComponent
//      Removes a path component that matches the default directory being uninstalled

VOID
StripPathComponent(
    LPCTSTR pszUninstallPath,
    HKEY hkeyroot,
    LPCTSTR lpSubkey,
    LPCTSTR lpValuename
    )
{
    HKEY hkey;
    DWORD dwType;
    DWORD dwcOut;
    LPSTR lpPath;
    DWORD dwcDir = strlen(pszUninstallPath);
    DWORD dwRet;

    // Open the key
    if (::RegOpenKeyEx(hkeyroot, lpSubkey, 0, KEY_ALL_ACCESS, &hkey) != ERROR_SUCCESS)
        return;

    // Get the length of the value
    if (::RegQueryValueEx(hkey, lpValuename, NULL, &dwType, NULL, &dwcOut) != ERROR_SUCCESS)
        return;

    // Allocate some memory to read the value into
    lpPath = (LPSTR)malloc(dwcOut);
    if (!lpPath)
        return;

    // Do the read
    dwRet = ::RegQueryValueEx(hkey, lpValuename, NULL, &dwType, (LPBYTE)lpPath, &dwcOut);
    if (dwRet != ERROR_SUCCESS || (dwType != REG_SZ && dwType != REG_EXPAND_SZ))
    {
        ::RegCloseKey(hkey);
        free(lpPath);
        return;
    }

    LPSTR lpNewPath = (LPSTR)malloc(dwcOut);
    LPSTR lpNew = lpNewPath;
    LPSTR lp = lpPath;

    // Walk through the path components, finding any piece that matches the install directory
    // Search for this component
    while (*lp)
    {
        // Did we find the component?
        if (_strnicmp(lp, pszUninstallPath, dwcDir) == 0)
        {
            // Scan to the next component, skipping it
            for (; *lp && *lp != ';' ; ++lp)
                ;
            if (*lp == ';')
                ++lp;
        }

        // Otherwise, copy this component
        else
        {
            while (*lp && *lp != ';')
                *lpNew++ = *lp++;
            if (*lp == ';')
                *lpNew++ = *lp++;
        }
    }
    *lpNew = 0;

    // Write the key back out
    ::RegSetValueEx(hkey, lpValuename, 0, REG_EXPAND_SZ, (LPBYTE)lpNewPath, strlen(lpNewPath) + 1);
    ::RegCloseKey(hkey);
    free(lpPath);
    free(lpNewPath);
}



