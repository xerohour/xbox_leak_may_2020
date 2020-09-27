//  HELPER.H
//
//      Header for common miscellaneous helper routines in HELPER.CPP
//
//  Created 9-May-2001 [JonT]

#ifndef _HELPER_H_
#define _HELPER_H_

BOOL GetRegistryString(LPTSTR lpOut, DWORD dwcOut, HKEY hkeyroot, LPCTSTR lpSubkey, LPCTSTR lpValuename);
LPSTR GetRegistryStringAlloc(HKEY hkeyroot, LPCTSTR lpSubkey, LPCTSTR lpValuename);
BOOL WriteRegistryString(HKEY hkeyroot, LPCTSTR lpSubkey, LPCTSTR lpValuename, LPCTSTR lpValue);
BOOL CompareFileVersion(LPTSTR lpPath, DWORD dwVerMS, DWORD dwVerLS, int* lpnResult);
BOOL IsAdministrator(void);
BOOL RegisterDLL(LPCTSTR lpPath, BOOL fUnregister);
BOOL RegisterDLLOnReboot(LPCTSTR lpPath);
int rsprintf(HINSTANCE hInstance, LPSTR pszDest, UINT uFormatResource, ...);
void FormatWindowText(HWND hWnd, ...);
BOOL Reboot();


#endif // #ifndef _HELPER_H_
