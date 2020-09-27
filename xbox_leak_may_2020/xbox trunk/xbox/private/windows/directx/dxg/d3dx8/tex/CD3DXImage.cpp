///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXImage.cpp
//  Content:    Image loader
//
///////////////////////////////////////////////////////////////////////////

#include "pchtex.h"
///////////////////////////////////////////////////////////////////////////
// From wingdi.h
///////////////////////////////////////////////////////////////////////////

typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER, FAR *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

#include <pshpack2.h>
typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
#include <poppack.h>

// The following two structures are used for defining RGB's in terms of CIEXYZ.
typedef long            FXPT2DOT30, FAR *LPFXPT2DOT30;

typedef struct tagCIEXYZ
{
        FXPT2DOT30 ciexyzX;
        FXPT2DOT30 ciexyzY;
        FXPT2DOT30 ciexyzZ;
} CIEXYZ;
typedef CIEXYZ  FAR *LPCIEXYZ;

typedef struct tagICEXYZTRIPLE
{
        CIEXYZ  ciexyzRed;
        CIEXYZ  ciexyzGreen;
        CIEXYZ  ciexyzBlue;
} CIEXYZTRIPLE;
typedef CIEXYZTRIPLE    FAR *LPCIEXYZTRIPLE;

typedef struct {
        DWORD        bV4Size;
        LONG         bV4Width;
        LONG         bV4Height;
        WORD         bV4Planes;
        WORD         bV4BitCount;
        DWORD        bV4V4Compression;
        DWORD        bV4SizeImage;
        LONG         bV4XPelsPerMeter;
        LONG         bV4YPelsPerMeter;
        DWORD        bV4ClrUsed;
        DWORD        bV4ClrImportant;
        DWORD        bV4RedMask;
        DWORD        bV4GreenMask;
        DWORD        bV4BlueMask;
        DWORD        bV4AlphaMask;
        DWORD        bV4CSType;
        CIEXYZTRIPLE bV4Endpoints;
        DWORD        bV4GammaRed;
        DWORD        bV4GammaGreen;
        DWORD        bV4GammaBlue;
} BITMAPV4HEADER, FAR *LPBITMAPV4HEADER, *PBITMAPV4HEADER;

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#ifndef BI_BITFIELDS
#define BI_BITFIELDS  3L
#endif  // BI_BITFIELDS
#define BI_JPEG       4L
#define BI_PNG        5L

typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;
typedef RGBQUAD FAR* LPRGBQUAD;

///////////////////////////////////////////////////////////////////////////
// CD3DXImage /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


CD3DXImage::CD3DXImage()
{
    m_Format         = D3DFMT_UNKNOWN;
    m_pvData         = NULL;
    m_pPalette       = NULL;
    m_bDeleteData    = FALSE;
    m_bDeletePalette = FALSE;
    m_pMip           = NULL;
    m_pFace          = NULL;
}


CD3DXImage::~CD3DXImage()
{
    if(m_pvData && m_bDeleteData)
        delete [] m_pvData;

    if(m_pPalette && m_bDeletePalette)
        delete [] m_pPalette;

    if(m_pMip)
        delete m_pMip;

    if(m_pFace)
        delete m_pFace;
}


