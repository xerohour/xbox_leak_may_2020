//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xzip.cpp
//
//  Description:    Implement functions to compress/decompress X files with
//                  mszip (nfmcomp.* and nfmdeco.*).
//
//  History:
//      02/16/98    CongpaY     Created
//
//**************************************************************************

#include "precomp.h"

extern "C" {
#include "nfmcomp.h"
#include "nfmdeco.h"
}

#include "xzip.h"

#define DXFILE_HEADER_LEN   16
#define BLOCKSIZE           0x8000
#define CMPBLOCKSIZE_MAX    BLOCKSIZE+MAX_GROWTH

static MI_MEMORY FAR __cdecl MyAlloc(ULONG amount)
{
    MI_MEMORY p;
    if (FAILED(XMalloc(&p, amount)))
        p = NULL;
    return p;
}

static void FAR __cdecl MyFree(MI_MEMORY pointer)
{
    XFree(pointer);
}

ZipDecompressor::ZipDecompressor()
  : m_nfmContext(NULL),
    m_bufDest(NULL)
{
}

ZipDecompressor::~ZipDecompressor()
{
    if (m_bufDest)
        XFree(m_bufDest);

    if (m_nfmContext)
        NFMdeco_destroy(m_nfmContext, MyFree);
}

BOOL ZipDecompressor::Init()
{
    if (FAILED(XMalloc(&m_bufDest, BLOCKSIZE)))
        return FALSE;

    m_nfmContext = NFMdeco_create(MyAlloc);

    return m_nfmContext != NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Compressed X File format:
//
// 16 bytes        Header : "xof 0303bzip0032" or "xof 0303tzip0032"
// DWORD           FileSize (For allocating memory for decompressed data.)
// WORD            Uncompressed chunk size cbOrg. (required by decompressor)
// WORD            Compressed chunk size cbCmp.
// cbCmp bytes     Compressed data.
// ...             Repeat of the last 3 items.
//
///////////////////////////////////////////////////////////////////////////////

HRESULT ZipDecompressor::Decompress(LPVOID pvSrc, DWORD cbSrc,
                                    LPVOID *ppvDest, LPDWORD pcbDest)
{
    CompressedFile *pSrcFile = (CompressedFile *)pvSrc;

    // Get file size.

    *pcbDest = pSrcFile->m_dwOrgFileSize;

    // Allocate memory for decompressed data.

    if (FAILED(XMalloc(ppvDest, *pcbDest)))
        return DXFILEERR_BADALLOC;

    // Copy over header.

    UnCompressedFile *pDestFile = (UnCompressedFile *)(*ppvDest);

    memcpy(pDestFile, pSrcFile, DXFILE_HEADER_LEN);

    // Convert header file format field.

    if (pDestFile->m_dwFormat == XFILE_FORMAT_BINARY_ZIP)
        pDestFile->m_dwFormat = XFILE_FORMAT_BINARY;
    else if (pDestFile->m_dwFormat == XFILE_FORMAT_TEXT_ZIP)
        pDestFile->m_dwFormat = XFILE_FORMAT_TEXT;

    // Prepare for the loop.

    CmpBlock *pCmpBlock = pSrcFile->m_pCmpBlock;

    LPBYTE pDest = pDestFile->m_pData;

    LPBYTE pEnd = (LPBYTE)pvSrc + cbSrc;

    HRESULT hr = NOERROR;

    while ((LPBYTE)pCmpBlock < pEnd) {
        // Don't change m_bufDest content.

        if (NFM_Prepare(m_nfmContext,
                        pCmpBlock->m_pData, pCmpBlock->m_cbData,
                        (LPBYTE)m_bufDest, BLOCKSIZE) != NFMsuccess) {
            hr = E_FAIL;
            break;
        }

        UINT cbDestRet = pCmpBlock->m_cbOrg;

        if (NFM_Decompress(m_nfmContext, &cbDestRet) != NFMsuccess ||
            cbDestRet != pCmpBlock->m_cbOrg) {
            hr = E_FAIL;
            break;
        }

        memcpy (pDest, m_bufDest, pCmpBlock->m_cbOrg);
        pDest += pCmpBlock->m_cbOrg;
        pCmpBlock = (CmpBlock *)(pCmpBlock->m_pData + pCmpBlock->m_cbData);
    }

    if (FAILED(hr))
        XFree(*ppvDest);

    return hr;
}

HRESULT DecompressFile_Zip(XSource *pSrc, XSrcMemory **ppSrcMem)
{
    HRESULT hr;

    ZipDecompressor decompressor;

    if (!decompressor.Init())
        return DXFILEERR_BADALLOC;

    LPVOID pvSrc, pvDest;
    DWORD cbSrc, cbDest;

    if (FAILED(hr = pSrc->GetFile(&pvSrc, &cbSrc)))
        return hr;

    if (FAILED(hr = decompressor.Decompress(pvSrc, cbSrc, &pvDest, &cbDest)))
        return hr;

    *ppSrcMem = new XSrcMemory(pvDest, cbDest, TRUE);

    if (!*ppSrcMem)
        return DXFILEERR_BADALLOC;

    return DXFILE_OK;
}

