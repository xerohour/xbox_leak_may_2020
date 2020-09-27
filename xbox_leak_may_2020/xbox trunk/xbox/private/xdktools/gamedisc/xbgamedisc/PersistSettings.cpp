// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      PersistSettings.cpp
// Contents:  Functionality to read and write default values from the
//            registry.  This includes window locations, user options, etc.
// Revisions: 28-Jan-2002: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- precompiled header file
#include "stdafx.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// REGISTRY_SUBKEY  -- The key that contains xbGameDisc default values.
#define REGISTRY_SUBKEY	"Software\\Microsoft\\XboxSDK\\xbGameDisc"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GetRegistryDWORD
// Purpose:   Gets a REG_DWORD value from the registry.  If not found, then
//            the specified default value is returned.
// Arguments: szKey          -- Registry key to get.
//            dwDefaultValue -- Default value to return if key isn't found.
// Return:    Key value (or dwDefaultValue if key isn't found)
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD GetRegistryDWORD(char *szKey, DWORD dwDefaultValue)
{
	HKEY hKey;
	DWORD dwType = REG_DWORD, dwRet, dwSize;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, REGISTRY_SUBKEY, 0, KEY_READ, &hKey))
        return dwDefaultValue;
    
	if (RegQueryValueEx(hKey, szKey, NULL, &dwType, (LPBYTE) &dwRet, &dwSize)
        || dwSize != 4)
    {
        RegCloseKey(hKey);
        return dwDefaultValue;
    }

	RegCloseKey(hKey);
	return dwRet;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  SetRegistryDWORD
// Purpose:   Sets a REG_DWORD value in the registry.
// Arguments: szKey     -- Registry key to set.
//            dwValue   -- Value to set key to.
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void SetRegistryDWORD(char *szKey, DWORD dwValue)
{
	HKEY hKey;
    DWORD dw;
	RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_SUBKEY, 0, "",
                   REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dw);
	RegSetValueEx(hKey, szKey, 0, REG_DWORD, (CONST LPBYTE) &dwValue,
                  sizeof(DWORD));
    RegCloseKey(hKey);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  InitGlobalVars
// Purpose:   Obtain default values from registry for window locations et al.
//            Force particular default values if not found in the registry.
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void InitGlobalVars()
{
    g_nWindowX = GetRegistryDWORD("nWindowX", 100);
    g_nWindowY = GetRegistryDWORD("nWindowY", 100);
    g_nWindowW = GetRegistryDWORD("nWindowW", 600);
    g_nWindowH = GetRegistryDWORD("nWindowH", 700);
    g_nCmdShow = GetRegistryDWORD("WindowShow", SW_SHOW);

    g_nSplitterX    = GetRegistryDWORD("nSplitterX", 300);
    g_nSplitterX2   = GetRegistryDWORD("nSplitterX2", 300);
    g_fDoStartupDlg = GetRegistryDWORD("fDoStartupDlg", TRUE) ? TRUE : FALSE;

    g_nUnplacedWindowX = GetRegistryDWORD("nUnplacedWindowX", 200);
    g_nUnplacedWindowY = GetRegistryDWORD("nUnplacedWindowY", 200);
    g_nUnplacedWindowW = GetRegistryDWORD("nUnplacedWindowW", 500);
    g_nUnplacedWindowH = GetRegistryDWORD("nUnplacedWindowH", 600);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  StoreGlobalVars
// Purpose:   Store window locations (et al) in registry for next run.
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void StoreGlobalVars()
{
    WINDOWPLACEMENT wp;
    wp.length = sizeof wp;
    GetWindowPlacement(g_hwndMain, &wp);

    SetRegistryDWORD("nWindowX", wp.rcNormalPosition.left);
    SetRegistryDWORD("nWindowY", wp.rcNormalPosition.top);
    SetRegistryDWORD("nWindowW", wp.rcNormalPosition.right - wp.rcNormalPosition.left);
    SetRegistryDWORD("nWindowH", wp.rcNormalPosition.bottom - wp.rcNormalPosition.top);
    if (wp.showCmd != SW_MAXIMIZE)
        SetRegistryDWORD("WindowShow", SW_SHOW);
    else
        SetRegistryDWORD("WindowShow", wp.showCmd);

    SetRegistryDWORD("nSplitterX",    g_nSplitterX);
    SetRegistryDWORD("nSplitterX2",   g_nSplitterX2);
    SetRegistryDWORD("fDoStartupDlg", g_fDoStartupDlg);

    SetRegistryDWORD("nUnplacedWindowX", g_nUnplacedWindowX);
    SetRegistryDWORD("nUnplacedWindowY", g_nUnplacedWindowY);
    SetRegistryDWORD("nUnplacedWindowW", g_nUnplacedWindowW);
    SetRegistryDWORD("nUnplacedWindowH", g_nUnplacedWindowH);

}

