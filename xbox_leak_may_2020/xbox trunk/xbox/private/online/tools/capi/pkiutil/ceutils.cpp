/*
	ceutils.cpp

	Collection of functions used by CAPI and not implemented on WinCE
*/
#include "global.hxx"

#ifdef UNDER_CE
LONG
APIENTRY
CERegCreateKeyExA (
    IN HKEY hKey,
    IN LPCSTR lpSubKey,
    IN DWORD Reserved,
    IN LPSTR lpClass,
    IN DWORD dwOptions,
    IN REGSAM samDesired,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    OUT PHKEY phkResult,
    OUT LPDWORD lpdwDisposition
    )
{
	LONG ret;
	LPWSTR pwszSubKey = MkWStr(lpSubKey);
	ret = RegCreateKeyExW(hKey,pwszSubKey,Reserved,
				NULL, dwOptions, samDesired, 
				lpSecurityAttributes,
				phkResult, lpdwDisposition);
	FreeWStr(pwszSubKey);
	return ret;
}

LONG
APIENTRY
CERegOpenKeyExA (
    IN HKEY hKey,
    IN LPCSTR lpSubKey,
    IN DWORD ulOptions,
    IN REGSAM samDesired,
    OUT PHKEY phkResult
    )
{
	LONG ret;
	LPWSTR pwszSubKey = MkWStr(lpSubKey);
	ret = RegOpenKeyExU(hKey, pwszSubKey, ulOptions, samDesired, phkResult);
	FreeWStr(pwszSubKey);
	return ret;
}

LONG
APIENTRY
CERegDeleteKeyA (
    IN HKEY hKey,
    IN LPCSTR lpSubKey
    )
{
	LONG ret;
	LPWSTR pwszSubKey = MkWStr(lpSubKey);
	ret = RegDeleteKeyW(hKey, pwszSubKey);
	FreeWStr(pwszSubKey);
	return ret;
}

LONG
APIENTRY
CERegSetValueExA (
    IN HKEY hKey,
    IN LPCSTR lpValueName,
    IN DWORD Reserved,
    IN DWORD dwType,
    IN CONST BYTE* lpData,
    IN DWORD cbData
    )
{
	LONG ret;
	LPWSTR pwszValueName = MkWStr(lpValueName);
	LPWSTR pwszValue = NULL;
	if (dwType == REG_SZ)
	{
		pwszValue = MkWStr((char *)lpData);
		lpData = (CONST BYTE *)pwszValue;
	}
	ret = RegSetValueExW(hKey, pwszValueName, Reserved, dwType, lpData, cbData);

	FreeWStr(pwszValueName);
	FreeWStr(pwszValue);
	return ret;
}

LONG RegEnumKeyA(  HKEY hKey,     // handle to key to query
  DWORD dwIndex, // index of subkey to query
  LPSTR lpName, // address of buffer for subkey name
  DWORD cbName   // size of subkey buffer
  )
{
	LPWSTR lpwszName = (LPWSTR) LocalAlloc(0,cbName*sizeof(WCHAR));
	FILETIME ft;
	char *pszConverted;
	LONG ret;
	ret = RegEnumKeyEx( hKey, dwIndex, lpwszName, &cbName, NULL, NULL, NULL, &ft);
	if (lpwszName)
	{
		if (ret == 0)
		{
			MkMBStr((PBYTE)lpName,cbName,lpwszName, &pszConverted);
			FreeMBStr((PBYTE)lpName, pszConverted);			
		}
	}
	return ret;
}

#endif //UNDER_CE
