#pragma once 

#include "std.h"


float GetFloat( char** pStr );
void GetNameData (char *rawData, char *result);
void GetWordData (char *rawData, char *result);
void GetNumberData (char *rawData, char *result);
float XAppGetNow();

void Unicode(wchar_t* wsz, const char* sz, int nMaxChars);
void Ansi(char* sz, const TCHAR* wsz, int nMaxChars);

#define countof(n) (sizeof (n) / sizeof (n[0]))


#ifdef _DEBUG
void XAppGetErrorString(HRESULT hr, TCHAR* szBuf, int cchBuf);
const TCHAR* XAppGetErrorString(HRESULT hr);
void LogComError(HRESULT hr, const char* szFunc = NULL);
void LogError(const char* szFunc);
#else
inline void LogComError(HRESULT hr, const char* szFunc = NULL) {}
inline void LogError(const char* szFunc) {}
#endif

