// dllreg.cpp -- autmatic registration and unregistration
//
#include "priv.h"
#include "installwv.h"

#include <advpub.h>
#include <comcat.h>
#include <xbnmspc.h>


// helper macros

// ADVPACK will return E_UNEXPECTED if you try to uninstall (which does a registry restore)
// on an INF section that was never installed.  We uninstall sections that may never have
// been installed, so this MACRO will quiet these errors.
#define QuietInstallNoOp(hr)   ((E_UNEXPECTED == hr) ? S_OK : hr)


const CHAR  c_szIexploreKey[]         = "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE";

/*----------------------------------------------------------
Purpose: Queries the registry for the location of the path
         of Internet Explorer and returns it in pszBuf.

Returns: TRUE on success
         FALSE if path cannot be determined

Cond:    --
*/
BOOL
GetIEPath(
    OUT LPSTR pszBuf,
    IN  DWORD cchBuf)
{
    BOOL bRet = FALSE;
    HKEY hkey;

    *pszBuf = '\0';

    // Get the path of Internet Explorer 
    if (NO_ERROR != RegOpenKeyA(HKEY_LOCAL_MACHINE, c_szIexploreKey, &hkey))  
    {
    }
    else
    {
        DWORD cbBrowser;
        DWORD dwType;

        lstrcatA(pszBuf, "\"");

        cbBrowser = CbFromCchA(cchBuf - lstrlenA(" -nohome") - 4);
        if (NO_ERROR != RegQueryValueExA(hkey, "", NULL, &dwType, 
                                         (LPBYTE)&pszBuf[1], &cbBrowser))
        {
        }
        else
        {
            bRet = TRUE;
        }

        lstrcatA(pszBuf, "\"");

        RegCloseKey(hkey);
    }

    return bRet;
}


BOOL UnregisterTypeLibrary(const CLSID* piidLibrary)
{
    TCHAR szScratch[GUIDSTR_MAX];
    HKEY hk;
    BOOL fResult = FALSE;

    // convert the libid into a string.
    //
    SHStringFromGUID(*piidLibrary, szScratch, ARRAYSIZE(szScratch));

    if (RegOpenKey(HKEY_CLASSES_ROOT, TEXT("TypeLib"), &hk) == ERROR_SUCCESS) {
        fResult = RegDeleteKey(hk, szScratch);
        RegCloseKey(hk);
    }
    
    return fResult;
}



HRESULT XboxRegTypeLib(void)
{
    HRESULT hr = S_OK;
    ITypeLib *pTypeLib;
    DWORD   dwPathLen;
    TCHAR   szTmp[MAX_PATH];
#ifdef UNICODE
    WCHAR   *pwsz = szTmp; 
#else
    WCHAR   pwsz[MAX_PATH];
#endif

    // Load and register our type library.
    //
    dwPathLen = GetModuleFileName(HINST_THISDLL, szTmp, ARRAYSIZE(szTmp));
#ifndef UNICODE
    if (SHAnsiToUnicode(szTmp, pwsz, MAX_PATH)) 
#endif
    {
        hr = LoadTypeLib(pwsz, &pTypeLib);

        if (SUCCEEDED(hr))
        {
            // call the unregister type library as we had some old junk that
            // was registered by a previous version of OleAut32, which is now causing
            // the current version to not work on NT...
            UnregisterTypeLibrary(&LIBID_XBNMSPCLib);
            hr = RegisterTypeLib(pTypeLib, pwsz, NULL);

            if (FAILED(hr))
            {
                TraceMsg(TF_WARNING, "XBNMSPC: RegisterTypeLib failed (%x)", hr);
            }
            pTypeLib->Release();
        }
        else
        {
            TraceMsg(TF_WARNING, "XBNMSPC: LoadTypeLib failed (%x)", hr);
        }
    } 
#ifndef UNICODE
    else {
        hr = E_FAIL;
    }
#endif

    return hr;
}


/*----------------------------------------------------------
Purpose: Calls the ADVPACK entry-point which executes an inf
         file section.

Returns: 
Cond:    --
*/
HRESULT CallRegInstall(HINSTANCE hinstXBOX, LPSTR szSection)
{
    HRESULT hr = E_FAIL;
    HINSTANCE hinstAdvPack = LoadLibrary(TEXT("ADVPACK.DLL"));

    if (hinstAdvPack)
    {
        REGINSTALL pfnri = (REGINSTALL)GetProcAddress(hinstAdvPack, "RegInstall");

        if (pfnri)
        {
            char szThisDLL[MAX_PATH];

            // Get the location of this DLL from the HINSTANCE
            if ( !EVAL(GetModuleFileNameA(hinstXBOX, szThisDLL, ARRAYSIZE(szThisDLL))) )
            {
                // Failed, just say "xbnmspc.dll"
                lstrcpyA(szThisDLL, "xbnmspc.dll");
            }

            STRENTRY seReg[] = {
                { "THISDLL", szThisDLL },

                // These two NT-specific entries must be at the end
                { "25", "%SystemRoot%" },
                { "11", "%SystemRoot%\\system32" },
            };
            STRTABLE stReg = { ARRAYSIZE(seReg) - 2, seReg };

            hr = pfnri(g_hinst, szSection, &stReg);
        }

        FreeLibrary(hinstAdvPack);
    }

    return hr;
}


STDAPI DllRegisterServer(void)
{
    HRESULT hrInternal;
    HRESULT hr;

    // Delete any old registration entries, then add the new ones.
    // Keep ADVPACK.DLL loaded across multiple calls to RegInstall.
    // (The inf engine doesn't guarantee DelReg/AddReg order, that's
    // why we explicitly unreg and reg here.)
    //
    HINSTANCE hinstXBOX = GetModuleHandle(TEXT("XBNMSPC.DLL"));
    HINSTANCE hinstAdvPack = LoadLibrary(TEXT("ADVPACK.DLL"));
    hr = CallRegInstall(hinstXBOX, "XboxNamespaceShellExtensionInstall");
    ASSERT(SUCCEEDED(hr));

    hrInternal = CallRegInstall(hinstXBOX, "XboxForceAssociations");
    if (SUCCEEDED(hr))
        EVAL(SUCCEEDED(hr = hrInternal));   // Propogate the error and assert.

    hrInternal = InstallWebViewFiles(hinstXBOX);
    if (SUCCEEDED(hr))
        EVAL(SUCCEEDED(hr = hrInternal));   // Propogate the error and assert.

    XboxRegTypeLib();

    if (hinstAdvPack)
        FreeLibrary(hinstAdvPack);

    return hr;
}

STDAPI DllUnregisterServer(void)
{
    HRESULT hr;
    HINSTANCE hinstXBOX = GetModuleHandle(TEXT("XBNMSPC.DLL"));

    // UnInstall the registry values
    hr = CallRegInstall(hinstXBOX, "XboxShellExtensionUninstall");
    UnregisterTypeLibrary(&LIBID_XBNMSPCLib);

    return hr;
}


/*----------------------------------------------------------
Purpose: Install/uninstall user settings

Description: Note that this function has special error handling.
             The function will keep hrExternal with the worse error
             but will only stop executing util the internal error (hr)
             gets really bad.  This is because we need the external
             error to catch incorrectly authored INFs but the internal
             error to be robust in attempting to install other INF sections
             even if one doesn't make it.
*/
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
    return S_OK;    
}