HRESULT CD3DXImage::Load(LPCVOID pvData, DWORD cbData, D3DXIMAGE_INFO *pInfo)
{
    HRESULT hr;

    for(int i = 0; i < 7; i++)
    {
        switch(i)
        {
        case 0: hr = LoadBMP(pvData, cbData); break;
        case 1: hr = LoadPPM(pvData, cbData); break;
        case 2: hr = LoadDDS(pvData, cbData); break;
        case 3: hr = LoadJPG(pvData, cbData); break;
        case 4: hr = LoadPNG(pvData, cbData); break;
        case 5: hr = LoadTGA(pvData, cbData); break;
        case 6: hr = LoadDIB(pvData, cbData); break;
        }

        if(SUCCEEDED(hr))
            break;

        if(m_pvData && m_bDeleteData)
            delete [] m_pvData;

        if(m_pPalette && m_bDeletePalette)
            delete [] m_pPalette;

        if(m_pMip)
            delete m_pMip;

        if(m_pFace)
            delete m_pFace;

        m_pvData         = NULL;
        m_pPalette       = NULL;
        m_bDeleteData    = FALSE;
        m_bDeletePalette = FALSE;
        m_pMip           = NULL;
        m_pFace          = NULL;
    }

    if(7 == i)
    {
        DPF(0, "Unsupported file format");
        return D3DXERR_INVALIDDATA;
    }


    // Fill in info struct
    if(pInfo)
    {
        memset(pInfo, 0x00, sizeof(D3DXIMAGE_INFO));

        pInfo->Width     = (UINT) m_Rect.right;
        pInfo->Height    = (UINT) m_Rect.bottom;
        pInfo->Depth     = 1;
        pInfo->MipLevels = 1;
        pInfo->Format    = m_Format;

        for(CD3DXImage *pImage = this; pImage->m_pMip; pImage = pImage->m_pMip)
            pInfo->MipLevels++;
    }

    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// LoadBMP ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


HRESULT CD3DXImage::LoadBMP(LPCVOID pvData, DWORD cbData)
{
    // Examine header
    if(cbData < sizeof(BITMAPFILEHEADER))
        return E_FAIL;

    BITMAPFILEHEADER *pFH = (BITMAPFILEHEADER *) pvData;

    if(pFH->bfType != (('B') | ('M' << 8)) || pFH->bfSize > cbData)
        return E_FAIL;

    return LoadDIB((BYTE *) pvData + sizeof(BITMAPFILEHEADER), cbData - sizeof(BITMAPFILEHEADER));
}


HRESULT CD3DXImage::LoadDIB(LPCVOID pvData, DWORD cbData)
{
    UNALIGNED BITMAPINFOHEADER *pIH;
    DWORD_PTR dwWidth, dwHeight, dwOffset, dwClrUsed;

    if(cbData < sizeof(BITMAPINFOHEADER))
        return E_FAIL;

    pIH = (BITMAPINFOHEADER *) pvData;

    if(pIH->biSize < sizeof(BITMAPINFOHEADER))
        return E_FAIL;

    dwWidth   = (DWORD_PTR) (pIH->biWidth);
    dwHeight  = (DWORD_PTR) (pIH->biHeight > 0 ? pIH->biHeight : -pIH->biHeight);
    dwClrUsed = (DWORD_PTR) (pIH->biClrUsed);

    if((pIH->biBitCount <= 8) && (0 == dwClrUsed))
        dwClrUsed = (DWORD_PTR) (1 << pIH->biBitCount);

    dwOffset  = (DWORD_PTR) pIH->biSize + dwClrUsed * sizeof(DWORD);

    if(dwOffset > (DWORD_PTR) cbData)
        return E_FAIL;

    if(pIH->biPlanes != 1)
        return E_FAIL;


    // Only RGB and BITFIELD bitmaps can be inverted
    if(pIH->biHeight < 0 && pIH->biCompression != BI_RGB && pIH->biCompression != BI_BITFIELDS)
        return E_FAIL;


    // Compute format
    DWORD dwB, dwG, dwR, dwA;
    D3DFORMAT Format = D3DFMT_UNKNOWN;

    switch(pIH->biCompression)
    {
    case BI_RGB:
    case BI_RLE4:
    case BI_RLE8:

        switch(pIH->biBitCount)
        {
        case 1:
        case 4:
        case 8:
            Format = D3DFMT_P8;
            break;

        case 16:
            Format = D3DFMT_X1R5G5B5;
            break;

        case 24:
#if 0
            Format = D3DFMT_R8G8B8;
            break;
#endif
        case 32:
            Format = D3DFMT_X8R8G8B8;
            break;

        default:
            return E_FAIL;
        }
        break;

    case BI_BITFIELDS:
        if(pIH->biSize < sizeof(BITMAPV4HEADER))
            return E_FAIL;

        dwB = ((BITMAPV4HEADER *) pIH)->bV4BlueMask;
        dwG = ((BITMAPV4HEADER *) pIH)->bV4GreenMask;
        dwR = ((BITMAPV4HEADER *) pIH)->bV4RedMask;
        dwA = ((BITMAPV4HEADER *) pIH)->bV4AlphaMask;

        switch(pIH->biBitCount)
        {
        case 16:
            if(dwB == 0x00ff && dwG == 0x00ff && dwR == 0x00ff && dwA == 0xff00)
                Format = D3DFMT_A8L8;

            else if(dwB == 0x001f && dwG == 0x07e0 && dwR == 0xf800 && dwA == 0x0000)
                Format = D3DFMT_R5G6B5;

            else if(dwB == 0x001f && dwG == 0x03e0 && dwR == 0x7c00 && dwA == 0x0000)
                Format = D3DFMT_X1R5G5B5;

            else if(dwB == 0x001f && dwG == 0x03e0 && dwR == 0x7c00 && dwA == 0x8000)
                Format = D3DFMT_A1R5G5B5;

            else if(dwB == 0x000f && dwG == 0x00f0 && dwR == 0x0f00 && dwA == 0xf000)
                Format = D3DFMT_A4R4G4B4;
#if 0
            else if(dwB == 0x0003 && dwG == 0x001c && dwR == 0x00e0 && dwA == 0xff00)
                Format = D3DFMT_A8R3G3B2;
#endif
            break;

        case 24:
            if(dwB == 0x0000ff && dwG == 0x00ff00 && dwR == 0xff0000 && dwA == 0x000000)
#if 0                
                Format = D3DFMT_R8G8B8;
#else
                Format = D3DFMT_X8R8G8B8;
#endif
            break;

        case 32:
            if(dwB == 0x000000ff && dwG == 0x0000ff00 && dwR == 0x00ff0000 && dwA == 0x00000000)
                Format = D3DFMT_X8R8G8B8;

            else if(dwB == 0x000000ff && dwG == 0x0000ff00 && dwR == 0x00ff0000 && dwA == 0xff000000)
                Format = D3DFMT_A8R8G8B8;

            break;
        }

        break;

    default:
        DPF(0, "LoadBMP: JPEG compression not supported");
        return E_NOTIMPL;
    }


    if(D3DFMT_UNKNOWN == Format)
    {
        return E_FAIL;
    }


    if(D3DFMT_P8 == Format)
    {
        DWORD dwClrUsed = pIH->biClrUsed;

        if(!dwClrUsed)
            dwClrUsed = 1 << pIH->biBitCount;

        m_bDeletePalette = TRUE;

        if(!(m_pPalette = new PALETTEENTRY[256]))
            return E_OUTOFMEMORY;

        DWORD dw;
        RGBQUAD* prgb = (RGBQUAD*) (((BYTE *) pIH) + pIH->biSize);

        for(dw = 0; dw < dwClrUsed; dw++, prgb++)
        {
            m_pPalette[dw].peRed   = prgb->rgbRed;
            m_pPalette[dw].peGreen = prgb->rgbGreen;
            m_pPalette[dw].peBlue  = prgb->rgbBlue;
            m_pPalette[dw].peFlags = 0xff;
        }

        for(dw = dwClrUsed; dw < 256; dw++)
        {
            m_pPalette[dw].peRed   = 0xff;
            m_pPalette[dw].peGreen = 0xff;
            m_pPalette[dw].peBlue  = 0xff;
            m_pPalette[dw].peFlags = 0xff;
        }
    }



    DWORD_PTR dwWidthBytes;
    DWORD_PTR dwSrcInc, dwDstInc;

    switch(pIH->biBitCount)
    {
    case 1:
        dwWidthBytes = dwWidth;
        dwSrcInc = ((dwWidth >> 3) + 3) & ~3;
        break;

    case 4:
        dwWidthBytes = dwWidth;
        dwSrcInc = ((dwWidth >> 1) + 3) & ~3;
        break;

    default:
        dwWidthBytes = (dwWidth * (pIH->biBitCount >> 3));
        dwSrcInc = (dwWidthBytes + 3) & ~3;
        break;
    }

    m_Format  = Format;
    m_cbPitch = (DWORD)((dwWidthBytes + 3) & ~3);

    m_Rect.left   = 0;
    m_Rect.top    = 0;
    m_Rect.right  = (DWORD)dwWidth;
    m_Rect.bottom = (DWORD)dwHeight;
#if 1
    if (pIH->biBitCount == 24 && Format == D3DFMT_X8R8G8B8)
    {
        UNALIGNED BYTE* pbSrc;
        DWORD*          pdwDst;
        int             nStrideDst;
        UINT            i, j;
        UINT            nAdjustSrc;
        
        nAdjustSrc = dwSrcInc - dwWidthBytes;

        dwWidthBytes = (dwWidth * (32 >> 3));
        m_cbPitch = (DWORD)((dwWidthBytes + 3) & ~3);

        m_bDeleteData = TRUE;

        m_pvData = new BYTE[dwHeight * m_cbPitch];
        if (!m_pvData)
            return E_OUTOFMEMORY;

        pbSrc = ((BYTE*)pvData) + dwOffset;
        if (pIH->biHeight < 0)
        {
            pdwDst = (DWORD*)m_pvData;
            nStrideDst = m_cbPitch >> 2;
        }
        else
        {
            pdwDst = (DWORD*)((BYTE*)m_pvData + m_cbPitch * (dwHeight - 1));
            nStrideDst = -(int)(m_cbPitch >> 2);
        }

        for (i = 0; i < dwHeight; i++)
        {
            for (j = 0; j < dwWidth; j++)
            {
                pdwDst[j] = pbSrc[2] << 16 | pbSrc[1] << 8 | *pbSrc;
                pbSrc += 3;
            }

            pbSrc += nAdjustSrc;
            pdwDst += nStrideDst;
        }

        return S_OK;
    }
#endif
    if(pIH->biHeight < 0 && pIH->biBitCount >= 8)
    {
        // The data is in the correct format already in the file..
        // just return a pointer to the data in the file without
        // copying it.  (This is valid since this object will be
        // destroyed before the API function returns to the user.)

        m_bDeleteData = FALSE;
        m_pvData = ((BYTE *) pvData) + dwOffset;

        return S_OK;
    }


    // Data in file needs to be converted.. so lets allocate the destination
    // buffer which will contain the image..

    m_bDeleteData = TRUE;
    m_pvData  = new BYTE[dwHeight * m_cbPitch];

    if(!m_pvData)
        return E_OUTOFMEMORY;

    UNALIGNED BYTE *pbSrc, *pbDest, *pbDestMin, *pbDestLim, *pbDestLine;

    pbSrc = ((BYTE *) pvData) + dwOffset;

    if(pIH->biHeight < 0)
    {
        dwDstInc = m_cbPitch;
        pbDest = (BYTE *) m_pvData;
    }
    else
    {
        dwDstInc = 0 - (DWORD_PTR)m_cbPitch;
        pbDest = (BYTE *) m_pvData + (dwHeight - 1) * m_cbPitch;
    }

    pbDestLine = pbDest;
    pbDestMin = (BYTE *) m_pvData;
    pbDestLim = (BYTE *) m_pvData + dwHeight * m_cbPitch;



    if(BI_RLE4 == pIH->biCompression)
    {
        // RLE4. Always encoded upsidedown.

        while(pbDest >= pbDestMin)
        {
            if(0 == pbSrc[0])
            {
                switch(pbSrc[1])
                {
                case 0:
                    D3DXASSERT(pbDest == pbDestLine + dwWidth);
                    pbDestLine -= m_cbPitch;
                    pbDest = pbDestLine;
                    break;

                case 1:
                    pbDest = pbDestMin - m_cbPitch;
                    break;

                case 2:
                    pbDest += pbSrc[2] - pbSrc[3] * m_cbPitch;
                    pbSrc += 2;
                    break;

                default:
                    for(int i = 0; i < pbSrc[1]; i++)
                        pbDest[i] = (i & 1) ?  (pbSrc[2 + (i >> 1)] & 0x0f) : (pbSrc[2 + (i >> 1)] >> 4);

                    pbDest += pbSrc[1];
                    pbSrc += ((pbSrc[1] >> 1) + 1) & ~1;
                    break;
                }
            }
            else
            {
                for(int i = 0; i < pbSrc[0]; i++)
                    pbDest[i] = (i & 1) ? (pbSrc[1] & 0x0f) : (pbSrc[1] >> 4);

                pbDest += pbSrc[0];
            }

            pbSrc += 2;
        }

        return S_OK;
    }

    if(pIH->biCompression == BI_RLE8)
    {
        // RLE8. Always encoded upsidedown.

        while(pbDest >= pbDestMin)
        {
            if(0 == pbSrc[0])
            {
                switch(pbSrc[1])
                {
                case 0:
                    D3DXASSERT(pbDest == pbDestLine + dwWidth);
                    pbDestLine -= m_cbPitch;
                    pbDest = pbDestLine;
                    break;

                case 1:
                    pbDest = pbDestMin - m_cbPitch;
                    break;

                case 2:
                    pbDest += pbSrc[2] - pbSrc[3] * m_cbPitch;
                    pbSrc += 2;
                    break;

                default:
                    memcpy(pbDest, pbSrc + 2, pbSrc[1]);
                    pbDest += pbSrc[1];
                    pbSrc += (pbSrc[1] + 1) & ~1;
                    break;
                }
            }
            else
            {
                memset(pbDest, pbSrc[1], pbSrc[0]);
                pbDest += pbSrc[0];
            }

            pbSrc += 2;
        }

        return S_OK;
    }


    if(1 == pIH->biBitCount)
    {
        while(pbDest >= pbDestMin && pbDest < pbDestLim)
        {
            for(UINT i = 0; i < dwWidth; i++)
                pbDest[i] = (pbSrc[i >> 3] >> (7 - (i & 7))) & 1;

            pbDest += dwDstInc;
            pbSrc  += dwSrcInc;
        }

        return S_OK;
    }

    if(4 == pIH->biBitCount)
    {
        while(pbDest >= pbDestMin && pbDest < pbDestLim)
        {
            for(UINT i = 0; i < dwWidth; i++)
                pbDest[i] = (i & 1) ? pbSrc[i >> 1] & 0x0f : (pbSrc[i >> 1] >> 4);

            pbDest += dwDstInc;
            pbSrc  += dwSrcInc;
        }

        return S_OK;
    }


    while(pbDest >= pbDestMin && pbDest < pbDestLim)
    {
        memcpy(pbDest, pbSrc, dwWidthBytes);

        pbDest += dwDstInc;
        pbSrc  += dwSrcInc;
    }

    return S_OK;
}



//////////////////////////////////////////////////////////////////////////////
// LoadJPG ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

struct d3dx_jpeg_error_mgr : public D3DX::jpeg_error_mgr
{
    jmp_buf setjmp_buffer;
};

struct d3dx_jpeg_source_mgr : public D3DX::jpeg_source_mgr
{
    LPCVOID pvData;
    DWORD   cbData;
};



static void
d3dx_jpeg_error_exit (D3DX::j_common_ptr cinfo)
{
    (*cinfo->err->output_message) (cinfo);
    longjmp(((d3dx_jpeg_error_mgr *) cinfo->err)->setjmp_buffer, 1);
}

static void
d3dx_jpeg_ignore_message (D3DX::j_common_ptr cinfo)
{
}

static void
d3dx_jpeg_output_message (D3DX::j_common_ptr cinfo)
{
  char sz[JMSG_LENGTH_MAX];

  (*cinfo->err->format_message) (cinfo, sz);
  DPF(0, "LoadJPG: %s", sz);
}

static void
d3dx_jpeg_init_source(D3DX::j_decompress_ptr cinfo)
{
}

static boolean
d3dx_jpeg_fill_input_buffer(D3DX::j_decompress_ptr cinfo)
{
    d3dx_jpeg_source_mgr *psrc = (d3dx_jpeg_source_mgr *) cinfo->src;

    psrc->next_input_byte = (BYTE *) psrc->pvData;
    psrc->bytes_in_buffer = psrc->cbData;

    return TRUE;
}

static void
d3dx_jpeg_skip_input_data(D3DX::j_decompress_ptr cinfo, long num_bytes)
{
    d3dx_jpeg_source_mgr *psrc = (d3dx_jpeg_source_mgr *) cinfo->src;

    psrc->next_input_byte += (size_t) num_bytes;
    psrc->bytes_in_buffer -= (size_t) num_bytes;
}


static void
d3dx_jpeg_term_source(D3DX::j_decompress_ptr cinfo)
{
}


HRESULT CD3DXImage::LoadJPG(LPCVOID pvData, DWORD cbData)
{
    HRESULT hr;

    D3DX::jpeg_decompress_struct cinfo;
    d3dx_jpeg_source_mgr *psrc;
    d3dx_jpeg_error_mgr err;

    D3DX::JSAMPARRAY pBuffer;

    cinfo.err = D3DX::jpeg_std_error(&err);
    err.error_exit = d3dx_jpeg_error_exit;
    err.output_message = d3dx_jpeg_ignore_message;

    if(setjmp(err.setjmp_buffer))
    {
        hr = E_FAIL;
        goto LDone;
    }

    D3DX::jpeg_CreateDecompress(&cinfo, JPEG_LIB_VERSION, sizeof(cinfo));



    cinfo.src = (struct D3DX::jpeg_source_mgr *)
      (*cinfo.mem->alloc_small) ((D3DX::j_common_ptr) &cinfo, JPOOL_PERMANENT, sizeof(d3dx_jpeg_source_mgr));

    psrc = (d3dx_jpeg_source_mgr *) cinfo.src;
    psrc->init_source       = d3dx_jpeg_init_source;
    psrc->fill_input_buffer = d3dx_jpeg_fill_input_buffer;
    psrc->skip_input_data   = d3dx_jpeg_skip_input_data;
    psrc->resync_to_restart = D3DX::jpeg_resync_to_restart;
    psrc->term_source       = d3dx_jpeg_term_source;
    psrc->bytes_in_buffer   = 0;
    psrc->next_input_byte   = NULL;
    psrc->cbData            = cbData;
    psrc->pvData            = pvData;


    D3DX::jpeg_read_header(&cinfo, TRUE);
    err.output_message = d3dx_jpeg_output_message;
    D3DX::jpeg_start_decompress(&cinfo);


    switch(cinfo.output_components)
    {
    case 1:
        m_Format = D3DFMT_L8;
        m_cbPitch = cinfo.output_width;
        break;

    case 3:
        m_Format = D3DFMT_X8R8G8B8;
        m_cbPitch = cinfo.output_width * sizeof(DWORD);
        break;

    default:
        hr = E_FAIL;
        goto LDone;
    }


    m_bDeleteData = TRUE;

    m_Rect.left = 0;
    m_Rect.top = 0;
    m_Rect.right = cinfo.output_width;
    m_Rect.bottom = cinfo.output_height;

    if(!(m_pvData = new BYTE[cinfo.output_height * m_cbPitch]))
    {
        hr = E_OUTOFMEMORY;
        goto LDone;
    }

    if(!(pBuffer = (*cinfo.mem->alloc_sarray)((D3DX::j_common_ptr) &cinfo, 
        JPOOL_IMAGE, cinfo.output_width * cinfo.output_components, 1)))
    {
        hr = E_OUTOFMEMORY;
        goto LDone;
    }


    if(D3DFMT_L8 == m_Format)
    {
        UINT8 *pubDest;
        pubDest = (UINT8 *) m_pvData;

        while(cinfo.output_scanline < cinfo.output_height)
        {
            D3DX::jpeg_read_scanlines(&cinfo, pBuffer, 1);
            memcpy(pubDest, pBuffer[0], cinfo.output_width);
            pubDest += cinfo.output_width;
        }
    }
    else if(D3DFMT_X8R8G8B8 == m_Format)
    {
        UINT8 *pubSrc;
        UINT32 *pulDest, *pulDestLine;
        pulDest = (UINT32 *) m_pvData;

        while(cinfo.output_scanline < cinfo.output_height)
        {
            D3DX::jpeg_read_scanlines(&cinfo, pBuffer, 1);

            pubSrc = (UINT8 *) pBuffer[0];
            pulDestLine = pulDest + cinfo.output_width;

            while(pulDest < pulDestLine)
            {
                *pulDest = ((UINT32) pubSrc[0] << 16) |
                           ((UINT32) pubSrc[1] << 8) |
                           ((UINT32) pubSrc[2]);

                pulDest++;
                pubSrc += 3;
            }
        }
    }


    D3DX::jpeg_finish_decompress(&cinfo);

    hr = S_OK;
    goto LDone;


LDone:
    D3DX::jpeg_destroy_decompress(&cinfo);
    return hr;
}


//////////////////////////////////////////////////////////////////////////////
// LoadTGA ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#pragma pack(1)
struct d3dx_tga_header
{
    BYTE cbId;
    BYTE bColorMap;
    BYTE bImageType;

    WORD wColorMapIndex;
    WORD wColorMapLength;
    BYTE bColorMapBits;

    WORD wXOrigin;
    WORD wYOrigin;
    WORD wWidth;
    WORD wHeight;
    BYTE bBits;
    BYTE bFlags;
};
#pragma pack()


HRESULT CD3DXImage::LoadTGA(LPCVOID pvData, DWORD cbData)
{
    // Validate header.  TGA files don't seem to have any sort of magic number
    // to identify them.  Therefore, we will proceed as if this is a real TGA
    // file, until we see something we don't understand.

    BYTE *pbData = (BYTE *) pvData;
    d3dx_tga_header *pFH = (d3dx_tga_header *) pbData;

    if(cbData < sizeof(d3dx_tga_header))
        return E_FAIL;

    if(pFH->bColorMap & ~0x01)
        return E_FAIL;

    if(pFH->bImageType & ~0x0B)
        return E_FAIL;

    if(!pFH->wWidth || !pFH->wHeight)
        return E_FAIL;



    // Colormap size and format
    UINT uColorMapBytes = ((UINT) pFH->bColorMapBits + 7) >> 3;
    D3DFORMAT ColorMapFormat = D3DFMT_UNKNOWN;

    if(pFH->bColorMap)
    {
        switch(pFH->bColorMapBits)
        {
        case 15: ColorMapFormat = D3DFMT_X1R5G5B5; break;
        case 16: ColorMapFormat = D3DFMT_A1R5G5B5; break;
        case 24: ColorMapFormat = D3DFMT_R8G8B8;   break;
        case 32: ColorMapFormat = D3DFMT_A8R8G8B8; break;
        default: return E_FAIL;
        }
    }


    // Image size and format
    UINT uBytes = ((UINT) pFH->bBits + 7) >> 3;
    D3DFORMAT Format = D3DFMT_UNKNOWN;

    switch(pFH->bImageType & 0x03)
    {
    case 1:
        if(!pFH->bColorMap)
            return E_FAIL;

        switch(pFH->bBits)
        {
        case 8: Format = D3DFMT_P8; break;
        default: return E_FAIL;
        }
        break;

    case 2:
        switch(pFH->bBits)
        {
        case 15: Format = D3DFMT_X1R5G5B5; break;
        case 16: Format = D3DFMT_A1R5G5B5; break;
        case 24: Format = D3DFMT_R8G8B8;   break;
        case 32: Format = D3DFMT_A8R8G8B8; break;
        default: return E_FAIL;
        }
        break;

    case 3:
        switch(pFH->bBits)
        {
        case 8: Format = D3DFMT_L8; break;
        default: return E_FAIL;
        }
        break;

    default:
        return E_FAIL;
    }

    BOOL bRLE         = pFH->bImageType & 0x08;
    BOOL bTopToBottom = 0x20 == (pFH->bFlags & 0x20);
    BOOL bLeftToRight = 0x10 != (pFH->bFlags & 0x10);

    pbData += sizeof(d3dx_tga_header);
    cbData -= sizeof(d3dx_tga_header);


    // Skip ID
    if(cbData < pFH->cbId)
        return E_FAIL;

    pbData += pFH->cbId;
    cbData -= pFH->cbId;


    // Color map
    UINT cbColorMap = (UINT) pFH->wColorMapLength * uColorMapBytes;

    if(cbData < cbColorMap)
        return E_FAIL;

    if(D3DFMT_P8 == Format)
    {
        if(pFH->wColorMapIndex + pFH->wColorMapLength > 256)
            return E_FAIL;

        if(!(m_pPalette = new PALETTEENTRY[256]))
            return E_OUTOFMEMORY;

        m_bDeletePalette = TRUE;
        memset(m_pPalette, 0xff, 256 * sizeof(PALETTEENTRY));

        BYTE *pb = pbData;
        PALETTEENTRY *pColor = m_pPalette + pFH->wColorMapIndex;
        PALETTEENTRY *pColorLim = pColor + pFH->wColorMapLength;

        while(pColor < pColorLim)
        {
            UINT u, uA, uR, uG, uB;

            switch(ColorMapFormat)
            {
            case D3DFMT_X1R5G5B5:
                u = *((WORD *) pb);

                uA = 0xff;
                uR = (u >> 10) & 0x1f;
                uG = (u >>  5) & 0x1f;
                uB = (u >>  0) & 0x1f;

                uR = (uR << 3) | (uR >> 2);
                uG = (uG << 3) | (uG >> 2);
                uB = (uB << 3) | (uB >> 2);

                pb += 2;
                break;

            case D3DFMT_A1R5G5B5:
                u = *((WORD *) pb);

                uA = (u >> 15) * 0xff;
                uR = (u >> 10) & 0x1f;
                uG = (u >>  5) & 0x1f;
                uB = (u >>  0) & 0x1f;

                uR = (uR << 3) | (uR >> 2);
                uG = (uG << 3) | (uG >> 2);
                uB = (uB << 3) | (uB >> 2);

                pb += 2;
                break;

            case D3DFMT_R8G8B8:
                uA = 0xff;
                uR = pb[2];
                uG = pb[1];
                uB = pb[0];

                pb += 3;
                break;

            case D3DFMT_A8R8G8B8:
                u = *((DWORD *) pb);

                uA = (u >> 24) & 0xff;
                uR = (u >> 16) & 0xff;
                uG = (u >>  8) & 0xff;
                uB = (u >>  0) & 0xff;

                pb += 4;
                break;
            }
            
            pColor->peRed   = (BYTE) uR;
            pColor->peGreen = (BYTE) uG;
            pColor->peBlue  = (BYTE) uB;
            pColor->peFlags = (BYTE) uA;

            pColor++;
        }
    }

    pbData += cbColorMap;
    cbData -= cbColorMap;


    // Image data
    UINT cbImage = (UINT) pFH->wWidth * (UINT) pFH->wHeight * uBytes;

    m_Format  = Format;
    m_pvData  = pbData;
    m_cbPitch = (UINT) pFH->wWidth * uBytes;

    m_Rect.left   = 0;
    m_Rect.top    = 0;
    m_Rect.right  = pFH->wWidth;
    m_Rect.bottom = pFH->wHeight;


    if(!bRLE && bTopToBottom && bLeftToRight)
    {
        // Data is already in a format usable to D3D.. no conversion is necessary
        if(cbData < cbImage)
            return E_FAIL;

        m_pvData = pbData;
        m_bDeleteData = FALSE;

        pbData += cbImage;
        cbData -= cbImage;
    }
    else
    {
        // Image data is compressed, or does not have origin at top-left
        if(!(m_pvData = new BYTE[cbImage]))
            return E_OUTOFMEMORY;

        m_bDeleteData = TRUE;


        BYTE *pbDestY = bTopToBottom ? (BYTE *) m_pvData : ((BYTE *) m_pvData + (pFH->wHeight - 1) * m_cbPitch);

        for(UINT uY = 0; uY < pFH->wHeight; uY++)
        {
            BYTE *pbDestX = bLeftToRight ? pbDestY : (pbDestY + m_cbPitch - uBytes);

            for(UINT uX = 0; uX < pFH->wWidth; )
            {
                BOOL bRunLength;
                UINT uCount;

                if(bRLE)
                {
                    if(cbData < 1)
                        return E_FAIL;

                    bRunLength = *pbData & 0x80;
                    uCount = (*pbData & 0x7f) + 1;

                    pbData++;
                    cbData--;
                }
                else
                {
                    bRunLength = FALSE;
                    uCount = pFH->wWidth;
                }

                uX += uCount;

                while(uCount--)
                {
                    if(cbData < uBytes)
                        return E_FAIL;

                    memcpy(pbDestX, pbData, uBytes);

                    if(!bRunLength)
                    {
                        pbData += uBytes;
                        cbData -= uBytes;
                    }

                    pbDestX = bLeftToRight ? (pbDestX + uBytes) : (pbDestX - uBytes);
                }

                if(bRunLength)
                {
                    pbData += uBytes;
                    cbData -= uBytes;
                }
            }

            pbDestY = bTopToBottom ? (pbDestY + m_cbPitch) : (pbDestY - m_cbPitch);
        }
    }

    return S_OK;
}





//////////////////////////////////////////////////////////////////////////////
// LoadPPM ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

enum
{
    PPM_WIDTH, PPM_HEIGHT, PPM_MAX, PPM_DATA_R, PPM_DATA_G, PPM_DATA_B
};

HRESULT CD3DXImage::LoadPPM(LPCVOID pvData, DWORD cbData)
{

    BYTE *pbData = (BYTE *) pvData;

    // Check header
    BOOL bAscii;

    if(cbData < 2)
        return E_FAIL;

    if('P' == pbData[0] && '3' == pbData[1])
        bAscii = TRUE;
    else if('P' == pbData[0] && '6' == pbData[1])
        bAscii = FALSE;
    else
        return E_FAIL;

    pbData += 2;
    cbData -= 2;


    // Image data
    UINT uMode   = PPM_WIDTH;
    UINT uWidth  = 0;
    UINT uHeight = 0;
    UINT uMax    = 255;

    DWORD *pdw, *pdwLim;


    while(cbData)
    {
        if(!bAscii && PPM_DATA_R == uMode)
        {
            // Binary image data
            if(uMax > 255)
                return E_FAIL;

            if(cbData > 1 && '\r' == *pbData)
            {
                pbData++;
                cbData--;
            }

            pbData++;
            cbData--;

            while(cbData && pdw < pdwLim)
            {
                *pdw++ = ((255 * pbData[0] / uMax) << 16) |
                         ((255 * pbData[1] / uMax) <<  8) |
                         ((255 * pbData[2] / uMax) <<  0) | 0xff000000;

                pbData += 3;
                cbData -= 3;
            }

            if(pdw != pdwLim)
                return E_FAIL;

            return S_OK;
        }
        if(isspace(*pbData))
        {
            // Whitespace
            pbData++;
            cbData--;
        }
        else if('#' == *pbData)
        {
            // Comment
            while(cbData && '\n' != *pbData)
            {
                pbData++;
                cbData--;
            }

            pbData++;
            cbData--;
        }
        else
        {
            // Number
            UINT u = 0;

            while(cbData && !isspace(*pbData))
            {
                if(!isdigit(*pbData))
                    return E_FAIL;

                u = u * 10 + (*pbData - '0');

                pbData++;
                cbData--;
            }

            switch(uMode)
            {
            case PPM_WIDTH:
                uWidth = u;

                if(0 == uWidth)
                    return E_FAIL;

                break;

            case PPM_HEIGHT:
                uHeight = u;

                if(0 == uHeight)
                    return E_FAIL;

                if(!(m_pvData = new BYTE[uWidth * uHeight * sizeof(DWORD)]))
                    return E_OUTOFMEMORY;

                m_bDeleteData = TRUE;

                pdw = (DWORD *) m_pvData;
                pdwLim = pdw + uWidth * uHeight;

                m_Format  = D3DFMT_X8R8G8B8;
                m_cbPitch = uWidth * sizeof(DWORD);

                m_Rect.left   = 0;
                m_Rect.top    = 0;
                m_Rect.right  = uWidth;
                m_Rect.bottom = uHeight;
                break;

            case PPM_MAX:
                uMax = u;

                if(0 == uMax)
                    return E_FAIL;

                break;

            case PPM_DATA_R:
                if(pdw >= pdwLim)
                    return E_FAIL;

                *pdw  = ((u * 255 / uMax) << 16) | 0xff000000;
                break;

            case PPM_DATA_G:
                *pdw |= ((u * 255 / uMax) <<  8);
                break;

            case PPM_DATA_B:
                *pdw |= ((u * 255 / uMax) <<  0);

                if(++pdw == pdwLim)
                    return S_OK;

                uMode = PPM_DATA_R - 1;
                break;
            }

            uMode++;
        }
    }

    return E_FAIL;
}




//////////////////////////////////////////////////////////////////////////////
// LoadPNG ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

struct d3dx_png_io
{
    LPCVOID pv;
    SIZE_T  cb;
};

static void __cdecl
d3dx_png_read_fn(D3DX::png_structp png_ptr, D3DX::png_bytep data, png_size_t length)
{
    d3dx_png_io *pio = (d3dx_png_io *) png_ptr->io_ptr;

    if(length > pio->cb)
        D3DX::png_error(png_ptr, "read error");

    memcpy(data, pio->pv, length);

    pio->pv = (BYTE *) pio->pv + length;
    pio->cb -= length;
}

static void __cdecl
d3dx_png_error_fn(D3DX::png_structp png_ptr, D3DX::png_const_charp error_msg)
{
    DPF(0, "LoadPNG: %s", error_msg);
    longjmp(png_ptr->jmpbuf, TRUE);
}

static void __cdecl
d3dx_png_warning_fn(D3DX::png_structp png_ptr, D3DX::png_const_charp warning_msg)
{
    DPF(0, "LoadPNG: %s", warning_msg);
}


HRESULT CD3DXImage::LoadPNG(LPCVOID pvData, DWORD cbData)
{
    HRESULT hr;

    unsigned long width, height, row;
    int bit_depth, color_type, intent;
    double image_gamma;
    D3DX::png_color_16p image_background;
    d3dx_png_io io;

    D3DX::png_structp png_ptr = NULL;
    D3DX::png_infop info_ptr  = NULL;
    BYTE** row_pointers = NULL;


    // Check header
    if(D3DX::png_sig_cmp((BYTE *) pvData, 0, cbData))
        return E_FAIL;

    // Create read and info structs
    if(!(png_ptr = D3DX::png_create_read_struct(PNG_LIBPNG_VER_STRING,
      NULL, d3dx_png_error_fn, d3dx_png_warning_fn)))
    {
        goto LOutOfMemory;
    }

    if(!(info_ptr = D3DX::png_create_info_struct(png_ptr)))
        goto LOutOfMemory;

    if(setjmp(png_ptr->jmpbuf))
        goto LFail;


    // Setup read func
    io.pv = pvData;
    io.cb = cbData;

    D3DX::png_set_read_fn(png_ptr, (void *) &io, d3dx_png_read_fn);


    // Read header
    D3DX::png_read_info(png_ptr, info_ptr);
    D3DX::png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

    switch(color_type)
    {
    case PNG_COLOR_TYPE_GRAY:       m_Format = D3DFMT_L8;       break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: m_Format = D3DFMT_A8L8;     break;
    case PNG_COLOR_TYPE_PALETTE:    m_Format = D3DFMT_P8;       break;
    case PNG_COLOR_TYPE_RGB:        m_Format = D3DFMT_R8G8B8;   break;
    case PNG_COLOR_TYPE_RGB_ALPHA:  m_Format = D3DFMT_A8R8G8B8; break;
    }

    // Force byte-sized components
    if (bit_depth == 16)
        D3DX::png_set_strip_16(png_ptr);

    if (bit_depth < 8)
        D3DX::png_set_packing(png_ptr);

    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        D3DX::png_set_gray_1_2_4_to_8(png_ptr);


    // Setup gamma
    if(D3DX::png_get_sRGB(png_ptr, info_ptr, &intent))
        D3DX::png_set_sRGB(png_ptr, info_ptr, intent);
    else if(D3DX::png_get_gAMA(png_ptr, info_ptr, &image_gamma))
        D3DX::png_set_gamma(png_ptr, 2.2, image_gamma);


    // Convert colorkey to alpha
    if(D3DFMT_P8 != m_Format &&
       D3DX::png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    {
        D3DX::png_set_tRNS_to_alpha(png_ptr);

        switch(m_Format)
        {
        case D3DFMT_L8:     m_Format = D3DFMT_A8L8;     break;
        case D3DFMT_R8G8B8: m_Format = D3DFMT_A8R8G8B8; break;
        }
    }

    // Use BGR or BGRA
    if(
       D3DFMT_R8G8B8 == m_Format || 
       D3DFMT_A8R8G8B8 == m_Format)
        D3DX::png_set_bgr(png_ptr);

    // Expand RGB to XRGB
    if(D3DFMT_R8G8B8 == m_Format)
    {
        D3DX::png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
        m_Format = D3DFMT_X8R8G8B8;
    }


    // Update info
    D3DX::png_read_update_info(png_ptr, info_ptr);

    int nComp;
    switch(m_Format)
    {
    case D3DFMT_L8:       nComp = 1; break;
    case D3DFMT_A8L8:     nComp = 2; break;
    case D3DFMT_P8:       nComp = 1; break;
    case D3DFMT_R8G8B8:   nComp = 3; break;
    case D3DFMT_X8R8G8B8: nComp = 4; break;
    case D3DFMT_A8R8G8B8: nComp = 4; break;
    }

    if(nComp != D3DX::png_get_channels(png_ptr, info_ptr))
        goto LFail;

    m_Rect.left   = 0;
    m_Rect.top    = 0;
    m_Rect.right  = width;
    m_Rect.bottom = height;

    m_cbPitch = D3DX::png_get_rowbytes(png_ptr, info_ptr);


    // Read image
    if(!(m_pvData = new BYTE[height * m_cbPitch]))
        goto LOutOfMemory;

    m_bDeleteData = TRUE;

    if(!(row_pointers = new BYTE*[height]))
        goto LOutOfMemory;

    for(row = 0; row < height; row++)
        row_pointers[row] = (BYTE *) m_pvData + row * m_cbPitch;

    D3DX::png_read_image(png_ptr, row_pointers);



    // Read palette (palette should have been gamma-corrected during read)
    if(D3DFMT_P8 == m_Format)
    {
        D3DX::png_bytep trans;
        D3DX::png_colorp palette;
        int num_trans   = 0;
        int num_palette = 0;

        if(D3DX::png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE))
            D3DX::png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

        if(D3DX::png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
            D3DX::png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, NULL);


        // build gamma table
        BYTE gamma[256];

        if((png_ptr->gamma > .000001) && (png_ptr->screen_gamma > .000001))
        {
            double g = 1.0 / (png_ptr->gamma * png_ptr->screen_gamma);

            for(int i = 0; i < 256; i++)
                gamma[i] = (BYTE)(pow((double)i / 255.0, g) * 255.0 + .5);
        }
        else
        {
            for(int i = 0; i < 256; i++)
                gamma[i] = static_cast<BYTE>(i);
        }



        // Allocate palettte
        if(!(m_pPalette = new PALETTEENTRY[256]))
            goto LOutOfMemory;

        m_bDeletePalette = TRUE;


        // Fill palette
        int i = 0;

        for(; i < num_trans; i++)
        {
            m_pPalette[i].peRed   = gamma[palette[i].red];
            m_pPalette[i].peGreen = gamma[palette[i].green];
            m_pPalette[i].peBlue  = gamma[palette[i].blue];
            m_pPalette[i].peFlags = trans[i];
        }

        for(; i < num_palette; i++)
        {
            m_pPalette[i].peRed   = gamma[palette[i].red];
            m_pPalette[i].peGreen = gamma[palette[i].green];
            m_pPalette[i].peBlue  = gamma[palette[i].blue];
            m_pPalette[i].peFlags = 0xff;
        }

        for(; i < 256; i++)
        {
            m_pPalette[i].peRed   = 0xff;
            m_pPalette[i].peGreen = 0xff;
            m_pPalette[i].peBlue  = 0xff;
            m_pPalette[i].peFlags = 0xff;
        }
    }


    hr = S_OK;
    goto LDone;

LFail:
    hr = E_FAIL;
    goto LDone;

LOutOfMemory:
    hr = E_OUTOFMEMORY;
    goto LDone;

LDone:
    if (png_ptr != NULL)
        D3DX::png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    if(row_pointers)
        delete [] row_pointers;

    return hr;
}





//////////////////////////////////////////////////////////////////////////////
// LoadDDS ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define DDS_RGB              0x00000040
#define DDS_RGBA             0x00000041
#define DDS_CUBEMAP_ALLFACES 0x0000fc00


struct DDS_PIXELFORMAT
{
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	DWORD dwRGBBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwABitMask;
};


struct DDS_HEADER
{
	DWORD dwSize;
	DWORD dwReserved1;
	DWORD dwHeight;
	DWORD dwWidth;
	DWORD dwLinearSize;
	DWORD dwReserved2;
	DWORD dwMipMapCount;
	DWORD dwReserved3[11];
	DDS_PIXELFORMAT ddspf;
	DWORD dwFlags;
	DWORD dwCubemapFlags;
	DWORD dwReserved4[3];
};


HRESULT CD3DXImage::LoadDDS(LPCVOID pvData, DWORD cbData)
{
    BYTE *pbData = (BYTE *) pvData;


    // Check magic number
    if(cbData < sizeof(DWORD))
        return E_FAIL;

    if(*((DWORD *) pbData) != MAKEFOURCC('D', 'D', 'S', ' '))
        return E_FAIL;

    pbData += sizeof(DWORD);
    cbData -= sizeof(DWORD);


    // Read header
    DDS_HEADER *pHeader;

    if(cbData < sizeof(DDS_HEADER))
        return E_FAIL;

    pHeader = (DDS_HEADER *) pbData;

    pbData += sizeof(DDS_HEADER);
    cbData -= sizeof(DDS_HEADER);


    // Grok dimensions
    m_Rect.left   = 0;
    m_Rect.top    = 0;
    m_Rect.right  = (LONG) pHeader->dwWidth;
    m_Rect.bottom = (LONG) pHeader->dwHeight;


    // Grok faces
    DWORD dwCubemapFlags;
    dwCubemapFlags = (pHeader->dwCubemapFlags & DDS_CUBEMAP_ALLFACES);

    UINT cSides;
    if(DDS_CUBEMAP_ALLFACES == dwCubemapFlags)
        cSides = 6;
    else if(0 == dwCubemapFlags)
        cSides = 1;
    else
    {
        DPF(0, "LoadDDS: Partial cubemaps not supported.");
        return E_FAIL;
    }


    // Grok levels
    UINT cMipLevels;
    cMipLevels = (UINT) pHeader->dwMipMapCount;

    if(0 == cMipLevels)
        cMipLevels = 1;


    // Grok pixel format
    D3DFORMAT Format;

  	if(pHeader->ddspf.dwFourCC == MAKEFOURCC('D', 'X', 'T', '1') ||
            pHeader->ddspf.dwFourCC == D3DFMT_DXT1) {
		Format = D3DFMT_DXT1;
	} else if(pHeader->ddspf.dwFourCC == MAKEFOURCC('D', 'X', 'T', '2') ||
            pHeader->ddspf.dwFourCC == D3DFMT_DXT2) {
		Format = D3DFMT_DXT2;
	} else if(pHeader->ddspf.dwFourCC == MAKEFOURCC('D', 'X', 'T', '3') ||
            pHeader->ddspf.dwFourCC == D3DFMT_DXT3) {
		Format = D3DFMT_DXT3;
	} else if(pHeader->ddspf.dwFourCC == MAKEFOURCC('D', 'X', 'T', '4') ||
            pHeader->ddspf.dwFourCC == D3DFMT_DXT4) {
		Format = D3DFMT_DXT4;
	} else if(pHeader->ddspf.dwFourCC == MAKEFOURCC('D', 'X', 'T', '5') ||
            pHeader->ddspf.dwFourCC == D3DFMT_DXT5) {
		Format = D3DFMT_DXT5;
	} else if(pHeader->ddspf.dwFlags == DDS_RGBA && pHeader->ddspf.dwRGBBitCount == 32 && pHeader->ddspf.dwABitMask == 0xff000000) {
		Format = D3DFMT_A8R8G8B8;
	} else if(pHeader->ddspf.dwFlags == DDS_RGB  && pHeader->ddspf.dwRGBBitCount == 32) {
		Format = D3DFMT_X8R8G8B8;
	} else if(pHeader->ddspf.dwFlags == DDS_RGB  && pHeader->ddspf.dwRGBBitCount == 16 && pHeader->ddspf.dwGBitMask == 0x000007e0) {
		Format = D3DFMT_R5G6B5;
	} else if(pHeader->ddspf.dwFlags == DDS_RGB  && pHeader->ddspf.dwRGBBitCount == 16 && pHeader->ddspf.dwGBitMask == 0x000003e0) {
		Format = D3DFMT_X1R5G5B5;
	} else if(pHeader->ddspf.dwFlags == DDS_RGBA && pHeader->ddspf.dwRGBBitCount == 16 && pHeader->ddspf.dwABitMask == 0x00008000) {
		Format = D3DFMT_A1R5G5B5;
	} else if(pHeader->ddspf.dwFlags == DDS_RGBA && pHeader->ddspf.dwRGBBitCount == 16 && pHeader->ddspf.dwABitMask == 0x0000f000) {
		Format = D3DFMT_A4R4G4B4;
	} else
    {
        DPF(0, "LoadDDS: Unsupported pixel format");
		return E_FAIL;
    }


    // Load images
    for(UINT uSide = 0; uSide < cSides; uSide++)
    {
        RECT Rect;
        Rect = m_Rect;


        // Allocate face
        CD3DXImage *pImage;
        CD3DXImage *pImageSide;

        if(0 == uSide)
        {
            pImageSide = this;
        }
        else
        {
            if(!(pImage = new CD3DXImage))
                return E_OUTOFMEMORY;

            pImageSide->m_pFace = pImage;
            pImageSide = pImage;
        }


        for(UINT uMipLevel = 0; uMipLevel < cMipLevels; uMipLevel++)
        {
            // Allocate mip level
            CD3DXImage *pImageMip;

            if(0 == uMipLevel)
            {
                pImageMip = pImageSide;
            }
            else
            {
                if(!(pImage = new CD3DXImage))
                    return E_OUTOFMEMORY;

                pImageMip->m_pMip = pImage;
                pImageMip = pImage;
            }

            // Compute pitch and area
            UINT cbPitch;
            UINT cbArea;

            switch(Format)
            {
		    case D3DFMT_DXT1:
                cbPitch = ((Rect.right + 3) >> 2) * 8;
                cbArea = cbPitch * ((Rect.bottom + 3) >> 2);
                break;

		    case D3DFMT_DXT2:
		    case D3DFMT_DXT4:
#if 0
		    case D3DFMT_DXT3:
		    case D3DFMT_DXT5:
#endif
                cbPitch = ((Rect.right + 3) >> 2) * 16;
                cbArea = cbPitch * ((Rect.bottom + 3) >> 2);
                break;

		    case D3DFMT_A8R8G8B8:
		    case D3DFMT_X8R8G8B8:
                cbPitch = Rect.right * sizeof(DWORD);
                cbArea = cbPitch * Rect.bottom;
                break;

		    case D3DFMT_R5G6B5:
		    case D3DFMT_X1R5G5B5:
		    case D3DFMT_A1R5G5B5:
		    case D3DFMT_A4R4G4B4:
                cbPitch = Rect.right * sizeof(WORD);
                cbArea = cbPitch * Rect.bottom;
                break;
            }


            // Set data
            pImageMip->m_Format         = Format;
            pImageMip->m_pvData         = pbData;
            pImageMip->m_cbPitch        = cbPitch;
            pImageMip->m_Rect           = Rect;
            pImageMip->m_pPalette       = NULL;
            pImageMip->m_bDeleteData    = FALSE;
            pImageMip->m_bDeletePalette = FALSE;

            if(cbData < cbArea)
                return E_FAIL;

            pbData += cbArea;
            cbData -= cbArea;

            Rect.right   = (Rect.right  == 1) ? 1 : (Rect.right  >> 1);
            Rect.bottom  = (Rect.bottom == 1) ? 1 : (Rect.bottom >> 1);
        }
    }

    return S_OK;
}

