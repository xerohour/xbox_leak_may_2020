// This header provides useful, protected wrappers for some OS functions.  Note that it is up to the 
// user of this code to determine whether it needs to go for the Unicode or ANSI versions of these
// functions.  All that is done here is to provide buffer protection and verification of success for 
// these calls.

#pragma once

extern BOOL UtilGetTempFileNameW(LPCOLESTR szDir, LPCOLESTR szPre, CStringW& strFile);
extern BOOL UtilGetTempFileNameA(LPCSTR szDir, LPCSTR szPre, CStringA& strFile);
extern BOOL UtilGetModuleFileNameW(HMODULE hModule, CStringW& strFileName);
extern BOOL UtilGetTempPathW(CStringW& strBuf);
extern BOOL UtilGetTempPathA(CStringA& strBuf);
extern BOOL UtilGetModuleFileNameW(HMODULE hModule, CStringA& strFileName);
extern HRESULT VCGetEnvironmentVariableW(LPCOLESTR strVar, BSTR *pbstr);
extern HRESULT VCGetEnvironmentVariableA(LPCSTR strVar, BSTR *pbstr);

#ifdef _UNICODE
#define UtilGetTempFileName UtilGetTempFileNameW
#define UtilGetTempPath UtilGetTempPathW
#define UtilGetModuleFileName UtilGetModuleFileNameW
#define VCGetEnvironmentVariable VCGetEnvironmentVariableW
#else	// _UNICODE
#define UtilGetTempFileName UtilGetTempFileNameA
#define UtilGetTempPath UtilGetTempPathA
#define UtilGetModuleFileName UtilGetModuleFileNameA
#define VCGetEnvironmentVariable VCGetEnvironmentVariableA
#endif	// _UNICODE

