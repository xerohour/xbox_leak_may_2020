//////////////////////////////////////////////////////////////////////////////
//
//  File:       dtrsint.h
//  Module:     detours.lib
//
//  Detours for binary functions.  Version 1.3. (Build 43)
//
//  Copyright 1995-1999, Microsoft Corporation
//
//  http://research.microsoft.com/sn/detours
//

#pragma once
#ifndef _DTRSINT_H_
#define _DTRSINT_H_

//////////////////////////////////////////////////////////////////////////////
//
typedef LPAPI_VERSION (NTAPI *PF_ImagehlpApiVersionEx)(LPAPI_VERSION AppVersion);

typedef BOOL (NTAPI *PF_SymInitialize)(IN HANDLE hProcess,
                                       IN LPSTR UserSearchPath,
                                       IN BOOL fInvadeProcess);
typedef DWORD (NTAPI *PF_SymSetOptions)(IN DWORD SymOptions);
typedef DWORD (NTAPI *PF_SymGetOptions)(VOID);
typedef BOOL (NTAPI *PF_SymLoadModule)(IN HANDLE hProcess,
                                       IN HANDLE hFile,
                                       IN PSTR ImageName,
                                       IN PSTR ModuleName,
                                       IN DWORD BaseOfDll,
                                       IN DWORD SizeOfDll);
typedef BOOL (NTAPI *PF_SymGetModuleInfo)(IN HANDLE hProcess,
                                          IN DWORD dwAddr,
                                          OUT PIMAGEHLP_MODULE ModuleInfo);
typedef BOOL (NTAPI *PF_SymGetSymFromName)(IN HANDLE hProcess,
                                           IN LPSTR Name,
                                           OUT PIMAGEHLP_SYMBOL Symbol);
typedef BOOL (NTAPI *PF_BindImage)(IN LPSTR pszImageName,
                                   IN LPSTR pszDllPath,
                                   IN LPSTR pszSymbolPath);

typedef struct _DETOUR_SYM_INFO
{
    HANDLE                   hProcess;
    HMODULE                  hImageHlp;
    PF_ImagehlpApiVersionEx  pfImagehlpApiVersionEx;
    PF_SymInitialize         pfSymInitialize;
    PF_SymSetOptions         pfSymSetOptions;
    PF_SymGetOptions         pfSymGetOptions;
    PF_SymLoadModule         pfSymLoadModule;
    PF_SymGetModuleInfo      pfSymGetModuleInfo;
    PF_SymGetSymFromName     pfSymGetSymFromName;
    PF_BindImage             pfBindImage;
} DETOUR_SYM_INFO, *PDETOUR_SYM_INFO;

//////////////////////////////////////////////////////////////////////////////
//
PDETOUR_SYM_INFO DetourLoadImageHlp(VOID);

//////////////////////////////////////////////////////////////////////////////
//
inline PBYTE DetourGenMovEax(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xB8;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEbx(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBB;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEcx(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xB9;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEdx(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBA;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEsi(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBE;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEdi(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBF;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEbp(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBD;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEsp(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBC;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenPush(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0x68;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenJmp(PBYTE pbCode, PBYTE pbJmpDst, PBYTE pbJmpSrc = 0)
{
    if (pbJmpSrc == 0) {
        pbJmpSrc = pbCode;
    }
    *pbCode++ = 0xE9;
    *((INT32*&)pbCode)++ = pbJmpDst - (pbJmpSrc + 5);
    return pbCode;
}

inline PBYTE DetourGenCall(PBYTE pbCode, PBYTE pbJmpDst, PBYTE pbJmpSrc = 0)
{
    if (pbJmpSrc == 0) {
        pbJmpSrc = pbCode;
    }
    *pbCode++ = 0xE8;
    *((INT32*&)pbCode)++ = pbJmpDst - (pbJmpSrc + 5);
    return pbCode;
}

inline PBYTE DetourGenBreak(PBYTE pbCode)
{
    *pbCode++ = 0xcc;
    return pbCode;
}

inline PBYTE DetourGenBreakRet(PBYTE pbCode)
{
    *pbCode++ = 0xc3;
    return pbCode;
}

inline PBYTE DetourGenBreakNop(PBYTE pbCode)
{
    *pbCode++ = 0x90;
    return pbCode;
}

#endif // _DTRSINT_H_

////////////////////////////////////////////////////////////////  End of File.

