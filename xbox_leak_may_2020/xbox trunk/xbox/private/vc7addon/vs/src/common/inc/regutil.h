////////////////////////////////////////////////////////////////////////////////
// regutil.h

extern long RegDeleteSubKeys(HKEY, LPCSTR);

//---------------------------------------------------------------------------
// contruct strings to be used in _ATL_REGMAP_ENTRY for the app reg root
//---------------------------------------------------------------------------
HRESULT util_GetRegRootStrings(LPCOLESTR pszRegistrationRoot, BSTR * pbstrRootBegin, BSTR * pbstrRootEnd);
