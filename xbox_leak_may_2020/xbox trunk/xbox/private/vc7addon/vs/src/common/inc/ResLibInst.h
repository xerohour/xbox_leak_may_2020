#pragma once

#if !defined(NUMBER_OF)
#define NUMBER_OF(x) (sizeof(x) / sizeof((x)[0]))
#endif // !defined(NUMBER_OF)

typedef BOOL (* PFN_GUILFN_LCID_CHECK)(LPARAM lParam, LCID lcid);

HRESULT W_GetUILibraryFileName
(
  LCID                  lcidDefault,    // can be 0
  LPCOLESTR             pszPath,
  LPCOLESTR             pszDllName,
  PFN_GUILFN_LCID_CHECK pfnCheckLCID,   // can be null
  LPARAM                lParam,
  BSTR                * pbstrFullPath,  // can be null
  LCID	              * plcidOut        // can be null
);

/*
HRESULT W_LoadPackageUILibrary(
  LPCOLESTR             pszRegRoot, 
  REFGUID               guidPackage, 
  DWORD                 dwExFlags, 
  PFN_GUILFN_LCID_CHECK pfnCheckLCID,   // can be null
  LPARAM                lParam,
  HINSTANCE           * phinstOut, 
  BSTR                * pbstrFullPath = NULL, 
  LCID                * plcidOut = NULL, 
  BOOL                  fUnloadCurrentLib = FALSE);
*/
