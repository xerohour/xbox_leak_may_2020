/*++

Copyright (c) Microsoft Corporation

Module Name:

    xbshlext.cpp

Abstract:

   Implementation of DLL Exports.
   Mostly ripped from ATL wizard generated code.

Environment:

    Windows 2000 and Later 
    User Mode
    Compiles UNICODE, but uses many ANSI APIs explictly.

Revision History:
    
    07-03-2001 : created

--*/


#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "xbshlext.h"
//#include "xbshlext_i.c"

//-------------------------------------------------------------------------------
//  ATL Module and Object 
//-------------------------------------------------------------------------------
CComModule _Module;
BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_XboxFolder, CXboxRoot)
END_OBJECT_MAP()

//-------------------------------------------------------------------------------
//  Global pointer to shell's IMalloc
//-------------------------------------------------------------------------------
IMalloc *g_pShellMalloc = NULL;

//-------------------------------------------------------------------------------
// DLL Entry Point - Standard ATL Implemenation.
//-------------------------------------------------------------------------------
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_SHELLEXTLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

//-------------------------------------------------------------------------------
//  In addition to ATL stuff, do Just In Time cleanup, if necessary.
//-------------------------------------------------------------------------------
STDAPI DllCanUnloadNow(void)
{
    //
    //  See if we can unload
    //
    HRESULT hr;
    hr = (_Module.GetLockCount()==0) ? S_OK : S_FALSE;

    //
    //  Cleanup a few things if we are going to return S_OK;
    //
    if(S_OK==hr)
    {
        //
        //  Release the shell's IMalloc
        //
        if(g_pShellMalloc)
        {
            g_pShellMalloc->Release();
            g_pShellMalloc = NULL;
        }
    } else
    {
        DUMP_TRACKED_OBJECTS();
    }
    return hr;
}

//-------------------------------------------------------------------------------
// Returns a class factory to create an object of the requested type
//-------------------------------------------------------------------------------
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    HRESULT hr = S_OK;
    //
    //  Basically, everyone needs the shell's IMalloc, so just keep a global.
    //  Refuse to create any objects if we cannot get one.
    //
    if(!g_pShellMalloc)
    {
        hr = SHGetMalloc(&g_pShellMalloc);
    }
    if(SUCCEEDED(hr))
    {
        hr = _Module.GetClassObject(rclsid, riid, ppv);
    }
    return hr;
}

/*  UpdateShellImages which used GetIconSize and SetIconSize.
 *
 *  Don't bother:
 *
 *  This didn't work all that well, because the most common upgrade scenario ends up
 *  calling DllRegisterServer during a reboot, when the desktop isn't running, and then
 *  this code doesn't work!  So the user is still stuck with the wrong icons.  We sold out
 *  and fixed the brute force way, by adding placeholder icons to take up the icon indices
 *  the shell had already cached.
 *
 *  It caused an ugly flash anyway!
 *
int GetIconSize()
{
    TCHAR ach[20];
    DWORD cb = sizeof(ach);
    DWORD dwType;
    
    // Get the icon size from GetSystemMetrics
    int cxIcon = GetSystemMetrics(SM_CXICON);

    //But prefer to get it from the registry
    if(ERROR_SUCCESS == SHGetValue(
                            HKEY_CURRENT_USER,
                            TEXT("Control Panel\\Desktop\\WindowMetrics"),
                            TEXT("Shell Icon Size"),
                            &dwType,
                            (LPVOID)ach,
                            &cb))
    {
        _ASSERTE(dwType==REG_SZ);
        if( (ach[0] >= TEXT('0')) && (ach[0] <= TEXT('9')))
            return _ttoi(ach);
    }
    
    return cxIcon;
    
}

void SetIconSize(int iSize)
{
    TCHAR ach[20];
    _ltot(iSize, ach, 10);
    SHSetValue(
        HKEY_CURRENT_USER,
        TEXT("Control Panel\\Desktop\\WindowMetrics"),
        TEXT("Shell Icon Size"),
        REG_SZ,
        (LPVOID)ach,
        _tcslen(ach)*sizeof(TCHAR));
}

void UpdateShellImages()
{
    ICONMETRICS iconMetrics;
    iconMetrics.cbSize = sizeof(iconMetrics);

    // The principle here is that we can force the shell to rebuild its image list
    // if we change the shell icon size, or color depth in the registry, and then
    // call FileIconInit(BOOL fRestoreCache = TRUE). This is an evil hack, BUT we
    // cannot be first, and we certainly won't be the last to do this.  Unfortunately,
    // we need to preserve the size, so we need to do this it twice.
    
    int iOrgSize = GetIconSize();
    int iNewSize = iOrgSize-1;
    SetIconSize(iNewSize);
    //Found this in Desktop Settings Code, it is only partially documented, but it is all in public headers.
    //Will force icon updates
    SystemParametersInfo(SPI_GETICONMETRICS, sizeof(iconMetrics), &iconMetrics, 0);
    SystemParametersInfo(SPI_SETICONMETRICS, sizeof(iconMetrics), &iconMetrics, SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
    SetIconSize(iOrgSize);
    SystemParametersInfo(SPI_GETICONMETRICS, sizeof(iconMetrics), &iconMetrics, 0);
    SystemParametersInfo(SPI_SETICONMETRICS, sizeof(iconMetrics), &iconMetrics, SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
}*/


