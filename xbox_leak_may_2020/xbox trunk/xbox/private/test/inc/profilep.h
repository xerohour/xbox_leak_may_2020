/************************************************************************
*                                                                       *
*   profilep.h -- INI file APIs pulled from winbase, used by tests      *
*                                                                       *
*   Copyright (c) 1990-1998, Microsoft Corp. All rights reserved.       *
*                                                                       *
************************************************************************/
#ifndef _PROFILEP_
#define _PROFILEP_


//
// Define API decoration for direct importing of DLL references.
//

#ifndef WINBASEAPI
#define WINBASEAPI
#endif  // ! WINBASEAPI

#ifdef __cplusplus
extern "C" {
#endif

WINBASEAPI
UINT
WINAPI
GetProfileIntA(
    IN LPCSTR lpAppName,
    IN LPCSTR lpKeyName,
    IN INT nDefault
    );
WINBASEAPI
UINT
WINAPI
GetProfileIntW(
    IN LPCWSTR lpAppName,
    IN LPCWSTR lpKeyName,
    IN INT nDefault
    );
#ifdef UNICODE
#define GetProfileInt  GetProfileIntW
#else
#define GetProfileInt  GetProfileIntA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetProfileStringA(
    IN LPCSTR lpAppName,
    IN LPCSTR lpKeyName,
    IN LPCSTR lpDefault,
    OUT LPSTR lpReturnedString,
    IN DWORD nSize
    );
WINBASEAPI
DWORD
WINAPI
GetProfileStringW(
    IN LPCWSTR lpAppName,
    IN LPCWSTR lpKeyName,
    IN LPCWSTR lpDefault,
    OUT LPWSTR lpReturnedString,
    IN DWORD nSize
    );
#ifdef UNICODE
#define GetProfileString  GetProfileStringW
#else
#define GetProfileString  GetProfileStringA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
WriteProfileStringA(
    IN LPCSTR lpAppName,
    IN LPCSTR lpKeyName,
    IN LPCSTR lpString
    );
WINBASEAPI
BOOL
WINAPI
WriteProfileStringW(
    IN LPCWSTR lpAppName,
    IN LPCWSTR lpKeyName,
    IN LPCWSTR lpString
    );
#ifdef UNICODE
#define WriteProfileString  WriteProfileStringW
#else
#define WriteProfileString  WriteProfileStringA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetProfileSectionA(
    IN LPCSTR lpAppName,
    OUT LPSTR lpReturnedString,
    IN DWORD nSize
    );
WINBASEAPI
DWORD
WINAPI
GetProfileSectionW(
    IN LPCWSTR lpAppName,
    OUT LPWSTR lpReturnedString,
    IN DWORD nSize
    );
#ifdef UNICODE
#define GetProfileSection  GetProfileSectionW
#else
#define GetProfileSection  GetProfileSectionA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
WriteProfileSectionA(
    IN LPCSTR lpAppName,
    IN LPCSTR lpString
    );
WINBASEAPI
BOOL
WINAPI
WriteProfileSectionW(
    IN LPCWSTR lpAppName,
    IN LPCWSTR lpString
    );
#ifdef UNICODE
#define WriteProfileSection  WriteProfileSectionW
#else
#define WriteProfileSection  WriteProfileSectionA
#endif // !UNICODE

WINBASEAPI
UINT
WINAPI
GetPrivateProfileIntA(
    IN LPCSTR lpAppName,
    IN LPCSTR lpKeyName,
    IN INT nDefault,
    IN LPCSTR lpFileName
    );
WINBASEAPI
UINT
WINAPI
GetPrivateProfileIntW(
    IN LPCWSTR lpAppName,
    IN LPCWSTR lpKeyName,
    IN INT nDefault,
    IN LPCWSTR lpFileName
    );
#ifdef UNICODE
#define GetPrivateProfileInt  GetPrivateProfileIntW
#else
#define GetPrivateProfileInt  GetPrivateProfileIntA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetPrivateProfileStringA(
    IN LPCSTR lpAppName,
    IN LPCSTR lpKeyName,
    IN LPCSTR lpDefault,
    OUT LPSTR lpReturnedString,
    IN DWORD nSize,
    IN LPCSTR lpFileName
    );
