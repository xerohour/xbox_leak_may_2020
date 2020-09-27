/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    util.cpp

Abstract:

    Contains utility classes and routines

--*/

#include "rombld.h"
#include "util.h"
#include "conio.h"
#include <rsa.h>
#include <sha.h>
#include <xcrypt.h>
#include "enc.h"


extern "C" {
#include <lci.h>
}

#include <bldr.h>



HRESULT 
TempFileName(LPSTR szFN)
{
    char szTempPath[MAX_PATH];

    if (GetTempPath(sizeof(szTempPath), szTempPath) != 0)
    {
        if (GetTempFileName(szTempPath, "ROM", 0, szFN) != 0)
        {
            return S_OK;
        }
    }
    return HRESULT_FROM_WIN32(GetLastError());
    
}



HRESULT 
CMemFile::Open(
    IN LPCSTR pszFN,
    IN BOOL CopyOnWrite
    )
/*++

Routine Description:

    Opens a file by creating a file mapping

Arguments:

    pszFN - supplies the file name to open

Return Value:

    HRESULT

--*/
{
    HRESULT hr = NOERROR;

    m_hFile = CreateFile(pszFN, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    m_hFileMapping = CreateFileMapping(m_hFile, NULL, CopyOnWrite ? PAGE_WRITECOPY : PAGE_READONLY, 0, 0, NULL);
    if (m_hFileMapping == NULL)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());

        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;

        return hr;
    }

    m_pBase = MapViewOfFile(m_hFileMapping, CopyOnWrite ? FILE_MAP_COPY : FILE_MAP_READ, 0, 0, 0);
    if (m_pBase == NULL)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());

        CloseHandle(m_hFileMapping);
        m_hFileMapping = NULL;

        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;

        return hr;
    }

    m_pHeader = (PIMAGE_NT_HEADERS)PtrFromBase(((PIMAGE_DOS_HEADER)m_pBase)->e_lfanew);

    return hr;
}


