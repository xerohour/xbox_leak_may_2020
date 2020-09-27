/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    util.h

Abstract:

    Contains utility classes and routines


--*/

#ifndef _UTIL_H
#define _UTIL_H

#include <windows.h>
#include <winnt.h>
#include <stdlib.h>
#include "list.h"
#include "rombld.h"


//
// Key file format copied from sdktools\xcrypt
//
#define KEYFILE_PUBPAIR      1      // public and private keys
#define KEYFILE_PUBONLY      2      // public key only
#define KEYFILE_SYM          3      // symmetric key

#define KEYFILE_ENCFILE_SIG  'kcne' // signature dword in the encrypted key file


typedef struct _XBOX_KEYFILE_HEADER
{
    USHORT KeyType;                 // KEYFILE_ defines
    USHORT KeyBits;                 // number of bits in the key
    GUID   KeyGUID;                 // unique guid genereted at the time the key file was created
    ULONG  KeyNumber;               // a number assigned key
    ULONG  PublicKeyBytes;          // bytes used by public key structure
    ULONG  PrivateKeyBytes;         // bytes used by private key structure

} XBOX_KEYFILE_HEADER, *PXBOX_KEYFILE_HEADER;


#define PASSWORD_MAXSIZE     21
#define PASSWORD_MINLEN      8


class CMemFile
{
public:
    CMemFile() 
        : m_hFile(INVALID_HANDLE_VALUE),
          m_hFileMapping(NULL),
          m_pBase(NULL),
          m_pHeader(NULL),
          m_pWriteCopy(NULL),
          m_dwSize(0)
    {
    }

    ~CMemFile()
    {
        Close();
    }
    HRESULT Open(LPCSTR pszFN, BOOL CopyOnWrite = FALSE);
    HRESULT MakeWriteCopy();
    void Close();
    DWORD Size();
    PVOID Ptr() 
    {
        return m_pBase;
    }

    //
    // for PE Files
    //
    DWORD GetDirectoryEntryVA(USHORT uDirectoryEntry)
    {
        return m_pHeader->OptionalHeader.DataDirectory[uDirectoryEntry].VirtualAddress;
    }
    DWORD GetDirectoryEntrySize(USHORT uDirectoryEntry)
    {
        return m_pHeader->OptionalHeader.DataDirectory[uDirectoryEntry].Size;
    }
    PVOID PtrFromBase(DWORD dwOfs)
    {
        return (PVOID)((PBYTE)m_pBase + dwOfs);
    }
    PIMAGE_NT_HEADERS NtHeaders()
    {
        return m_pHeader;
    }

private:
    HANDLE m_hFile;
    HANDLE m_hFileMapping;
    PVOID m_pWriteCopy;

    //
    // for PE files
    //
    PIMAGE_NT_HEADERS m_pHeader;
    PVOID m_pBase;
    DWORD m_dwSize;   
};




class CStrNode : public CNode
{
public:
    CStrNode() 
        : m_pszValue(NULL)
    {
    }
    ~CStrNode()
    {
        if (m_pszValue != NULL)
            free(m_pszValue);
    }
    void SetValue(LPCSTR pszValue)
    {
        if (m_pszValue != NULL)
            free(m_pszValue);
        m_pszValue = _strdup(pszValue);
    }
    LPCSTR GetValue()
    {
        return m_pszValue;
    }

    virtual void Dump();

private:
    LPSTR m_pszValue;
};


class CTimer
{
public:
    // constructor
    CTimer()
    {
        QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
        ticksPerSecond /= 1000;
    }

    ~CTimer() {};

    void Start()
        { frunning = true; QueryPerformanceCounter((LARGE_INTEGER *)&ticksstart); }

    void Stop()
        { frunning = false; QueryPerformanceCounter((LARGE_INTEGER *)&ticksend); }

    float getTime()
        {
            if(frunning)
                QueryPerformanceCounter((LARGE_INTEGER *)&ticksend);
            return ((ticksend - ticksstart) * 1.0f / ticksPerSecond);
        }

    bool frunning;
    _int64 ticksstart;
    _int64 ticksend;
    _int64 ticksPerSecond;
};



typedef struct
{
    enum VARTYPE {VAR_NONE, VAR_INT, VAR_STRING, VAR_STRINGLIST};

    LPCTSTR lpCmdLine;
    LPCTSTR lpPropName;
    VARTYPE vartype;
    void* lpVar;

    static int _cdecl Compare(const void*, const void*);
} CMDTABLE;


LPCSTR strcpystr(LPCSTR pszStr, LPCSTR pszSep, LPSTR pszTokOut);

LPSTR strncpyz(LPSTR pszDest, LPCSTR pszSource, size_t count);

HRESULT TempFileName(LPSTR szFN);

BOOL CompressFile(CMemFile &SourceFile, LPCSTR pszCompressName);

BOOL ReadKeyFromEncFile(LPCSTR pszFN, LPCSTR pszPassword, PBYTE  pKeyBuffer, DWORD cbKeyBuffer, BOOL SymKey);

BOOL ReadPassword(LPCSTR pszPrompt, LPSTR pszPassword);

BOOL IsEncKeyFile(LPCSTR pszFN);

VOID BuildFilename(LPSTR pszFullFN, LPCSTR pszPath, LPCSTR pszFN);

VOID DumpBytes(LPCSTR pszLabel, PBYTE pbData, DWORD dwDataLen);


#endif // _UTIL_H