WINBASEAPI
DWORD
WINAPI
GetPrivateProfileStringW(
    IN LPCWSTR lpAppName,
    IN LPCWSTR lpKeyName,
    IN LPCWSTR lpDefault,
    OUT LPWSTR lpReturnedString,
    IN DWORD nSize,
    IN LPCWSTR lpFileName
    );
#ifdef UNICODE
#define GetPrivateProfileString  GetPrivateProfileStringW
#else
#define GetPrivateProfileString  GetPrivateProfileStringA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
WritePrivateProfileStringA(
    IN LPCSTR lpAppName,
    IN LPCSTR lpKeyName,
    IN LPCSTR lpString,
    IN LPCSTR lpFileName
    );
WINBASEAPI
BOOL
WINAPI
WritePrivateProfileStringW(
    IN LPCWSTR lpAppName,
    IN LPCWSTR lpKeyName,
    IN LPCWSTR lpString,
    IN LPCWSTR lpFileName
    );
#ifdef UNICODE
#define WritePrivateProfileString  WritePrivateProfileStringW
#else
#define WritePrivateProfileString  WritePrivateProfileStringA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetPrivateProfileSectionA(
    IN LPCSTR lpAppName,
    OUT LPSTR lpReturnedString,
    IN DWORD nSize,
    IN LPCSTR lpFileName
    );
WINBASEAPI
DWORD
WINAPI
GetPrivateProfileSectionW(
    IN LPCWSTR lpAppName,
    OUT LPWSTR lpReturnedString,
    IN DWORD nSize,
    IN LPCWSTR lpFileName
    );
#ifdef UNICODE
#define GetPrivateProfileSection  GetPrivateProfileSectionW
#else
#define GetPrivateProfileSection  GetPrivateProfileSectionA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
WritePrivateProfileSectionA(
    IN LPCSTR lpAppName,
    IN LPCSTR lpString,
    IN LPCSTR lpFileName
    );
WINBASEAPI
BOOL
WINAPI
WritePrivateProfileSectionW(
    IN LPCWSTR lpAppName,
    IN LPCWSTR lpString,
    IN LPCWSTR lpFileName
    );
#ifdef UNICODE
#define WritePrivateProfileSection  WritePrivateProfileSectionW
#else
#define WritePrivateProfileSection  WritePrivateProfileSectionA
#endif // !UNICODE


WINBASEAPI
DWORD
WINAPI
GetPrivateProfileSectionNamesA(
    OUT LPSTR lpszReturnBuffer,
    IN DWORD nSize,
    IN LPCSTR lpFileName
    );
WINBASEAPI
DWORD
WINAPI
GetPrivateProfileSectionNamesW(
    OUT LPWSTR lpszReturnBuffer,
    IN DWORD nSize,
    IN LPCWSTR lpFileName
    );
#ifdef UNICODE
#define GetPrivateProfileSectionNames  GetPrivateProfileSectionNamesW
#else
#define GetPrivateProfileSectionNames  GetPrivateProfileSectionNamesA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
GetPrivateProfileStructA(
    IN LPCSTR lpszSection,
    IN LPCSTR lpszKey,
    OUT LPVOID   lpStruct,
    IN UINT     uSizeStruct,
    IN LPCSTR szFile
    );
WINBASEAPI
BOOL
WINAPI
GetPrivateProfileStructW(
    IN LPCWSTR lpszSection,
    IN LPCWSTR lpszKey,
    OUT LPVOID   lpStruct,
    IN UINT     uSizeStruct,
    IN LPCWSTR szFile
    );
#ifdef UNICODE
#define GetPrivateProfileStruct  GetPrivateProfileStructW
#else
#define GetPrivateProfileStruct  GetPrivateProfileStructA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
WritePrivateProfileStructA(
    IN LPCSTR lpszSection,
    IN LPCSTR lpszKey,
    IN LPVOID   lpStruct,
    IN UINT     uSizeStruct,
    IN LPCSTR szFile
    );
WINBASEAPI
BOOL
WINAPI
WritePrivateProfileStructW(
    IN LPCWSTR lpszSection,
    IN LPCWSTR lpszKey,
    IN LPVOID   lpStruct,
    IN UINT     uSizeStruct,
    IN LPCWSTR szFile
    );
#ifdef UNICODE
#define WritePrivateProfileStruct  WritePrivateProfileStructW
#else
#define WritePrivateProfileStruct  WritePrivateProfileStructA
#endif // !UNICODE


#ifdef __cplusplus
}
#endif


#endif // _PROFILEP_