//-------------------------------------------------------------------------------
// DllRegisterServer - Adds entries to the system registry
//-------------------------------------------------------------------------------
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _Module.RegisterServer();
    
    if(SUCCEEDED(hr))
    {
        //
        // notify the shell that we installed a new namespace.
        //
        SHChangeNotify(SHCNE_MKDIR, SHCNF_PATH|SHCNF_FLUSH, ROOT_GUID_NAME_WIDE, NULL);
        
        //
        //  Create a shortcut on the start menu (don't fail registration if this
        //  doesn't work.)
        //
        
        HRESULT hres; //Different error value, we don't return this one.
        IShellLinkW* psl; 
        // Get a pointer to the IShellLink interface. 
        hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID *) &psl); 
        if (SUCCEEDED(hres))
        { 
            IPersistFile* ppf; 
            WCHAR wszShortcutPath[MAX_PATH];
            WCHAR wszShortcutName[40];
            WCHAR wszShortcutDescription[60];
            LoadStringW(_Module.GetModuleInstance(), IDS_ROOT_SHORT_CUT_NAME, wszShortcutName, ARRAYSIZE(wszShortcutName));
            LoadStringW(_Module.GetModuleInstance(), IDS_ROOT_SHORT_CUT_DESCRIPTION, wszShortcutDescription, ARRAYSIZE(wszShortcutDescription));

            // Set the path to the shortcut target and add the 
            // description. 
            psl->SetPath(ROOT_GUID_NAME_WIDE); 
            psl->SetDescription(wszShortcutDescription); 
            
            hres = SHGetFolderPathW( NULL, CSIDL_COMMON_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, wszShortcutPath);
            if(SUCCEEDED(hres))
            {
                wcscat(wszShortcutPath, wszShortcutName);
                // Query IShellLink for the IPersistFile interface for saving the 
                // shortcut in persistent storage. 
                hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
                if (SUCCEEDED(hres))
                { 
                    // Save the link by calling IPersistFile::Save. 
                    hres = ppf->Save(wszShortcutPath, TRUE); 
                    ppf->Release(); 
                }
            }
            psl->Release();

        }
        //UpdateShellImages();
    }
    return hr;
}

//-------------------------------------------------------------------------------
// DllUnregisterServer - Removes entries from the system registry
//-------------------------------------------------------------------------------
STDAPI DllUnregisterServer(void)
{
    HRESULT hr = _Module.UnregisterServer(TRUE);
    
    if(SUCCEEDED(hr))
    {
        //Remove the shortcut
        HRESULT hres;
        WCHAR wszShortcutPath[MAX_PATH];
        hres = SHGetFolderPathW( NULL, CSIDL_COMMON_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, wszShortcutPath);
        if(SUCCEEDED(hres))
        {
            WCHAR wszShortcutName[40];
            LoadStringW(_Module.GetModuleInstance(), IDS_ROOT_SHORT_CUT_NAME, wszShortcutName, ARRAYSIZE(wszShortcutName));
            wcscat(wszShortcutPath, wszShortcutName);
            DeleteFileW(wszShortcutPath);  // The use may have deleted it already."
        }
        // Tell the shell that we removed the shell extension
        SHChangeNotify(SHCNE_RMDIR, SHCNF_PATH|SHCNF_FLUSH, ROOT_GUID_NAME_WIDE, NULL);
    }
    return hr;
}

