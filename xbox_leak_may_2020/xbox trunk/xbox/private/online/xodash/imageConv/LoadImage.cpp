//-----------------------------------------------------------------------------
// File: LoadImage.cpp
//
// Desc: Loads image files.
//
// Hist: 04.13.01 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <windows.h>
#include <d3d8.h>
#include "LoadImage.h"




//-----------------------------------------------------------------------------
// Name: CImage()
// Desc: Initializes object
//-----------------------------------------------------------------------------
CImage::CImage()
{
    m_Format         = D3DFMT_UNKNOWN;
    m_pData          = NULL;
    m_pPalette       = NULL;
    m_bDeleteData    = FALSE;
    m_bDeletePalette = FALSE;
}




//-----------------------------------------------------------------------------
// Name: ~CImage()
// Desc: Frees resources held by the object
//-----------------------------------------------------------------------------
CImage::~CImage()
{
    if( m_pData && m_bDeleteData )
        delete[] m_pData;

    if( m_pPalette && m_bDeletePalette )
        delete[] m_pPalette;
}




//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Attempts to load the given data as an image
//-----------------------------------------------------------------------------
HRESULT CImage::Load( const VOID* pData, DWORD cbData )
{
    HRESULT hr;

    // Try all known image loading subroutines
    for( int i = 0; i < 4; i++ )
    {
        switch(i)
        {
            case 0: hr = LoadBMP( pData, cbData ); break;
            case 1: hr = LoadPPM( pData, cbData ); break;
            case 2: hr = LoadTGA( pData, cbData ); break;
            case 3: hr = LoadDIB( pData, cbData ); break;
        }

        if( SUCCEEDED(hr) )
            break;

        if( m_pData && m_bDeleteData )
            delete[] m_pData;

        if( m_pPalette && m_bDeletePalette )
            delete[] m_pPalette;

        m_pData          = NULL;
        m_pPalette       = NULL;
        m_bDeleteData    = FALSE;
        m_bDeletePalette = FALSE;
    }

    if( 4 == i )
    {
        DPF(0, "Unsupported file format");
        return D3DXERR_INVALIDDATA;
    }

    m_Rect.left   = 0;
    m_Rect.top    = 0;
    m_Rect.right  = m_Width;
    m_Rect.bottom = m_Height;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Depalettize()
// Desc: Depalettize palettized images
//-----------------------------------------------------------------------------
HRESULT CImage::Depalettize()
{
    if( NULL == m_pPalette )
        return S_OK;

    BYTE*  pSrcData = (BYTE*)m_pData;
    DWORD* pDstData = new DWORD[m_Width*m_Height];

    BYTE*  pOldData = (BYTE*)pSrcData;
    BYTE*  pNewData = (BYTE*)pDstData;

    // Loop through all texels and get 32-bit color from the 8-bit palette index
    for( DWORD y=0; y<m_Height; y++ )
    {
        for( DWORD x=0; x<m_Width; x++ )
        {
            BYTE  index = *pSrcData++;

            DWORD red   = m_pPalette[index].peRed;
            DWORD green = m_pPalette[index].peGreen;
            DWORD blue  = m_pPalette[index].peBlue;
            DWORD alpha = m_pPalette[index].peFlags;

            *pDstData++ = (alpha<<24) | (red<<16) | (green<<8) | (blue<<0);
        }
    }

    // Delete the old palette
    if( m_bDeletePalette )
        delete[] m_pPalette;
    m_pPalette       = NULL;
    m_bDeletePalette = FALSE;

    // Delete the old data, and assign the new data
    if( m_bDeleteData )
        delete[] m_pData;
    m_pData          = pNewData;
    m_bDeleteData    = TRUE;

    // The format is now A8R8G8B8
    m_Format = D3DFMT_A8R8G8B8;
    m_Pitch  = m_Width * sizeof(DWORD);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Reads the data from the file and tries to load it as an image
//-----------------------------------------------------------------------------
HRESULT CImage::Load( CHAR* strFilename )
{
    HANDLE hFile = CreateFileA( strFilename, GENERIC_READ, FILE_SHARE_READ, NULL, 
                                OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
    if( INVALID_HANDLE_VALUE == hFile )
        return E_FAIL;

    // Allocate memory
    DWORD dwFileSize = GetFileSize( hFile, NULL );
    VOID* pFileData  = malloc( dwFileSize );

    // Read it the file
    DWORD dwRead;
    ReadFile( hFile, (VOID*)pFileData, dwFileSize, &dwRead, NULL );

    HRESULT hr = Load( pFileData, dwFileSize );

    // Free stuff
    CloseHandle( hFile );
    free( pFileData );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: LoadBMP()
// Desc: Attempts to load the given data as a BMP
//-----------------------------------------------------------------------------
HRESULT CImage::LoadBMP( const VOID* pvData, DWORD cbData )
{
    // Examine header
    if(cbData < sizeof(BITMAPFILEHEADER))
        return E_FAIL;

    BITMAPFILEHEADER *pFH = (BITMAPFILEHEADER *) pvData;

    if(pFH->bfType != (('B') | ('M' << 8)) || pFH->bfSize > cbData)
        return E_FAIL;

    return LoadDIB((BYTE *) pvData + sizeof(BITMAPFILEHEADER), cbData - sizeof(BITMAPFILEHEADER));
}




//-----------------------------------------------------------------------------
// Name: LoadDIB()
// Desc: Attempts to load the given data as a DIB
//-----------------------------------------------------------------------------
HRESULT CImage::LoadDIB( const VOID* pvData, DWORD cbData )
{
    UNALIGNED BITMAPINFOHEADER *pIH;
    DWORD     dwWidth, dwHeight, dwOffset, dwClrUsed;

    if(cbData < sizeof(BITMAPINFOHEADER))
        return E_FAIL;

    pIH = (BITMAPINFOHEADER *) pvData;

    if(pIH->biSize < sizeof(BITMAPINFOHEADER))
        return E_FAIL;

    dwWidth   = (DWORD) (pIH->biWidth);
    dwHeight  = (DWORD) (pIH->biHeight > 0 ? pIH->biHeight : -pIH->biHeight);
    dwClrUsed = (DWORD) (pIH->biClrUsed);

    if((pIH->biBitCount <= 8) && (0 == dwClrUsed))
        dwClrUsed = (DWORD) (1 << pIH->biBitCount);

    dwOffset  = (DWORD) pIH->biSize + dwClrUsed * sizeof(DWORD);

    if(dwOffset > (DWORD) cbData)
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

    DWORD dwWidthBytes;
    DWORD dwSrcInc, dwDstInc;

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
    m_Pitch   = (DWORD)((dwWidthBytes + 3) & ~3);
    m_Width   = (DWORD)dwWidth;
    m_Height  = (DWORD)dwHeight;
#if 1
    if (pIH->biBitCount == 24 && Format == D3DFMT_X8R8G8B8)
    {
        UNALIGNED BYTE* pbSrc;
        DWORD*          pdwDst;
        int             nStrideDst;
        UINT            i, j;

        dwWidthBytes = (dwWidth * (32 >> 3));
        m_Pitch      = (DWORD)((dwWidthBytes + 3) & ~3);

        m_bDeleteData = TRUE;

        m_pData = new BYTE[dwHeight * m_Pitch];
        if (!m_pData)
            return E_OUTOFMEMORY;

        pbSrc = ((BYTE*)pvData) + dwOffset;
        if (pIH->biHeight < 0)
        {
            pdwDst = (DWORD*)m_pData;
            nStrideDst = m_Pitch >> 2;
        }
        else
        {
            pdwDst = (DWORD*)((BYTE*)m_pData + m_Pitch * (dwHeight - 1));
            nStrideDst = -(int)(m_Pitch >> 2);
        }

        for (i = 0; i < dwHeight; i++)
        {
            for (j = 0; j < dwWidth; j++)
            {
                pdwDst[j] = pbSrc[2] << 16 | pbSrc[1] << 8 | *pbSrc;
                pbSrc += 3;
            }

            pdwDst += nStrideDst;
        }

        return S_OK;
    }
#endif
    if(pIH->biHeight < 0 && pIH->biBitCount >= 8)
    {
        // The data is in the correct format already in the file.
        m_pData  = new BYTE[dwHeight * m_Pitch];
        memcpy( m_pData, ((BYTE *)pvData) + dwOffset, dwHeight * m_Pitch );
        m_bDeleteData = TRUE;

        return S_OK;
    }

    // Data in file needs to be converted.. so lets allocate the destination
    // buffer which will contain the image..

    m_bDeleteData = TRUE;
    m_pData  = new BYTE[dwHeight * m_Pitch];

    if(!m_pData)
        return E_OUTOFMEMORY;

    UNALIGNED BYTE *pbSrc, *pbDest, *pbDestMin, *pbDestLim, *pbDestLine;

    pbSrc = ((BYTE *) pvData) + dwOffset;

    if(pIH->biHeight < 0)
    {
        dwDstInc = m_Pitch;
        pbDest = (BYTE *) m_pData;
    }
    else
    {
        dwDstInc = 0 - (DWORD)m_Pitch;
        pbDest = (BYTE *) m_pData + (dwHeight - 1) * m_Pitch;
    }

    pbDestLine = pbDest;
    pbDestMin = (BYTE *) m_pData;
    pbDestLim = (BYTE *) m_pData + dwHeight * m_Pitch;



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
                        pbDestLine -= m_Pitch;
                        pbDest = pbDestLine;
                        break;

                    case 1:
                        pbDest = pbDestMin - m_Pitch;
                        break;

                    case 2:
                        pbDest += pbSrc[2] - pbSrc[3] * m_Pitch;
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
                        pbDestLine -= m_Pitch;
                        pbDest = pbDestLine;
                        break;

                    case 1:
                        pbDest = pbDestMin - m_Pitch;
                        break;

                    case 2:
                        pbDest += pbSrc[2] - pbSrc[3] * m_Pitch;
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




//-----------------------------------------------------------------------------
// Name: struct TGAHEADER
// Desc: Defines the header format for TGA files
//-----------------------------------------------------------------------------
#pragma pack(1)
struct TGAHEADER
{
    BYTE IDLength;
    BYTE ColormapType;
    BYTE ImageType;

    WORD wColorMapIndex;
    WORD wColorMapLength;
    BYTE bColorMapBits;

    WORD wXOrigin;
    WORD wYOrigin;
    WORD wWidth;
    WORD wHeight;
    BYTE PixelDepth;
    BYTE ImageDescriptor;
};
#pragma pack()




//-----------------------------------------------------------------------------
// Name: LoadTGA()
// Desc: Attempts to load the given data as a TGA file
//-----------------------------------------------------------------------------
HRESULT CImage::LoadTGA( const VOID* pvData, DWORD cbData )
{
    // Validate header.  TGA files don't seem to have any sort of magic number
    // to identify them.  Therefore, we will proceed as if this is a real TGA
    // file, until we see something we don't understand.

    BYTE*      pbData = (BYTE*)pvData;
    TGAHEADER* pFH    = (TGAHEADER*)pbData;

    if( cbData < sizeof(TGAHEADER) )
        return E_FAIL;

    if( pFH->ColormapType & ~0x01 )
        return E_FAIL;

    if( pFH->ImageType & ~0x0b )
        return E_FAIL;

    if( !pFH->wWidth || !pFH->wHeight )
        return E_FAIL;



    // Colormap size and format
    UINT uColorMapBytes = ((UINT) pFH->bColorMapBits + 7) >> 3;
    D3DFORMAT ColorMapFormat = D3DFMT_UNKNOWN;

    if(pFH->ColormapType)
    {
        switch(pFH->bColorMapBits)
        {
            case 15: ColorMapFormat = D3DFMT_X1R5G5B5; break;
            case 16: ColorMapFormat = D3DFMT_A1R5G5B5; break;
            case 24: ColorMapFormat = D3DFMT_X8R8G8B8; break;
            case 32: ColorMapFormat = D3DFMT_A8R8G8B8; break;
            default: return E_FAIL;
        }
    }


    // Image size and format
    UINT uBytes = ((UINT) pFH->PixelDepth + 7) >> 3;
    D3DFORMAT Format = D3DFMT_UNKNOWN;

    switch(pFH->ImageType & 0x03)
    {
        case 1:
            if(!pFH->ColormapType)
                return E_FAIL;

            switch(pFH->PixelDepth)
            {
                case 8: Format = D3DFMT_P8; break;
                default: return E_FAIL;
            }
            break;

        case 2:
            switch(pFH->PixelDepth)
            {
                case 15: Format = D3DFMT_X1R5G5B5; break;
                case 16: Format = D3DFMT_A1R5G5B5; break;
                case 24: Format = D3DFMT_X8R8G8B8;   break;
                case 32: Format = D3DFMT_A8R8G8B8; break;
                default: return E_FAIL;
            }
            break;

        case 3:
            switch(pFH->PixelDepth)
            {
                case 8: Format = D3DFMT_L8; break;
                default: return E_FAIL;
            }
            break;

        default:
            return E_FAIL;
    }

    BOOL bRLE         = pFH->ImageType & 0x08;
    BOOL bTopToBottom = 0x20 == (pFH->ImageDescriptor & 0x20);
    BOOL bLeftToRight = 0x10 != (pFH->ImageDescriptor & 0x10);

    pbData += sizeof(TGAHEADER);
    cbData -= sizeof(TGAHEADER);


    // Skip ID
    if(cbData < pFH->IDLength)
        return E_FAIL;

    pbData += pFH->IDLength;
    cbData -= pFH->IDLength;


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

                case D3DFMT_X8R8G8B8:
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
    UINT cbImage;
    if(Format == D3DFMT_X8R8G8B8)
        cbImage = (UINT) pFH->wWidth * (UINT) pFH->wHeight * (uBytes+1);
    else
        cbImage = (UINT) pFH->wWidth * (UINT) pFH->wHeight * uBytes;

    m_Format  = Format;
    m_pData   = pbData;
    m_Pitch   = (UINT) pFH->wWidth * uBytes;

    m_Width  = pFH->wWidth;
    m_Height = pFH->wHeight;


    if(!bRLE && bTopToBottom && bLeftToRight )
    {
        // Data is already in a format usable to D3D.. no conversion is necessary
        
        m_pData = new BYTE[cbImage];
        memcpy( m_pData, pbData, min(cbData, cbImage) );
        m_bDeleteData = TRUE;

        pbData += cbImage;
        cbData -= cbImage;
    }
    else
    {
        // Image data is compressed, or does not have origin at top-left
        if(!(m_pData = new BYTE[cbImage]))
            return E_OUTOFMEMORY;

        m_bDeleteData = TRUE;


        BYTE *pbDestY = bTopToBottom ? (BYTE *) m_pData : ((BYTE *) m_pData + (pFH->wHeight - 1) * m_Pitch);

        for(UINT uY = 0; uY < pFH->wHeight; uY++)
        {
            BYTE *pbDestX = bLeftToRight ? pbDestY : (pbDestY + m_Pitch - uBytes);

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

            pbDestY = bTopToBottom ? (pbDestY + m_Pitch) : (pbDestY - m_Pitch);
        }
    }

    if(Format == D3DFMT_X8R8G8B8)
    {
        //convert from 24-bit R8G8B8 to 32-bit X8R8G8B8
        // do the conversion in-place
        BYTE *pSrc, *pDst;
        pSrc = (BYTE *)m_pData + (m_Height)*(m_Width*uBytes) - uBytes;
        pDst = (BYTE *)m_pData + (m_Height)*(m_Width*(uBytes+1)) - (uBytes+1);
            
        while(pSrc >= m_pData)
        {
            *(pDst+3) = 0xff;       //A
            *(pDst+2) = *(pSrc+2);  //R
            *(pDst+1) = *(pSrc+1);  //G
            *(pDst+0) = *pSrc;      //B
            pSrc -= 3;
            pDst -= 4;
        }
        m_Pitch   = m_Width * (uBytes+1);
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Anonymous enum
// Desc: Enumerations used for loading PPM files
//-----------------------------------------------------------------------------
enum
{
    PPM_WIDTH, PPM_HEIGHT, PPM_MAX, PPM_DATA_R, PPM_DATA_G, PPM_DATA_B
};




//-----------------------------------------------------------------------------
// Name: LoadPPM()
// Desc: Attempts to load the given data as a PPM file
//-----------------------------------------------------------------------------
HRESULT CImage::LoadPPM( const VOID* pvData, DWORD cbData )
{
    BYTE *pbData = (BYTE *)pvData;

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

                    if(!(m_pData = new BYTE[uWidth * uHeight * sizeof(DWORD)]))
                        return E_OUTOFMEMORY;

                    m_bDeleteData = TRUE;

                    pdw = (DWORD *) m_pData;
                    pdwLim = pdw + uWidth * uHeight;

                    m_Format = D3DFMT_X8R8G8B8;
                    m_Pitch  = uWidth * sizeof(DWORD);

                    m_Width  = uWidth;
                    m_Height = uHeight;
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