void
CMemFile::Close()
/*++

Routine Description:

    Closes the file

Arguments:

Return Value:

    None

--*/
{
    if (m_pWriteCopy != NULL)
    {
        free(m_pWriteCopy);
        m_pWriteCopy = NULL;
    }
    else
    {
        if (m_pBase != NULL)
        {
            UnmapViewOfFile(m_pBase);
            m_pBase = NULL;
        }
    }

    if (m_hFileMapping != NULL)
    {
        CloseHandle(m_hFileMapping);
        m_hFileMapping = NULL;
    }

    if (m_hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
}



HRESULT 
CMemFile::MakeWriteCopy()
{
    if (m_hFileMapping == NULL || m_hFile == INVALID_HANDLE_VALUE)
        return E_FAIL;

    m_dwSize = GetFileSize(m_hFile, NULL);

    m_pWriteCopy = malloc(m_dwSize);

    if (NULL == m_pWriteCopy)
    {
        return E_OUTOFMEMORY;
    }
    
    memcpy(m_pWriteCopy, m_pBase, m_dwSize);

    UnmapViewOfFile(m_pBase);
    m_pBase = m_pWriteCopy;
    m_pHeader = (PIMAGE_NT_HEADERS)PtrFromBase(((PIMAGE_DOS_HEADER)m_pBase)->e_lfanew);

    CloseHandle(m_hFileMapping);
    m_hFileMapping = NULL;

    CloseHandle(m_hFile);
    m_hFile = INVALID_HANDLE_VALUE;

    return NOERROR;
}


DWORD
CMemFile::Size()
{
    if (m_hFileMapping == NULL || m_hFile == INVALID_HANDLE_VALUE)
        return m_dwSize;

    return GetFileSize(m_hFile, NULL);
}



int
_cdecl
CMDTABLE::Compare(
    const void* p1, 
    const void* p2
    )
{
    char** pkey = (char**)p1;
    CMDTABLE* ptab = (CMDTABLE*)p2;

    return _strnicmp(*pkey, ptab->lpCmdLine, strlen(ptab->lpCmdLine));
}



 
void
CStrNode::Dump()
{
    TRACE_OUT(TRACE_ALWAYS, m_pszValue);
}




LPCSTR 
strcpystr(
    IN  LPCSTR pszStr, 
    IN  LPCSTR pszSep, 
    OUT LPSTR pszTokOut)
/*++

Routine Description:

    Called multiple times to parse out tokens from a string

Arguments:

    pszStr - Supplies the string to parse out.  This can be return value
             of this function for subsequent calls.

    pszSep - Supplies a string that is the separator between tokens

    pszTokOut - Returns the token.  The caller must allocate enough space for the token

Return Value:

    Returns a pointer to the next token or NULL

--*/
{
    
    if (pszStr == NULL || *pszStr == '\0')
    {
        pszTokOut[0] = '\0';
        return NULL;
    }

    LPCSTR p = strstr(pszStr, pszSep);
    if (p != NULL)
    {
        strncpy(pszTokOut, pszStr, p - pszStr);
        pszTokOut[p - pszStr] = '\0';       
        return p + strlen(pszSep);
    }
    else
    {
        strcpy(pszTokOut, pszStr);
        return NULL;
    }
}


LPSTR
strncpyz(
    OUT LPSTR pszDest, 
    IN  LPCSTR pszSource, 
    IN  size_t count
    )
/*++

Routine Description:

    Copies characters from source to destination similar to strncpy but ensures that a NULL 
    character is always copied even if the source length is greater or equal to the count

Arguments:

    pszDest - Supplies the destination string
    
    pszSource - Supplies the source string

    count - Supplies the number of characters to be copied (size of buffer)

Return Value:

    Pointer to the destination string

--*/
{
    strncpy(pszDest, pszSource, count);
    pszDest[count - 1] = 0;              

    return pszDest;
}



static
MI_MEMORY 
DIAMONDAPI LzxAlloc(
    ULONG amount
    )
{
    return(malloc((int) amount));
}


static
void DIAMONDAPI LzxFree(
    MI_MEMORY pointer
    )
{
    free(pointer);
}


static HANDLE s_hWriteFile = INVALID_HANDLE_VALUE;


static
int
DIAMONDAPI
CompressCallback(
    void* ,
    unsigned char* compressed_data,
    long compressed_size,
    long uncompressed_size
    )
{
    ULONG BytesWritten;
    LZXBOX_BLOCK Block;

    TRACE_OUT(TRACE_SPEW, "Writing compressed data: was=%d is=%d", uncompressed_size, compressed_size);


    //
    // write block header
    //
    Block.CompressedSize = (USHORT)compressed_size;
    Block.UncompressedSize = (USHORT)uncompressed_size;

    if (!WriteFile(s_hWriteFile, &Block, sizeof(Block), &BytesWritten, NULL)) {
        ERROR_OUT("Cannot write size to file");
        return -1;
    }

    //
    // write compressed data
    //
    if (!WriteFile(s_hWriteFile, compressed_data, compressed_size, &BytesWritten, NULL)) {
        ERROR_OUT("Cannot write data to file");
        return -1;
    }

    return 0;
}


BOOL CompressFile(
    CMemFile &SourceFile,
    LPCSTR pszCompressName
    )
{
    BOOL bRet = FALSE;
    UINT SourceSize = LZX_CHUNK_SIZE;
    LCI_CONTEXT_HANDLE Handle = NULL;
    PVOID Source = NULL;
    LZXCONFIGURATION cfg;
    UINT DestSize;
    ULONG BytesRead;
    ULONG BytesCompressed;
    int LCIRet;
    ULONG BytesRemaining;
    LPBYTE FilePointer;

    //
    // initialize the compression engine
    //
    cfg.WindowSize = LZX_WINDOW_SIZE;
    cfg.SecondPartitionSize = 65536;

    LCIRet = LCICreateCompression(&SourceSize, &cfg, LzxAlloc, LzxFree, &DestSize, &Handle,
        CompressCallback, NULL);
    if (LCIRet != MCI_ERROR_NO_ERROR)
    {
        ERROR_OUT("Cannot create compression %d", LCIRet);
        Handle = NULL;
        goto Cleanup;
    }

    Source = LzxAlloc(SourceSize);

    //
    // open files
    //
    s_hWriteFile = CreateFile(pszCompressName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (s_hWriteFile == INVALID_HANDLE_VALUE) {
        ERROR_OUT("Cannot open %s for writing", pszCompressName);
        goto Cleanup;
    }

    //
    // perform the compression
    //
    BytesRemaining = SourceFile.Size();
    FilePointer = (LPBYTE)SourceFile.Ptr();
    (void)LCIFlushCompressorOutput(Handle);
    while (BytesRemaining > 0) {

        BytesRead = (BytesRemaining > SourceSize) ? SourceSize : BytesRemaining;

        CopyMemory(Source, FilePointer, BytesRead);

        BytesRemaining -= BytesRead;
        FilePointer += BytesRead;

        LCIRet = LCICompress(Handle, Source, BytesRead, NULL, DestSize, &BytesCompressed);
        if (LCIRet != MCI_ERROR_NO_ERROR)
        {
            ERROR_OUT("Compression failed %d", LCIRet);
            goto Cleanup;
        }

    }
    (void)LCIFlushCompressorOutput(Handle);

    bRet = TRUE;
    // fall through

Cleanup:

    if (Handle != NULL)
    {
        (void)LCIDestroyCompression(Handle);
    }

    if (s_hWriteFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(s_hWriteFile);
        s_hWriteFile = INVALID_HANDLE_VALUE;
    }

    if (Source != NULL)
    {
        LzxFree(Source);
    }
    
    return bRet;
}


BOOL
ReadPassword(
    LPCSTR pszPrompt, 
    LPSTR pszPassword
    )
{
    int c;
    int n = 0;

    printf(pszPrompt);

    for (;;)
    {
         c = _getch();

         //
         // ^C
         //
         if (c == 0x3)
         {
             putchar('\n');
             ExitProcess(0);
         }

         //
         // Backspace
         //
         if (c == 0x8)
         {
             if (n)
             {
                n--;
                _putch(0x8);
                _putch(' ');
                _putch(0x8);
             }
             continue;
         }

         //
         // Return
         //
         if (c == '\r')
             break;

         if ((n + 1) < PASSWORD_MAXSIZE) 
         {
             pszPassword[n++] = (char)c;
             _putch('*');
         }
    }

    pszPassword[n] = 0;
    putchar('\n');

    if (strlen(pszPassword) < PASSWORD_MINLEN) 
    {
        ERROR_OUT("Password must at least be %d characters", PASSWORD_MINLEN);
        pszPassword[0] = 0;
        return FALSE;        
    }

    return TRUE;
} 


BOOL 
ReadKeyFromEncFile(
    LPCSTR pszFN, 
    LPCSTR pszPassword,
    PBYTE  pKeyBuffer,
    DWORD  cbKeyBuffer,
    BOOL   SymKey
    )
{
    BOOL bRet = FALSE;
    DWORD MsgLen;
    PBYTE pKeyData;
    PXBOX_KEYFILE_HEADER pKeyHeader;

    if (XcGetEncryptedFileMsgLen(pszFN, &MsgLen)) {

        //
        // Allocate memory for the plain text message
        //
        pKeyData = (PBYTE)malloc(MsgLen);


        //
        // Read the message from encrypted file
        //

        if (XcReadEncryptedFile(pszFN, pszPassword, pKeyData, MsgLen) == TRUE) {
        
            pKeyHeader = (PXBOX_KEYFILE_HEADER)pKeyData;
    
            if (SymKey) 
            {
                //
                // We are interested in Symmetric keys
                //
    
                if (pKeyHeader->KeyType != KEYFILE_SYM) 
                {
                    ERROR_OUT("Key file does not have a symmetric key");
                }
                else if ((DWORD)(pKeyHeader->KeyBits / 8) > cbKeyBuffer) 
                {
                    ERROR_OUT("Symmetric Key is too large for ROMBLD");
                }
                else
                {
                    //
                    // Copy the symmetric key out of the XCK file
                    //
                    memcpy(pKeyBuffer, 
                           pKeyData + sizeof(XBOX_KEYFILE_HEADER) + sizeof(BSAFE_PUB_KEY),
                           pKeyHeader->KeyBits / 8);

                    bRet = TRUE;
                }
            }
            else
            {     
                //
                // We are interested in public key portion of pub/prv key
                //
                if (pKeyHeader->KeyType != KEYFILE_PUBPAIR && pKeyHeader->KeyType != KEYFILE_PUBONLY) 
                {
                    ERROR_OUT("Key file does not have a public key");
                }
                else if ((DWORD)(pKeyHeader->PublicKeyBytes) > cbKeyBuffer) 
                {
                    ERROR_OUT("Public Key is too large for ROMBLD");
                }
                else
                {
                    //
                    // Copy the public key out of the XCK file
                    //
                    memcpy(pKeyBuffer, 
                           pKeyData + sizeof(XBOX_KEYFILE_HEADER),
                           pKeyHeader->PublicKeyBytes);
    
                    bRet = TRUE;
                }
    
            }

        }

        free(pKeyData);
    }
    else {
        ERROR_OUT("File %s is not an encrypted file", pszFN);
    }


    return bRet;
}


BOOL 
IsEncKeyFile(
    LPCSTR pszFN
    )
{
    DWORD MsgLen;

    return XcGetEncryptedFileMsgLen(pszFN, &MsgLen);
}


VOID
BuildFilename(
    LPSTR pszFullFN,
    LPCSTR pszPath,
    LPCSTR pszFN
    )
{
    int len;

    strcpy(pszFullFN, pszPath);
    
    len = strlen(pszFullFN);
    if (pszFullFN[len - 1] != '\\') 
    {
        pszFullFN[len] = '\\';
        pszFullFN[len + 1] = '\0';
    }
    strcat(pszFullFN, pszFN);
}


VOID
DumpBytes(
    LPCSTR pszLabel, 
    PBYTE pbData, 
    DWORD dwDataLen
    )
{
    DWORD i;

    printf("    %s (%d bytes):\n", pszLabel, dwDataLen);
    printf("    {\n");

    for (i = 0; i < dwDataLen; i++) {
        if (i % 8 == 0) {
           printf("           ");
        }

        printf("0x%02X", pbData[i]);

        if ((i + 1) != dwDataLen)
            printf(",");
        if ((i + 1) % 8 == 0)
            printf("\n");
    }
    printf("\n    }\n\n");
}