HRESULT WINAPI CXboxFolder::UpdateRegistry(BOOL bRegister)
/*++
  Routine Description:
    
    Modifies the normal ATL class registration.  This is how to create more
    map entries for script susbstitution.  We create three:

    RUNDLLPATH    - The full path to rundll32.exe
    FIRST_ARG     - "%1" the script sees % as a special character, so a subst
                    is necessary to get this through.
    XBOX_HTT_PATH - Path to xbox.htt - assumed to be in the same place as this
                    module.

    Then we just run the script our of our resource.

  Comments:

    It is a static member of CXboxFolder, because ATL registers classes not
    servers.

--*/
{
    USES_CONVERSION;
    TCHAR szRunDllBuffer[_MAX_PATH];
    TCHAR szXboxHttBuffer[_MAX_PATH];

    //Allocate and terminate the regMapEntries
    _ATL_REGMAP_ENTRY regMapEntries[4];
    memset(&regMapEntries[3], 0, sizeof(_ATL_REGMAP_ENTRY)); //terminate the list

    //Fill out the first entry for RUNDLLPATH
    HRESULT hr = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_DEFAULT, szRunDllBuffer);
    _ASSERTE(SUCCEEDED(hr));
    ULONG ulSystemDirLen = _tcslen(szRunDllBuffer);
    _tcscpy(szRunDllBuffer + ulSystemDirLen, TEXT("\\rundll32.exe"));
    regMapEntries[0].szKey = OLESTR("RUNDLLPATH");
    regMapEntries[0].szData = T2OLE(szRunDllBuffer);

    //Fill out the second entry for %1 (this crap is necessary because the .rgs
    //script is way to simplist, with no escape sequences.
    regMapEntries[1].szKey = OLESTR("FIRST_ARG");
    regMapEntries[1].szData = OLESTR("%1");

    //Fill out the XBOX_HTT_PATH
    GetModuleFileName(_Module.GetModuleInstance(), szXboxHttBuffer, ARRAYSIZE(szXboxHttBuffer));
    _tcscpy(_tcsrchr(szXboxHttBuffer, '\\') + 1, TEXT("xbox.htt"));
    regMapEntries[2].szKey = OLESTR("XBOX_HTT_PATH");
    regMapEntries[2].szData = T2OLE(szXboxHttBuffer);

    return _Module.UpdateRegistryFromResource(IDR_XBOXFOLDER, bRegister, regMapEntries);
}

void CALLBACK LaunchExplorer(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
/*++
 Routine Description:
   The pluggable protocol stuff doesn't work all that well, because explorer cannot
   navigate to our pidl.  Somehow it finds our class, and we get to parse the display
   name (successfully!), but it is expecting our pidl to be some special case URL thing,
   which it is not.  The shell guys are useless here.  As Buzz put it, "I mention 'pluggable
   protocol' and people run the other way."

   However, 'explorer.exe /e,::{OUR CLSID}\<path to folder>' works just find.  So, rather than
   launch explorer, our pluggable protocol launches RunDLL32.exe, which calls this
   entry point.  Here we turn xbox://<path to folder>/ into ::{OUR CLSID}\<path to folder and
   we call explorer with it using WinExec.

   Note that we can do other neat things here.  Like:
     1) If a file is specified redirect to the parent folder.
--*/
{
  const char *szProtocolName = "xbox://";
  const int   szProtocolNameLen = (sizeof("xbox://")/sizeof(char))-1;
  const char *szCommandFormat = "explorer.exe ::{DB15FEDD-96B8-4DA9-97E0-7E5CCA05CC44}\\%s";
  char szCommand[1024];

  if(0==strncmp(szProtocolName, lpszCmdLine, szProtocolNameLen))
  {
    lpszCmdLine += szProtocolNameLen;
    int CmdLineLen = strlen(lpszCmdLine);
    if('/'==lpszCmdLine[CmdLineLen-1])
    {   
        lpszCmdLine[CmdLineLen-1] = '\0';
    }
    wsprintfA(szCommand, szCommandFormat, lpszCmdLine);
    WinExec(szCommand, nCmdShow);
  } else
  {
    MessageBoxA(hwnd, lpszCmdLine, "LaunchExplorer - Bad Args", MB_OK|MB_ICONERROR);
  }
}


