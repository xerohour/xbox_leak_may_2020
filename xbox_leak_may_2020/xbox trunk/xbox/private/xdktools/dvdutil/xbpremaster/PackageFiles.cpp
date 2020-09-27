// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      PackageFiles.cpp
// Contents:  Contains code related to creation and expansion of Package files.
// Revisions: 6-Sep-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"

// xboxverp.h       -- Necessary to get xbPremaster version stamp.
#include <xboxverp.h>

extern int g_nMinAppLevel;

extern "C"
{
#include <lci.h>
#include <ldi.h>
}
#include "xcrypt.h"
#include "xsum.h"

//
// Constants for LZX compression
//

#define LZX_WINDOW_SIZE     (128*1024)
#define LZX_CHUNK_SIZE      (32*1024)
#define LZX_WORKSPACE       (256*1024)

//
// This structure is the header for each compression block
//
typedef struct _LZXBOX_BLOCK
{
    USHORT  CompressedSize;
    USHORT  UncompressedSize;

} LZXBOX_BLOCK, *PLZXBOX_BLOCK;


DWORD g_dwPackageChecksum;
extern char g_szPackageExtractFolder[MAX_PATH];


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES, ENUMS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// BUFFER_SIZE      -- Size of read and write buffers for compression/expansion
#define BUFFER_SIZE 32768

// eFileType        -- The different types of files that can be added to a
//                     Package.  Necessary since FLD and FST files require
//                     special handling.
typedef enum eFileType {FILE_FLD, FILE_FST, FILE_OTHER};

#define PACKAGE_SIG     "XBOX_PACKAGE_FILE"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ STRUCTURES, CLASSES +++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// sPackageHeader   -- Header structure for Package files
struct sPackageHeader
{
    // rgbySig              -- Signature specifying that its a package file
    BYTE rgbySignature[17];
    
    // rgbyPremasterVersion -- Contains version of xbPremaster used to generate
    //                         the package.
    BYTE rgbyPremasterVersion[8];

    // dwChecksum           -- Checksums the uncompressed data placed in the 
    //                         package
    DWORD dwChecksum;
};

// sPackageFileHeader -- Header structure for each file in a Package file.
struct sPackageFileHeader
{
    // szFilePathAndName    -- Relative path/file name of the file
    char szFilePathAndName[MAX_PATH];

    // uliFileSize          -- Size of the file (compressed) in the Package.
    ULARGE_INTEGER uliFileSize;
};

class CPackage
{
public:
    // CPackage         -- CPackage constructor
    CPackage(CStep_Progress *pstep, CDlg *pdlg, char *szFilePathName,
             bool fOpenForExpand);

    // Close            -- Closes the package file
    void Close();

    // Add              -- Adds (and compresses) a new file to the package
    HRESULT Add(char *szName, char *szRoot, eFileType filetype = FILE_OTHER);

    // DecodeNextFile   -- Decodes (and writes out) the next file in the package
    bool DecodeNextFile(char *szExtractDir, HRESULT *phr);

    // WriteChecksum    -- Writes the checksum value to the package header.
    HRESULT WriteChecksum();

    // m_hrInit         -- Result of the initialization
    HRESULT m_hrInit;

    // m_hPackageFile   -- Handle to the package file itself.
    HANDLE m_hPackageFile;

    // m_xsum           -- The checksum of all data (in uncompressed form) read
    //                     or written so far.
    CCheckSum m_xsum;

    // m_pp            -- The package header for this package
    sPackageHeader m_ph;

    int CPackage::CompressionCallback(void *pfol, unsigned char *compressed_data,
                                      long compressed_size, long uncompressed_size);

    ULARGE_INTEGER m_uliBytesRead;
    CDlg *m_pdlg;
    CStep_Progress *m_pstep;

private:

    sPackageFileHeader m_pfh;

    char m_szName[MAX_PATH];
    char m_szCurFile[MAX_PATH];

    bool m_fOpenForExpand;
};

CPackage *g_ppkgCur;

LONG CompressedSize, UncompressedSize;
int __cdecl CompressionCallback(void * pfol, unsigned char * compressed_data,
                        long compressed_size, long uncompressed_size)
{
    return g_ppkgCur->CompressionCallback(pfol, compressed_data,
                                           compressed_size, uncompressed_size);
}


