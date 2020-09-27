//  HELPER.CPP
//
//      Common file and registry helpers to make installation tasks simpler
//
//  Created 19-Apr-2001 [JonT]

#include "common.h"

//  GetRegistryString
//      Reads a registry value into the string passed in. Assumes the string is at
//      least MAX_PATH in length. Returns FALSE if the key/value is not found. Pass
//      in NULL for the string to get the default value.

BOOL
GetRegistryString(
    LPTSTR lpOut,
    DWORD dwcOut,
    HKEY hkeyroot,
    LPCTSTR lpSubkey,
    LPCTSTR lpValuename
    )
{
    HKEY hkey;
    DWORD dwType;
    DWORD dwRet;

    // Open the key and get the value
    if (::RegOpenKeyEx(hkeyroot, lpSubkey, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
        return FALSE;
    dwRet = ::RegQueryValueEx(hkey, lpValuename, NULL, &dwType, (PBYTE)lpOut, &dwcOut);
    ::RegCloseKey(hkey);

    // If we got back a DWORD, convert nicely to a string
    if (dwType == REG_DWORD)
    {
        DWORD dw = *(DWORD*)lpOut;
        sprintf(lpOut, "%d", dw);
        dwType = REG_SZ;
    }

    return (dwRet == ERROR_SUCCESS && dwType == REG_SZ);
}


//  GetRegistryStringAlloc
//      Reads a registry string after allocating a buffer just the right size for it.
//      Returns NULL if the regvalue doesn't exist. Caller's responsibility to free
//      the string.

LPSTR
GetRegistryStringAlloc(
    HKEY hkeyroot,
    LPCTSTR lpSubkey,
    LPCTSTR lpValuename
    )
{
    HKEY hkey;
    DWORD dwType;
    DWORD dwRet;
    LPSTR lpRet;
    DWORD dwcOut;

    // Open the key
    if (::RegOpenKeyEx(hkeyroot, lpSubkey, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
        return NULL;

    // Get the length of the value
    if (::RegQueryValueEx(hkey, lpValuename, NULL, &dwType, NULL, &dwcOut) != ERROR_SUCCESS)
        return NULL;

    // Allocate some memory to read the value into
    lpRet = (LPSTR)malloc(dwcOut);
    if (!lpRet)
        return NULL;

    // Do the read
    dwRet = ::RegQueryValueEx(hkey, lpValuename, NULL, &dwType, (LPBYTE)lpRet, &dwcOut);
    ::RegCloseKey(hkey);

    if (dwRet != ERROR_SUCCESS)
    {
        free(lpRet);
        return NULL;
    }

    // If we got back a DWORD, convert to a string
    if (dwType == REG_DWORD)
    {
        DWORD dw = *(DWORD*)lpRet;
        free(lpRet);
        lpRet = (LPSTR)malloc(12);
        sprintf(lpRet, "%d", dw);
        dwType = REG_SZ;
    }

    return lpRet;
}


//  WriteRegistryString
//      Writes a registry string out

BOOL
WriteRegistryString(
    HKEY hkeyroot,
    LPCTSTR lpSubkey,
    LPCTSTR lpValuename,
    LPCTSTR lpValue
    )
{
    HKEY hkey;
    LONG lRet;

    // Open the key
    if (::RegCreateKeyEx(hkeyroot, lpSubkey, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, NULL) != ERROR_SUCCESS)
        return FALSE;

    // Set the value
    lRet = ::RegSetValueEx(hkey, lpValuename, 0, REG_SZ, (LPBYTE)lpValue, strlen(lpValue) + 1);
    ::RegCloseKey(hkey);

    return (lRet == ERROR_SUCCESS);
}


//  CompareFileVersion
//      Compares a file's version info with that passed in.
//      Returns FALSE if the file cannot be found or the version information can't be read.
//      Returns TRUE otherwise and places the following values in *lpnResult:
//          -1 if the file is a version less than that passed in,
//          0 if the file is the same version and
//          1 if the file is a greater (later) version.

BOOL
CompareFileVersion(
    LPTSTR lpPath,
    DWORD dwVerMS,
    DWORD dwVerLS,
    int* lpnResult
    )
{
    DWORD dwZero;
    DWORD dwVerSize = ::GetFileVersionInfoSize(lpPath, &dwZero);

    // Make sure the file can be located
    if (dwVerSize == 0)
        return FALSE;

    // Allocate memory for the version block
    PBYTE pbVerInfo = (PBYTE)malloc(dwVerSize);
    if (pbVerInfo == NULL)
        return FALSE;

    // Get the version info for the file in question
    UINT uLen;
    VS_FIXEDFILEINFO* pVer;
    if (::GetFileVersionInfo(lpPath, 0, dwVerSize, pbVerInfo) == FALSE ||
        ::VerQueryValue(pbVerInfo, TEXT("\\"), (LPVOID*)&pVer, &uLen) == FALSE)
    {
        free(pbVerInfo);
        return FALSE;
    }

    // Check most significant DWORD
    if (pVer->dwFileVersionMS > dwVerMS)
        *lpnResult = 1;
    else if (pVer->dwFileVersionMS < dwVerMS)
        *lpnResult = -1;

    // Otherwise, the most significant is equal, so we have to decide based on the least significant
    else 
    {
        if (pVer->dwFileVersionLS > dwVerLS)
            *lpnResult = 1;
        else if (pVer->dwFileVersionLS < dwVerLS)
            *lpnResult = -1;
        else
            *lpnResult = 0;
    }
    free(pbVerInfo);
    return TRUE;
}


//  IsAdministrator
//      Returns TRUE iff the current user belongs to the administrator group

BOOL
IsAdministrator(void)
{
    HANDLE htoken;
    TOKEN_GROUPS* ptg = NULL;
    DWORD  cbTokenGroups;
    DWORD  dwGroup;
    SID_IDENTIFIER_AUTHORITY SystemSidAuthority= SECURITY_NT_AUTHORITY;
    PSID   psidAdmin;
    BOOL fAdmin;

    // First we must open a handle to the access token for this thread.
    if (!OpenThreadToken ( GetCurrentThread(), TOKEN_QUERY, FALSE, &htoken))
    {
        if (GetLastError() == ERROR_NO_TOKEN)
        {
            // If the thread does not have an access token, we'll examine the
            // access token associated with the process.
            if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &htoken))
                return FALSE;
        }
        else 
            return FALSE;
    }

    // Then we must query the size of the group information associated with
    // the token. Note that we expect a FALSE result from GetTokenInformation
    // because we've given it a NULL buffer. On exit cbTokenGroups will tell
    // the size of the group information.
    if (GetTokenInformation(htoken, TokenGroups, NULL, 0, &cbTokenGroups))
        return FALSE;

    // Allocate buffer for token groups
     if (!(ptg = (TOKEN_GROUPS*)_alloca(cbTokenGroups))) 
        return FALSE;

    // Now get the information
    if (!GetTokenInformation(htoken, TokenGroups, ptg, cbTokenGroups, &cbTokenGroups))
        return FALSE;

     // Create a System Identifier for the Admin group.
     if (!AllocateAndInitializeSid(&SystemSidAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, 
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &psidAdmin))
        return FALSE;

    // Iterate through the list of groups for this access
    // token looking for a match against the SID we created above.
    fAdmin = FALSE;
    for (dwGroup = 0 ; dwGroup < ptg->GroupCount ; ++dwGroup)
    {
        if (EqualSid(ptg->Groups[dwGroup].Sid, psidAdmin))
        {
            fAdmin = TRUE;
            break;
        }
    }

    // Before we exit we must explicity deallocate the SID we created.
    FreeSid(psidAdmin);
    CloseHandle(htoken);

    return fAdmin;
}


//  RegisterDLL
//      Does OLE registration on a DLL (same as regsvr32, but with less options)

typedef HRESULT (STDAPICALLTYPE *LPREGISTER_ENTRYPOINT)(void);

BOOL
RegisterDLL(
    LPCTSTR lpPath,
    BOOL fUnregister
    )
{
    LPREGISTER_ENTRYPOINT pfnEntrypoint;
    BOOL fRet = FALSE;
    HRESULT hr;

    // Load the library in question. Fail silently on errors
    UINT errMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    HINSTANCE hlib = LoadLibraryEx(lpPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (hlib < (HINSTANCE)HINSTANCE_ERROR)
    {
        goto nocleanup;
    }

    // Get the entrypoint
    if (fUnregister)
        pfnEntrypoint = (LPREGISTER_ENTRYPOINT)GetProcAddress(hlib, "DllUnregisterServer");
    else
        pfnEntrypoint = (LPREGISTER_ENTRYPOINT)GetProcAddress(hlib, "DllRegisterServer");
    if (!pfnEntrypoint)
    {
        goto cleanup;
    }

    // Call the entrypoint
    hr = (*pfnEntrypoint)();
    if (FAILED(hr))
    {
        goto cleanup;
    }

    // Success, we're done
    fRet = TRUE;

cleanup:
    FreeLibrary(hlib);

nocleanup:
    return fRet;
}


const LPTSTR RUN_ONCE_KEY = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce");
BOOL 
RegisterDLLOnReboot(LPCSTR lpPath)
{
    TCHAR  szRegisterCommand[512];
    TCHAR  systemPath[MAX_PATH];
    LPTSTR pszParse = szRegisterCommand;
    UINT   uLen;
    HKEY   hRunOnceKey;
    BOOL   fReturn = FALSE;
    if(GetSystemDirectory(systemPath, MAX_PATH))
    {
        wsprintf(szRegisterCommand, TEXT("\"%s\\regsvr32.exe\" /s \"%s\""), systemPath, lpPath);
        if(ERROR_SUCCESS==RegOpenKey(HKEY_LOCAL_MACHINE, RUN_ONCE_KEY, &hRunOnceKey))
        {
            // Get the length of the command in bytes
            uLen = strlen(szRegisterCommand)+1;
            uLen *= sizeof(TCHAR);
            //Get the name of the DLL to use as the unique ID.
            pszParse = strrchr(lpPath, '\\');
            pszParse++;
            if(ERROR_SUCCESS==RegSetValueEx(hRunOnceKey, pszParse, 0, REG_SZ, (LPBYTE)szRegisterCommand, uLen))
            {
                fReturn = TRUE;
            }
            RegCloseKey(hRunOnceKey);
        }
    }
    return fReturn;
}

int rsprintf(HINSTANCE hInstance, LPSTR pszDest, UINT uFormatResource, ...)
{
    int i=0;
    va_list vl;
    char szFormat[MAX_PATH];
    if(LoadStringA(hInstance, uFormatResource, szFormat, sizeof(szFormat)))
    {
        va_start(vl, uFormatResource); 
        i = vsprintf(pszDest, szFormat, vl);
        va_end(vl);
    }
    return i;
}

void FormatWindowText(HWND hWnd,...)
{
    char szFormat[MAX_PATH];
    char szText[MAX_PATH];
    GetWindowText(hWnd, szFormat, sizeof(szFormat));
    va_list vl;
    va_start(vl, hWnd); 
    vsprintf(szText, szFormat, vl);
    va_end(vl);
    SetWindowText(hWnd, szText);
}

BOOL Reboot()
{
    struct {
        DWORD PrivilegeCount; 
        LUID_AND_ATTRIBUTES RebootPrivilege;
    } tokenPrivilege;

    HANDLE hProcessToken;
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES, &hProcessToken))
    {
        tokenPrivilege.PrivilegeCount = 1;
        tokenPrivilege.RebootPrivilege.Attributes = SE_PRIVILEGE_ENABLED;
        if(LookupPrivilegeValue( NULL,SE_SHUTDOWN_NAME, &tokenPrivilege.RebootPrivilege.Luid))
        {
            if(AdjustTokenPrivileges(hProcessToken, FALSE, (PTOKEN_PRIVILEGES)&tokenPrivilege, sizeof(tokenPrivilege), NULL, NULL))
            {
                return ExitWindowsEx(EWX_REBOOT, 0);
            }
        }
   }
   return FALSE;
}