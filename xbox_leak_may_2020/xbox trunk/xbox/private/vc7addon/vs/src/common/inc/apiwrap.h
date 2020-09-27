//---------------------------------------------------------------------------
// Microsoft Visual Studio
//
// Microsoft Confidential
// Copyright (C) 1994 - 2000 Microsoft Corporation. 
// All Rights Reserved.
//
// Win32 API Unicode wrappers
//
// Implements Unicode versions of Win32 APIs that run on both NT and Win95
//---------------------------------------------------------------------------
#ifndef APIWRAP_H_INCLUDED
#define APIWRAP_H_INCLUDED

#include "windows.h"
#include "smartptr.h"
#include "dll.h"


extern bool g_fIsUnicodeOS; // global flag is true if OS support Unicode

// Win32 API wrappers
int Win32SHFileOperationA(LPSHFILEOPSTRUCTA pFileOp);
int Win32SHFileOperationW(LPSHFILEOPSTRUCTW pFileOp);

// declare function pointer types
typedef int (WINAPI * PFN_SH_FILE_OPERATIONA)(LPSHFILEOPSTRUCTA pFileOp);
typedef int (WINAPI * PFN_SH_FILE_OPERATIONW)(LPSHFILEOPSTRUCTW pFileOp);

// declare import names
// import names must be Ansi strings, don't wrap with TEXT()
#define IMPORT_NAME_SH_FILE_OPERATIONA     "SHFileOperation"
#define IMPORT_NAME_SH_FILE_OPERATIONW     "SHFileOperationW"

class CShell32DLL : public CDLL
{
public:
    CShell32DLL();
    int SHFileOperationA(LPSHFILEOPSTRUCTA pFileOp);
    int SHFileOperationW(LPSHFILEOPSTRUCTW pFileOp);
protected:
    CDLLImport<PFN_SH_FILE_OPERATIONA> ImportSHFileOperationA;
    CDLLImport<PFN_SH_FILE_OPERATIONW> ImportSHFileOperationW;
    virtual bool GetDLLImports();
};

inline CShell32DLL::CShell32DLL() : CDLL(L"shell32.dll")
{
}

inline bool CShell32DLL::GetDLLImports()
{
    return ImportSHFileOperationA.ResolveImportAddress(hDLL, IMPORT_NAME_SH_FILE_OPERATIONA, false)
        && ImportSHFileOperationW.ResolveImportAddress(hDLL, IMPORT_NAME_SH_FILE_OPERATIONW, false);
}


#endif // APIWRAP_H_INCLUDED