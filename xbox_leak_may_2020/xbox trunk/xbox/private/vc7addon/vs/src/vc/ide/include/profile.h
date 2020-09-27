// profile.h: useful routines for manipulating the registry

#pragma once

#include <vcstring.h>

#define MAX_REG_STR	4096

const char chKeySep = '\\';

LPCTSTR GetRegistryKeyName();
void SetAlternateKey(LPCSTR szAltKey);

CVCString GetRegString(LPCSTR szSection, LPCSTR szKey, LPCSTR szDefault = NULL);
CStringW GetRegString(LPCOLESTR szSection, LPCOLESTR szKey, LPCOLESTR szDefault = NULL);
void GetRegPath(LPCSTR szSection, LPCSTR szKey, CStringA& strPath, LPCTSTR szDefault = NULL, BOOL bDefaultToHKLM = TRUE);
void GetRegPath(LPCOLESTR szSection, LPCOLESTR szKey, CStringW& strPath, LPCOLESTR szDefault = NULL, BOOL bDefaultToHKLM = TRUE);
DWORD GetRegInt(LPCSTR szSection, LPCSTR szKey, DWORD dwDefault);
extern HGLOBAL GetRegData(LPCSTR szSection, LPCSTR szKey, HGLOBAL hglobDefault);
IStream *GetRegIStream(LPCTSTR szSection, LPCTSTR szKey);

BOOL WriteRegString(LPCSTR szSection, LPCSTR szKey, LPCSTR szVal, BOOL bUserSpecific = TRUE);
BOOL WriteRegString(LPCOLESTR szSection, LPCOLESTR szKey, LPCOLESTR szVal, BOOL bUserSpecific = TRUE);
BOOL WriteRegPath(LPCSTR szSection, LPCSTR szKey, LPCSTR szVal, BOOL bUserSpecific = TRUE);
BOOL WriteRegPath(LPCOLESTR szSection, LPCOLESTR szKey, LPCOLESTR szVal, BOOL bUserSpecific = TRUE);
BOOL WriteRegInt(LPCSTR szSection, LPCSTR szKey, DWORD nValue, BOOL bUserSpecific = TRUE);
extern BOOL WriteRegData(LPCSTR szSection, LPCSTR szKey, HGLOBAL hglobValue);
BOOL WriteRegIStream(LPCTSTR szSection, LPCTSTR szKey, IStream *pIStream);

BOOL DeleteRegData(LPCSTR szSection, LPCSTR szKey);

HRESULT WriteRegPathW(LPCOLESTR szFile,LPCOLESTR szSection, LPCOLESTR szKey, LPCOLESTR szVal);
HRESULT WriteRegStringW(LPCOLESTR szSection, LPCOLESTR szKey, LPCOLESTR szVal, HKEY keyType = HKEY_CURRENT_USER);
HRESULT WriteRegIntW(LPCOLESTR szSection, LPCOLESTR szKey, DWORD nValue, HKEY keyType = HKEY_CURRENT_USER);
HRESULT WriteRegBoolW(LPCOLESTR szSection, LPCOLESTR szKey, VARIANT_BOOL nValue, HKEY keyType = HKEY_CURRENT_USER);

HRESULT GetRegPathW(LPCOLESTR szFile, LPCOLESTR szSection, LPCOLESTR szKey, BSTR *pbstrOut);
HRESULT GetRegStringW(LPCOLESTR szSection, LPCOLESTR szKey, BSTR *pbstrOut);
HRESULT GetRegIntW(LPCOLESTR szSection, LPCOLESTR szKey, DWORD *pdwOut);
HRESULT GetRegBoolW(LPCOLESTR szSection, LPCOLESTR szKey, VARIANT_BOOL *pdwOut);

HRESULT GetAppDataDirW(BSTR *pbstrAppData);