int CPackage::CompressionCallback(void *pfol, unsigned char *compressed_data,
                                  long compressed_size, long uncompressed_size)
{
    LZXBOX_BLOCK Block;

    //
    // Write block header
    //

    Block.CompressedSize = (USHORT)compressed_size;
    Block.UncompressedSize = (USHORT)uncompressed_size;

    CompressedSize += sizeof Block + compressed_size;
    UncompressedSize += uncompressed_size;

    ULONG BytesWritten;
    if (!WriteFile(m_hPackageFile, &Block, sizeof(Block), &BytesWritten, NULL)) {
        return -1;
    }

    //
    // Write compressed data
    //

    if (!WriteFile(m_hPackageFile, compressed_data, compressed_size, &BytesWritten, NULL)) {
        return -1;
    }

    // Update the progress bar
    m_uliBytesRead.QuadPart += uncompressed_size;
    m_pdlg->SetProgressPos(IDC_PROGRESS_PROGRESSBAR, (int)(m_uliBytesRead.QuadPart / 1024));

    m_pfh.uliFileSize.QuadPart += uncompressed_size;
    // Check if we should cancel
    if (m_pstep->CheckCancelled())
        return -1;

    return 0;
}

MI_MEMORY
DIAMONDAPI
LzxAlloc(
    ULONG cb
    )
{
    return LocalAlloc( LPTR, cb );
}

VOID
DIAMONDAPI
LzxFree(
    MI_MEMORY pv
    )
{
    LocalFree( pv );
}

