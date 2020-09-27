/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    xctool.h

Abstract:

    Generates and dumps keys

--*/

 
#ifndef _XCTOOL_H
#define _XCTOOL_H

#include <windows.h>

class CXCTool
{
public:

    enum DUMPTYPE {DUMP_NONE, DUMP_BIN, DUMP_ASM, DUMP_C};

    CXCTool() {}
    
    BOOL GeneratePubPair(LPCSTR pszKeyFileName, BOOL bEnc);
    BOOL GenerateSymKey(LPCSTR pszKeyFileName, BOOL bEnc);
    BOOL DumpKeyFile(LPCSTR pszKey, DUMPTYPE DumpType);
    BOOL CopyKeyFile(LPCSTR pszInFileName, LPCSTR pszOutFileName, BOOL bEncOut);

    virtual void Out(LPCSTR pszFormat, ...);
    virtual void DumpBytes(DUMPTYPE DumpType, LPCSTR pszLabel, PBYTE pbData, DWORD dwDataLen);
    virtual BOOL ReadPassword(LPCSTR pszPrompt, LPSTR pszPassword);
};


#endif //_XCTOOL_H