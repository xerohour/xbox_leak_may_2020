//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xzip.h
//
//  Description:    Functions to compress/decompress X files with mszip.
//
//  History:
//      02/16/98    CongpaY     Created
//
//**************************************************************************

#ifndef _XZIP_H_
#define _XZIP_H_

class UnCompressedFile
{
public:
    DWORD m_dwMagic;
    DWORD m_dwVersion;
    DWORD m_dwFormat;
    DWORD m_dwFloatSize;
    BYTE  m_pData[1];
};

class CmpBlock
{
public:
    WORD m_cbOrg;
    WORD m_cbData;
    BYTE m_pData[1];
};

class CompressedFile
{
public:
    DWORD m_dwMagic;
    DWORD m_dwVersion;
    DWORD m_dwFormat;
    DWORD m_dwFloatSize;
    DWORD m_dwOrgFileSize;
    CmpBlock m_pCmpBlock[1];
};

class ZipDecompressor
{
    LPVOID    m_nfmContext;
    MI_MEMORY m_bufDest;

public:
    ZipDecompressor();
    ~ZipDecompressor();

    BOOL Init();

    HRESULT Decompress(LPVOID pvSrc, DWORD cbSrc,
                       LPVOID *ppvDest, LPDWORD pcbDest);
};

#endif // _XZIP_H_