DWORD CompressFile(HANDLE hfileSrc, SIZE_T sizeCurFile, CCheckSum *pxsum)
{
    UINT DestSize;
    int LciReturn;
    UINT SourceSize;
    ULONG_PTR Source;
    DWORD dw;
    LZXCONFIGURATION cfg;
    LCI_CONTEXT_HANDLE Handle;
    SIZE_T BytesRead, BytesRemaining;
    SIZE_T BytesCompressed;

    SourceSize = LZX_CHUNK_SIZE;
    cfg.WindowSize = LZX_WINDOW_SIZE;
    cfg.SecondPartitionSize = 64 * 1024;

    LciReturn = LCICreateCompression(&SourceSize, &cfg, LzxAlloc, LzxFree,
                                      &DestSize, &Handle, CompressionCallback,
                                      NULL);

    if (LciReturn != MCI_ERROR_NO_ERROR) {
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    BytesRemaining = sizeCurFile;
    DWORD dwSourceSize = 32768;
    Source = (ULONG_PTR)malloc(dwSourceSize);
    BytesRead = (BytesRemaining > SourceSize) ? SourceSize : BytesRemaining;

    while (BytesRemaining) {
        if (BytesRead > dwSourceSize)
        {
            dwSourceSize = BytesRead;
            Source = (ULONG_PTR)realloc((void*)Source, dwSourceSize);
        }
        // Read the next block from the source file
        if (!ReadFile(hfileSrc, (void*)Source, BytesRead, &dw, NULL))
            return ERROR_GEN_FAILURE;
        LciReturn = LCICompress(Handle, (PVOID)Source, BytesRead, NULL,
                                 DestSize, &BytesCompressed);

        pxsum->SumBytes((BYTE*)Source, BytesRead);

        if (LciReturn != MCI_ERROR_NO_ERROR)
            return ERROR_GEN_FAILURE;

        BytesRemaining -= BytesRead;
        BytesRead = (BytesRemaining > SourceSize) ? SourceSize : BytesRemaining;
    }

    LCIFlushCompressorOutput(Handle);
    LCIDestroyCompression(Handle);

    free((void*)Source);

    return BytesRemaining ? ERROR_GEN_FAILURE : ERROR_SUCCESS;
}

DWORD DecompressFile(HANDLE hfileDest, ULARGE_INTEGER uliOutputFileSize, HANDLE hfileSrc)
{
    UINT DestSize;
    UINT SourceSize;
    ULONG_PTR Source;
    ULONG_PTR Dest;
    DWORD dw;
    LZXDECOMPRESS Decomp;
    LCI_CONTEXT_HANDLE Handle;
    UINT BytesDecompressed;
    LZXBOX_BLOCK Block;

    SourceSize = LZX_CHUNK_SIZE;
    Decomp.fCPUtype = LDI_CPU_80386;
    Decomp.WindowSize = LZX_WINDOW_SIZE;

    if (LDICreateDecompression(&SourceSize, &Decomp, LzxAlloc, LzxFree, &DestSize,
        &Handle, NULL,NULL,NULL,NULL,NULL) != MDI_ERROR_NO_ERROR)
    {
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    DWORD dwSrcSize = 32768;
    DWORD dwDestSize = 32768;
    Source = (ULONG_PTR)malloc(dwSrcSize);
    Dest = (ULONG_PTR)malloc(dwDestSize);

    while (uliOutputFileSize.QuadPart)
    {
        // Read the compression block
        if (!ReadFile(hfileSrc, (void*)&Block, sizeof(LZXBOX_BLOCK), &dw, NULL))
            return ERROR_GEN_FAILURE;

        if (Block.CompressedSize > dwSrcSize)
        {
            dwSrcSize = Block.CompressedSize;
            Source = (ULONG_PTR)realloc((void*)Source, dwSrcSize);
        }
        if (Block.UncompressedSize > dwDestSize)
        {
            dwDestSize = Block.UncompressedSize;
            Dest = (ULONG_PTR)realloc((void*)Dest, dwDestSize);
        }

        // read the compressed data
        if (!ReadFile(hfileSrc, (void*)Source, Block.CompressedSize, &dw, NULL))
            return ERROR_GEN_FAILURE;

        // Perform decompression
        BytesDecompressed = Block.UncompressedSize;
        if (LDIDecompress(Handle, (PVOID)Source, Block.CompressedSize, (PVOID)Dest,
            &BytesDecompressed) != MDI_ERROR_NO_ERROR)
        {
            LDIDestroyDecompression(Handle);
            return ERROR_GEN_FAILURE;
        }

        // output the decompressed data
        if (!WriteFile(hfileDest, (void*)Dest, BytesDecompressed, &dw, NULL))
            return ERROR_GEN_FAILURE;

        // update the checksum
        g_ppkgCur->m_xsum.SumBytes((BYTE*)Dest, BytesDecompressed);

        // Update the progress bar
        g_ppkgCur->m_uliBytesRead.QuadPart += BytesDecompressed;
        g_ppkgCur->m_pdlg->SetProgressPos(IDC_PROGRESS_PROGRESSBAR,
                             (int)(g_ppkgCur->m_uliBytesRead.QuadPart / 1024));

        // Check if we should cancel
        if (g_ppkgCur->m_pstep->CheckCancelled())
        {
            LDIDestroyDecompression(Handle);
            return ERROR_GEN_FAILURE;
        }
        
        
        //
        // Advance the pointers
        //
        uliOutputFileSize.QuadPart -= (__int64)BytesDecompressed;
    }

    LDIDestroyDecompression(Handle);

    free((void*)Source);
    free((void*)Dest);

    return uliOutputFileSize.QuadPart ? ERROR_GEN_FAILURE : ERROR_SUCCESS;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CPackage::CPackage
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CPackage::CPackage(CStep_Progress *pstep, CDlg *pdlg, char *szFilePathName, bool fOpenForExpand)
{
    m_hrInit = E_FAIL;
    m_pdlg = pdlg;
    m_pstep = pstep;

    // Track why this package was opened (expanding or compacting)
    m_fOpenForExpand = fOpenForExpand;

    strcpy(m_szName, szFilePathName);

    // Create the file that will contain all of the compressed files
    if (fOpenForExpand)
        m_hPackageFile = CreateFile(szFilePathName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL, 0);
    else
        m_hPackageFile = CreateFile(szFilePathName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL, 0);
    if (m_hPackageFile == INVALID_HANDLE_VALUE)
    {
        // Failed to create the file for some reason.
        // Figure out why we died
        DWORD dwErr = GetLastError();
        switch(dwErr)
        {
        case ERROR_DISK_FULL:
            MessageBox(m_pdlg->GetHwnd(), "The specified drive is full.  "
                       "Please free some space on it or select another drive, "
                       "and try again.", "Insufficient disk space",
                       MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            return;

        case ERROR_FILE_NOT_FOUND:
            MessageBox(m_pdlg->GetHwnd(), "The specified package could not "
                       "be found.", "Package not found",
                       MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            return;

        case ERROR_ACCESS_DENIED:
            MessageBox(m_pdlg->GetHwnd(), "Cannot access Package.  Please ensure it is not in use.", "Package not accessible",
                       MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            return;

        default:
            MessageBox(m_pdlg->GetHwnd(), "xbPremaster cannot create the "
                       "package at the specified location.  Please check the "
                       "device and try again.", "Cannot create package",
                       MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            return;
        }
    }

    if (fOpenForExpand)
    {
        // Verify it's a valid package file
        DWORD dwRead;
        if (!ReadFile(m_hPackageFile, &m_ph, sizeof m_ph, &dwRead, 0))
            return;

        if (memcmp((char*)m_ph.rgbySignature, PACKAGE_SIG, strlen(PACKAGE_SIG)))
        {
            MessageBox(m_pdlg->GetHwnd(), "Invalid Package file specified",
                       "Not a package file",
                       MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            return;
        }
    }
    else
    {
        sPackageHeader ph;
        DWORD dwWritten;
        
        // Add package header information
        memcpy(ph.rgbySignature, PACKAGE_SIG, strlen(PACKAGE_SIG));
            
        int nMajor, nMinor, nBuild, nQFE;
        sscanf(VER_PRODUCTVERSION_STR, "%d.%d.%d.%d", &nMajor, &nMinor,
                                                      &nBuild, &nQFE);
        *(WORD*)(&ph.rgbyPremasterVersion[0]) = (WORD)nMajor;
        *(WORD*)(&ph.rgbyPremasterVersion[2]) = (WORD)nMinor;
        *(WORD*)(&ph.rgbyPremasterVersion[4]) = (WORD)nBuild;
        *(WORD*)(&ph.rgbyPremasterVersion[6]) = (WORD)nQFE;

        if (!WriteFile(m_hPackageFile, &ph, sizeof ph, &dwWritten, 0))
            return;
    }

    // Need to keep track of the total number of bytes read so that we can
    // update the progress bar accordingly.
    m_uliBytesRead.QuadPart = 0;

    // If here, then the file was successfully created
    m_hrInit = S_OK;
}

void CPackage::Close()
{
    if (m_hPackageFile)
        CloseHandle(m_hPackageFile);
}

void GetFilePointer(HANDLE hfile, LARGE_INTEGER *puliPos)
{
    puliPos->QuadPart = 0;
    puliPos->LowPart = SetFilePointer(hfile, 0, (long*)&puliPos->HighPart, FILE_CURRENT);
}
void SetFilePointer(HANDLE hfile, LARGE_INTEGER *puliPos)
{
    ::SetFilePointer(hfile, puliPos->LowPart, (long*)&puliPos->HighPart, FILE_BEGIN);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CPackage::DecodeNextFile
// Purpose:   
// Arguments: 
// Return:    'false' of eof, 'true' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CPackage::DecodeNextFile(char *szExtractDir, HRESULT *phr)
{
    char sz[MAX_PATH];
    DWORD dwRead;

    assert(m_hPackageFile);

    // Get the next file header
    if (!ReadFile(m_hPackageFile, &m_pfh, sizeof m_pfh, &dwRead, 0))
    {
        *phr = E_FAIL;
        return false;
    }
    if (dwRead == 0)
    {
        *phr = S_OK;
        return false;
    }

    // Change dir to the root extraction dir
    SetCurrentDirectory(szExtractDir);

    // Create the directory(s) necessary to place the file
    strcpy(sz, m_pfh.szFilePathAndName);
    char *pch = strtok(sz, "\\/");
    char *pchFileName = strrchr(m_pfh.szFilePathAndName, '/');
    if (!pchFileName)
        pchFileName = strrchr(m_pfh.szFilePathAndName, '\\');
    if (!pchFileName)
    {
        // File's in the root
        pchFileName = m_pfh.szFilePathAndName;
    }
    else
    {
        pchFileName++; // Skip the slash
        while(pch && strcmp(pch, pchFileName))
        {
            if (!CreateDirectory(pch, 0))
            {
                // It's valid for createdirectory to fail if the dir was already there;
                // however, it could be that we're out of space...
                if (GetLastError() == ERROR_DISK_FULL)
                {
                    MessageBox(m_pdlg->GetHwnd(), "Not enough disk space on system drive to expand package",
                               "Not enough space", MB_ICONWARNING | MB_APPLMODAL | MB_OK);
                    *phr = E_FAIL;
                    return true;
                }
            }
            SetCurrentDirectory(pch);
            // Get next token
            pch = strtok(NULL, "\\/");
        }
    }

    // Create the to-be-decompressed file
    HANDLE hCurFile = CreateFile(pchFileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, 0);
    if (hCurFile == INVALID_HANDLE_VALUE)
    {
        // Failed to create the file for some reason.
        // Figure out why we died
        DWORD dwErr = GetLastError();
        switch(dwErr)
        {
        case ERROR_DISK_FULL:
            MessageBox(m_pdlg->GetHwnd(), "The specified drive is full.  "
                       "Please free some space on it or select another drive, "
                       "and try again.", "Insufficient disk space",
                       MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            *phr = E_FAIL;
            return true;

        case ERROR_ACCESS_DENIED:
            MessageBox(m_pdlg->GetHwnd(), "Cannot access file.  Please ensure it is not in use.", "File not accessible",
                       MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            *phr = E_FAIL;
            return true;

        default:
            MessageBox(m_pdlg->GetHwnd(), "xbPremaster cannot decompress the "
                       "package.  Please check the "
                       "package and try again.", "Cannot create package",
                       MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            *phr = E_FAIL;
            return true;
        }
    }

    *phr = DecompressFile(hCurFile, m_pfh.uliFileSize, m_hPackageFile);

    // We're now done with the file
    CloseHandle(hCurFile);
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CPackage::WriteChecksum
// Purpose:   Writes the uncompressed-data checksum value to the package file.
// Arguments: None
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CPackage::WriteChecksum()
{ 
    sPackageHeader ph;
    DWORD          dwChecksum = m_xsum.DwFinalSum(); 
    LARGE_INTEGER uli;
    DWORD dwWritten;

    // Calculate offset to the checksum data in the package header
     uli.QuadPart = (BYTE*)&ph.dwChecksum - (BYTE*)&ph;
 
     SetFilePointer(m_hPackageFile, &uli);
     if (!WriteFile(m_hPackageFile, (BYTE*)&dwChecksum, 4, &dwWritten , 0))
         return E_FAIL;

    g_dwPackageChecksum = dwChecksum;
    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CPackage::Add
// Purpose:   
// Arguments: 
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CPackage::Add(char *szName, char *szRoot, eFileType filetype)
{
    char sz[MAX_PATH + 200];
    DWORD error;
    HRESULT hr = E_FAIL;
    HANDLE hCurFile;
    strcpy(m_szCurFile, szName);

    // Verify that any xbes attached are linked only to approved libraries
    if (!lstrcmpi(szName + strlen(szName) - 4, ".xbe"))
    {
       // Create a new CXBEFile to point at the file
        CFileStmRand *pstm;
        CFile *pfileNext = new CFile(m_pdlg, m_pstep, szName);
        if (!pfileNext->IsInited())
        {
            // Failed to load the specified FST object
            hr = pfileNext->GetInitCode();
            pfileNext->InformError(m_pdlg->GetHwnd(), hr, pfileNext->GetFilename());
            if (pfileNext)
                delete pfileNext;
            goto done;
        }
        pfileNext->CreateFileStm(&pstm);
        CXBEFile xf(pstm);
        int nLibApprovalLevel = xf.GetLibApprovalLevel();
        g_nMinAppLevel = min(g_nMinAppLevel, nLibApprovalLevel);

        if (nLibApprovalLevel <= 1)
        {
            // uh oh, we don't support it.  Warn the user

            // Need to allocate more space for the list of invalid
            // xbe names?
            if (g_cInvalidXbes == g_cMaxInvalidXbes)
            {
                g_cMaxInvalidXbes += BATCH;
                g_rgstrInvalidXbeName = (sString*)realloc(
                                g_rgstrInvalidXbeName,
                                g_cMaxInvalidXbes*sizeof(sString));
            }

            strcpy(g_rgstrInvalidXbeName[g_cInvalidXbes].sz, szName);
            sprintf(sz, "Warning: Invalid xbe encountered (%s)", szName);
            m_pdlg->SetControlText(IDC_PROGRESS_BADXBE, sz);
            g_cInvalidXbes++;
        }

        delete pstm;
        delete pfileNext;
    }
    
    // Open the file handle so that we can read from it for the encoding
    hCurFile = CreateFile(szName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, 0);
    if (hCurFile == INVALID_HANDLE_VALUE)
    {
        // Failed to open the file for some reason. Figure out why we died
        DWORD dwErr = GetLastError();
        switch(dwErr)
        {
        case ERROR_ACCESS_DENIED:
            sprintf(sz, "xbPremaster cannot access the file '%s'.  Please"
                       " ensure the file isn't in use and try again.", szName);
            MessageBox(m_pdlg->GetHwnd(), sz, "Cannot create package",
                       MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            return E_FAIL;

        default:
            sprintf(sz, "xbPremaster cannot open the file '%s'.  Please check the "
                       "device and try again.", szName);
            MessageBox(m_pdlg->GetHwnd(), sz, "Cannot create package",
                       MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            return E_FAIL;
        }
    }

    ULARGE_INTEGER uliSize;
    uliSize.LowPart = GetFileSize(hCurFile, &uliSize.HighPart);

    memset(&m_pfh, 0, sizeof(m_pfh));

    // We only want to store relative paths, so ignore the root
    switch(filetype)
    {
    case FILE_FLD:
        // Place FLD file in root (relative)
        strcpy(m_pfh.szFilePathAndName, "main.fld");
        break;
    case FILE_FST:
        // Place FST in _amc dir
        strcpy(m_pfh.szFilePathAndName, "_amc\\main.fst");
        break;
    case FILE_OTHER:
        strcpy(m_pfh.szFilePathAndName, szName + strlen(szRoot) + 1);
        break;
    }
    
    // Save the current file position so that we can restore it
    LARGE_INTEGER uliCurFilePos;
    LARGE_INTEGER uliCurFilePos2;
    DWORD dwWritten;
    
    GetFilePointer(m_hPackageFile, &uliCurFilePos);
    if (!WriteFile(m_hPackageFile, &m_pfh, sizeof(m_pfh), &dwWritten, 0))
        goto done;

    UncompressedSize = 0;
    error = CompressFile(hCurFile, uliSize.LowPart, &m_xsum);
    if (error != ERROR_SUCCESS)
        goto done;

    if(UncompressedSize != (long)uliSize.LowPart)
        goto done;

    // Write out the file header
    GetFilePointer(m_hPackageFile, &uliCurFilePos2);
    SetFilePointer(m_hPackageFile, &uliCurFilePos);
    if (!WriteFile(m_hPackageFile, &m_pfh, sizeof(m_pfh), &dwWritten, 0))
        goto done;
    SetFilePointer(m_hPackageFile, &uliCurFilePos2);

    hr = S_OK;

done:

    // We're now done with the file
    CloseHandle(hCurFile);

    // Return if a failure occurred -- the user has already been notified
    return hr;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::GetFSTFileSizes
// Purpose:   Gets size of all files in the specified FST object.  This is
//            necessary to properly update the progress bar.
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CStep_Progress::GetFSTFileSizes(CFST *pfst, ULARGE_INTEGER *puliSize)
{
    bool fHitGdfs = false;
    
    // Step over each file in the FST and calculate its size
    for (int iLayer = 0; iLayer <= 1; iLayer++)
    {
        int   nStartLSN, nEndLSN;
        DWORD dwOffset;
        char  szName[MAX_PATH];
        int   iObject = 0;

        while (true)
        {
            // Get the next object in the FST.
            if (!pfst->GetEntry(iLayer, iObject, szName, &nStartLSN, &nEndLSN, &dwOffset))
            {
                // no more objects on the current layer
                break;
            }

            // Only add first gdfs.bin here
            if (strlen(szName) > 8)
            {
                if (!lstrcmpi(szName + strlen(szName) - 8, "gdfs.bin"))
                    if (fHitGdfs)
                    {
                        iObject++;
                        continue;
                    }
                    else
                        fHitGdfs = true;
            }

            // At this point, szName contains the entire path to the file
            WIN32_FILE_ATTRIBUTE_DATA wfad;
            if (!GetFileAttributesEx(szName, GetFileExInfoStandard, &wfad))
                return false;

            ULARGE_INTEGER uli;
            uli.HighPart = wfad.nFileSizeHigh;
            uli.LowPart = wfad.nFileSizeLow;
            puliSize->QuadPart += uli.QuadPart;
            
            iObject++;
        }
    }
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::ExtractPackagedFiles
// Purpose:   
// Arguments: 
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CStep_Progress::ExtractPackagedFiles(char *szPackageName)
{
    HRESULT hr;
    char szTempDir[MAX_PATH], szTemp[400];
    ULARGE_INTEGER uliSize;

    // Create a temporary directory to hold the files.

    // Allow override via regkey.  Used for internal testing purposes only
    HKEY hkey;
    bool fGotTemp = false;

    if (g_szPackageExtractFolder[0])
    {
        strcpy(m_szPackageExtractFolder, g_szPackageExtractFolder);
        fGotTemp = true;
    }
    if (!fGotTemp)
    {
        if ((RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                          TEXT("SOFTWARE\\Microsoft\\XboxSDK\\xbPremaster"), 0,
                          KEY_READ, &hkey) == ERROR_SUCCESS))
        {
            DWORD dwSize = sizeof m_szPackageExtractFolder;
            if (RegQueryValueEx(hkey, "RenderTemp", NULL, NULL,
                                (BYTE*)m_szPackageExtractFolder, &dwSize) ==
                                ERROR_SUCCESS)
                fGotTemp = true;
            RegCloseKey(hkey);
        }
    }
    if (!fGotTemp)
    {
        GetTempPath(MAX_PATH, szTempDir);
        if (GetTempFileName(szTempDir, "xbp", 0, m_szPackageExtractFolder) == 0)
        {
            // Couldn't create the temporary directory
            sprintf(szTemp, "Failed to create temp directory for extraction of package"
                        " files.  Ensure your system drive ('%c':\\) has enough "
                        "free space available and that you have the appropriate "
                        "administrative permissions.", szTempDir[0]);
            MessageBox(m_pdlg->GetHwnd(), szTemp, "Could not extract package",
                       MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            strcpy(m_szPackageExtractFolder, "");
            m_fCancelled = true;
            return E_FAIL;
        }

        // GetTempFileName forces creation of a file with the temporary name; we
        // want a directory, so delete the created file and create a directory.
        DeleteFile(m_szPackageExtractFolder);
    }
    
    CreateDirectory(m_szPackageExtractFolder, 0);
    
    // Create the package from which all of the layout files will be created
    CPackage pkg(this, m_pdlg, m_szSourceFile, true);
    if (!SUCCEEDED(pkg.m_hrInit))
    {
        m_fCancelled = true;
        return pkg.m_hrInit;
    }

    // We track decompression progress by current position in the package file.
    uliSize.LowPart = GetFileSize(pkg.m_hPackageFile, &uliSize.HighPart);
    m_pdlg->SetProgressRange(IDC_PROGRESS_PROGRESSBAR, 0, (int)(uliSize.QuadPart / 1024));


    g_ppkgCur = &pkg;
    // Iterate the files in the package file and extract them to the temporary
    // directory.
    do
    {
        if (!pkg.DecodeNextFile(m_szPackageExtractFolder,&hr))
            break;
    } while (SUCCEEDED(hr) && !m_fCancelled);

    // Store the path\name of the extracted FLD file
    strcpy(m_szSourceFile, m_szPackageExtractFolder);
    strcat(m_szSourceFile, "\\main.fld");

    if (SUCCEEDED(hr))
    {
        // Compare the checksum stored in the package with the calculated checksum
        if (pkg.m_xsum.DwFinalSum() != pkg.m_ph.dwChecksum)
        {
            MessageBox(m_pdlg->GetHwnd(), "ERROR: Checksum does not match!",
                                          "Invalid Checksum", MB_ICONEXCLAMATION |
                                          MB_APPLMODAL | MB_OK);
            m_fCancelled = true;
            hr = E_FAIL;                                        
        }
        else
            g_dwPackageChecksum = pkg.m_ph.dwChecksum;
    }

    if (FAILED(hr))
    {
        CStep *pstepNext = new CStep_Completion(m_pdlg, m_source, m_media, m_szDestPath, m_szSourceFile,
                                                "Failed during package extraction");
        m_pdlg->SetNextStep(pstepNext);
    }

    return hr;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::PackageFiles
// Purpose:   Packages up all of the files in the layout for future premastering
// Arguments: 
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CStep_Progress::PackageFiles(CFST *pfst, char *szRoot, char *szFLDName,
                                     char *szFSTName, char *szBinName)
{
    WIN32_FILE_ATTRIBUTE_DATA wfad;
    ULARGE_INTEGER uliTotalSourceBytes;
    HRESULT hr;
    int iLayer;

    assert(pfst);
    assert(szBinName);

    // First, determine the total size of all files so that we can update the
    // progress bar accordingly
    uliTotalSourceBytes.QuadPart = 0;
    if (!GetFSTFileSizes(pfst, &uliTotalSourceBytes))
    {
        MessageBox(m_pdlg->GetHwnd(), "xbPremaster failed while writing the "
                                      "package file", "Cannot save file",
                    MB_ICONWARNING | MB_APPLMODAL | MB_OK);
        m_fCancelled = true;
        return E_FAIL;
    }

    // Add size of FLD file
    if (!GetFileAttributesEx(szFLDName, GetFileExInfoStandard, &wfad))
        return false;
    uliTotalSourceBytes.HighPart += wfad.nFileSizeHigh;
    uliTotalSourceBytes.LowPart  += wfad.nFileSizeLow;

    g_nMinAppLevel = 255;
    g_cInvalidXbes = 0;
    g_cMaxInvalidXbes = 0;
    g_rgstrInvalidXbeName = NULL;

    m_pdlg->SetProgressRange(IDC_PROGRESS_PROGRESSBAR, 0,
                             (int)(uliTotalSourceBytes.QuadPart / 1024));

    // Create the package to which all of the layout files will be added
    CPackage pkg(this, m_pdlg, m_szDestPath, false);
    if (!SUCCEEDED(pkg.m_hrInit))
    {
        m_fCancelled = true;
        return pkg.m_hrInit;
    }

    // Necessary for lzx compression callback function
    g_ppkgCur = &pkg;
    
    // Add the FLD to the package
    hr = pkg.Add(szFLDName, NULL, FILE_FLD);
    if (FAILED(hr) || m_fCancelled)
        goto Failure;

    // Add the BIN to the package
    hr = pkg.Add(szBinName, szRoot);
    if (FAILED(hr) || m_fCancelled)
        goto Failure;

    // Step over each file in the FST and add it to the package
    for (iLayer = 0; iLayer <= 1; iLayer++)
    {
        int   nStartLSN, nEndLSN;
        DWORD dwOffset;
        char  szName[MAX_PATH];
        int   iObject = 0;

        while (true)
        {
            // Get the next object in the FST.
            if (!pfst->GetEntry(iLayer, iObject, szName, &nStartLSN, &nEndLSN,
                                &dwOffset))
            {
                // no more objects on the current layer
                break;
            }
            iObject++;

            // Don't add gdfs.bin here
            if (strlen(szName) > 8 && !lstrcmpi(szName + strlen(szName) - 8,
                                                "gdfs.bin"))
                continue;

            // At this point, szName contains the entire path to the file
            hr = pkg.Add(szName, szRoot);
            if (FAILED(hr) || m_fCancelled)
                goto Failure;
        }
    }

    // Now that we're done with the FST, we can delete the pfst object so that
    // the file is unlocked so that we can add it to the package
    delete pfst;

    // Add the FST file to the package
    hr = pkg.Add(szFSTName, NULL, FILE_FST);
    if (FAILED(hr) || m_fCancelled)
        goto Failure;

    // Place the uncompressed-data checksum in the package's header
    hr = pkg.WriteChecksum();
    if (FAILED(hr) || m_fCancelled)
        goto Failure;

    // Success!
    goto done;

Failure:

    // Something failed - delete the package and inform caller that we're done.
    DeleteFile(m_szDestPath);
    m_fCancelled = true;

done:

    // All done!
    pkg.Close();

    return hr;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  VerifyPackageFile
// Purpose:   
// Arguments: 
// Return:    'true' if valid, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool VerifyPackageFile(char *szPKGFile)
{
    DWORD dwRead;
    sPackageHeader ph;

    // Open the specified package file
    HANDLE hfile = CreateFile(szPKGFile, GENERIC_READ, 0, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hfile == INVALID_HANDLE_VALUE)
        return false;

    // Get package header information
    if (!ReadFile(hfile, &ph, sizeof(ph), &dwRead, 0) || dwRead != sizeof(ph))
    {
        // Failed to read the header.
        CloseHandle(hfile);
        return false;
    }

    // Verify its a valid package file
    if (memcmp((char*)ph.rgbySignature, PACKAGE_SIG, strlen(PACKAGE_SIG)))
    {
        CloseHandle(hfile);
        return false;
    }

    CloseHandle(hfile);
    return true;
}