//-----------------------------------------------------------------------------
// FILE: BITFONT.CPP
//
// Desc: bit font rasterizer
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "bitfont.h"
#include <stdio.h>
#include <assert.h>
#include <D3DX8Math.h>
#include <xgraphics.h>

DWORD dwColorFromD3DXColor(D3DFORMAT dwSurfaceFormat, D3DCOLOR color);

// default font: 'Small Fonts' size 7
static const DWORD rgSmallFont[] =
{
    0x30304246, 0x0000000b, 0x00000060, 0x00000002,
    0x000b0200, 0x16040000, 0x06000000, 0x00000021,
    0x00002c06, 0x00370700, 0x42060000, 0x02000000,
    0x0000004d, 0x00005803, 0x00630300, 0x6e030000,
    0x04000000, 0x00000079, 0x00008402, 0x008f0300,
    0x9a020000, 0x03000000, 0x000000a5, 0x0000b005,
    0x00bb0300, 0xc6050000, 0x05000000, 0x000000d1,
    0x0000dc05, 0x00e70500, 0xf2050000, 0x05000000,
    0x000000fd, 0x00010805, 0x01130500, 0x1e020000,
    0x02000001, 0x00000129, 0x00013405, 0x013f0500,
    0x4a050000, 0x05000001, 0x00000155, 0x00016009,
    0x01760700, 0x81070000, 0x07000001, 0x0000018c,
    0x00019707, 0x01a20600, 0xad060000, 0x07000001,
    0x000001b8, 0x0001c307, 0x01ce0200, 0xd9050000,
    0x06000001, 0x000001e4, 0x0001ef05, 0x01fa0900,
    0x10070000, 0x07000002, 0x0000021b, 0x00022607,
    0x02310700, 0x3c070000, 0x06000002, 0x00000247,
    0x00025206, 0x025d0600, 0x68060000, 0x08000002,
    0x00000273, 0x00027e06, 0x02890600, 0x94060000,
    0x03000002, 0x0000029f, 0x0002aa03, 0x02b50300,
    0xc0040000, 0x05000002, 0x000002cb, 0x0002d603,
    0x02e10500, 0xec050000, 0x05000002, 0x000002f7,
    0x00030205, 0x030d0500, 0x18030000, 0x05000003,
    0x00000323, 0x00032e05, 0x03390200, 0x44020000,
    0x05000003, 0x0000034f, 0x00035a02, 0x03650800,
    0x70050000, 0x06000003, 0x0000037b, 0x00038605,
    0x03910500, 0x9c030000, 0x05000003, 0x000003a7,
    0x0003b203, 0x03bd0500, 0xc8050000, 0x06000003,
    0x000003d3, 0x0003de04, 0x03e90400, 0xf4040000,
    0x03000003, 0x000003ff, 0x00040a02, 0x04150300,
    0x20050000, 0x02000004, 0x0000042b, 0xc0c0c0c0,
    0xc0c0c0c0, 0xc0c0c0c0, 0x808080c0, 0x80c08080,
    0xf0f0c0c0, 0xf0f0a0a0, 0xf0f0f0f0, 0xd4fcfcf0,
    0x80d480d4, 0xfcfcd4d4, 0xc4ecfcfc, 0xa8e4cca8,
    0xfefcecc4, 0x9a9cfefe, 0xb8d8eef6, 0xfefefefe,
    0xaadeacde, 0xfec8b4b6, 0x9efefefe, 0xf6eadeac,
    0xfefec8f4, 0xacdefefe, 0xb4b6aabe, 0xfedebea8,
    0xdeccbefe, 0xc8d4d6ca, 0xfefebede, 0xeafe8c9e,
    0xfee8f4f6, 0xfefefefe, 0x86dafefc, 0xfefef8d4,
    0xfcfefefe, 0xd4c6dafe, 0xfefebeb8, 0xfefcfefe,
    0xf8f486fa, 0xfefefefe, 0xfafefcfe, 0xfeb8f4c6,
    0xdefefefe, 0xa69adedc, 0xfefeb8b4, 0xb4cefefe,
    0xb4b6b2b6, 0xfefefec8, 0xd694cefe, 0xc8d4d6d2,
    0xfefefefe, 0xeaf6b4ce, 0xfe80bcde, 0xcefefefe,
    0xf6eaf6b4, 0xfefec8b4, 0xcceefefe, 0xec86aace,
    0xfefefee8, 0x8ebc86fe, 0xc8b4f6b2, 0xfefefefe,
    0x8abeb4ce, 0xfec8b4b6, 0x86fefefe, 0xdeeaeef4,
    0xfefed8dc, 0xb4cefefe, 0xb4b6cab6, 0xfefefec8,
    0xb6b4cefe, 0xc8b4f6c2, 0xfefefefe, 0x82f6f4ce,
    0xfe88f4f6, 0xcefefefe, 0xf682f6f4, 0xfebe88f4,
    0xfcfefefe, 0xdcbedaee, 0xfefefee8, 0xfefcfefe,
    0xf88cfe8a, 0xfefefefe, 0xdabefcfe, 0xfeb8dcee,
    0xcefefefe, 0xdeeaf6b4, 0xfefed8fc, 0x80ff80ff,
    0x80de80e1, 0x00ab00b3, 0x80b400ab, 0x80e180df,
    0xffff80ff, 0xdbdbe6e7, 0xffbdbcc3, 0x83ffffff,
    0xbd83bdbc, 0xffff83bc, 0xbcc3ffff, 0xbcbfbfbf,
    0xffffffc3, 0xbdbc83ff, 0x83bcbdbd, 0xffffffff,
    0x85bdbc83, 0xff83bcbd, 0x83ffffff, 0xbd85bdbc,
    0xffffbfbc, 0xbcc3ffff, 0xb8bdb1bf, 0xffffffc5,
    0xbdbcbdff, 0xbdbcbd81, 0xffffffff, 0x81bdbcbd,
    0xffbdbcbd, 0xf5ffffff, 0xb5f1f5f4, 0xffffcdb4,
    0xb4b9ffff, 0xb4ad9dad, 0xffffffb9, 0xbdbcb9ff,
    0x81bcbdbd, 0x80ffffff, 0x009e80ff, 0x00ad009e,
    0x00b300ad, 0x00bf00b3, 0x80ff80ff, 0x9c9cffff,
    0xb5b5adad, 0xffffffb9, 0xbdbcc2ff, 0xc3bdbdbd,
    0xffffffff, 0x83bdbc82, 0xffbfbfbf, 0xc2ffffff,
    0xb5bdbdbc, 0xfffdc3b9, 0xbc82ffff, 0xbdbd83bd,
    0xffffffbd, 0xbdb8c6ff, 0xc5b9f9c7, 0xffffffff,
    0xefedec82, 0xffededed, 0xbaffffff, 0xb9bbb9b8,
    0xffffc5b9, 0xb8baffff, 0xedd5d7d5, 0xffffffed,
    0xddbebeff, 0xebebd5d5, 0xffffffff, 0xedd5baba,
    0xffbbbbd5, 0xbaffffff, 0xededd5ba, 0xffffefef,
    0xfa82ffff, 0xbfddedf5, 0xffffff83, 0xb5ba82ff,
    0xa3bfbdad, 0xffff9fbf, 0xadb5bae2, 0xffc3df9d,
    0x82ffffff, 0xddcdd5da, 0x9fdfc3df, 0xaad2ffff,
    0xfffdfdf5, 0xfffffff3, 0xfdfafaff, 0xfbfffdfd,
    0xffff07ff, 0xfdfddaba, 0xfffbfffd, 0xfaffffe7,
    0xc5f5cdfa, 0xffffc3b7, 0xbabaffff, 0xb7b5b58d,
    0xffffff8b, 0xcdfafaff, 0xcbb7bdb5, 0xffffffff,
    0xb5c5f2f2, 0xffc3b7b5, 0xfaffffff, 0x85b5cdfa,
    0xffffcbbf, 0xbadaffff, 0xbfa595ad, 0xffffffab,
    0xc5fafaff, 0xc3b7b5b5, 0xffff8ff7, 0x95adbaba,
    0xffb3b7b5, 0xbaffffff, 0xb595adfa, 0xffffb3b7,
    0xfabaffff, 0xb7b595ad, 0xffbfbfb3, 0xadbabaff,
    0xb3af9d9d, 0xffffffff, 0x9dadbaba, 0xffb3af9d,
    0xffffffff, 0xb6b681ff, 0xffffb6b6, 0xffffffff,
    0xb6b6b689, 0xffffffb6, 0xc5ffffff, 0xc6bababa,
    0xffffffff, 0xb28dffff, 0xbf8eb2b2, 0xffffffbf,
    0xb2b2c5ff, 0xf7f7c6b2, 0xffffffff, 0xb2b2b285,
    0xfff7f7a6, 0xc5ffffff, 0x8ef2caba, 0xffffffff,
    0xba85bfff, 0xffceb2aa, 0xffffffff, 0xb2b2b5ff,
    0xffffc6b2, 0xffffffff, 0xcab2b2b5, 0xffffffce,
    0xb9ffffff, 0xd6d6aaaa, 0xffffffff, 0xaaa9ffff,
    0xffa6a6da, 0xffffffff, 0xaaaaa9ff, 0x9fefc6a6,
    0xffffffff, 0xb6daea89, 0xffffff86, 0xc9dfdfff,
    0xc6d69aca, 0xffffdfdf, 0x8a899fdf, 0x9f86969a,
    0xbfffffdf, 0xdaaaa9bf, 0xbfbfa6b6, 0xd7ffffff,
    0xfefafaa9, 0xfffffffe, 0xa997ffff, 0xbebebaba,
    0x0000ffbf, 0x0006fe21, 0x0fffff9d, 0x00000002,
};

//----------------------------------------------------------------------------
BitFont::BitFont()
{
    // start out with our default font
    m_pBitFontInfo = (BitFontInfo *)rgSmallFont;
}

//----------------------------------------------------------------------------
BitFont::~BitFont()
{
    if(m_pBitFontInfo != (BitFontInfo *)rgSmallFont)
        delete m_pBitFontInfo;
}

//----------------------------------------------------------------------------
// Intialize a BitFont from a specified filename
bool BitFont::Load(const WCHAR* filename)
{
	bool retval = false;
	BitFontInfo *pBitFontInfo = NULL;

	HANDLE hFile = CreateFile(filename, 
		                      GENERIC_READ, 
							  FILE_SHARE_READ, 
							  NULL, 
							  OPEN_EXISTING, 
							  FILE_ATTRIBUTE_NORMAL, 
							  NULL);


    if (hFile == INVALID_HANDLE_VALUE)
    {
		goto err;
	}

    // Get the file size.
	BY_HANDLE_FILE_INFORMATION info;

	if (!GetFileInformationByHandle(hFile, &info))
	{
		goto err;
	}

    // need at least a BITFONT and one BITFONTINFO, but the
	// file can be too big as well.
	//
	if (info.nFileSizeLow < sizeof(BitFontInfo) + sizeof(BitFontCharInfo)
		|| info.nFileSizeHigh != 0)
	{
		goto err;
	}

	// Allocate the BITFONT.
	pBitFontInfo = (BitFontInfo *)new BYTE[info.nFileSizeLow];

	if (!pBitFontInfo)
	{
		goto err;
	}

	// Load it.
	if (!ReadFile(hFile, pBitFontInfo, info.nFileSizeLow, NULL, NULL))
	{
		goto err;
	}

    // if the sig doesn't match or we don't have any bitfontinfos then bail
    if (pBitFontInfo->dwSig != BITFONT_SIG || !pBitFontInfo->cBfis)
	{
        goto err;
	}

	// Delete any old fonts.
    if(m_pBitFontInfo != (BitFontInfo *)rgSmallFont)
	{
		delete m_pBitFontInfo;
	}

	// Set the new.
	m_pBitFontInfo = pBitFontInfo;
	pBitFontInfo = NULL;

	retval = true;

err:
	if (pBitFontInfo)
	{
		delete pBitFontInfo;
	}

    if (hFile != INVALID_HANDLE_VALUE)
	{
        CloseHandle(hFile);
	}

    return retval;
}


//----------------------------------------------------------------------------
// Returns pixel height and width of string
void BitFont::GetTextStringLength(DWORD *pdwWidth, DWORD *pdwHeight, const WCHAR *str)
{
    if(pdwHeight)
        *pdwHeight = m_pBitFontInfo->dwFontHeight;

    if(pdwWidth)
    {
        DWORD dwWidth = 0;

        // go through the string adding up the widths
        for(const WCHAR *sz = str; *sz; sz++)
        {
            DWORD iBfi = *sz - 32;

            if(iBfi >= m_pBitFontInfo->cBfis)
                iBfi = 0;

            dwWidth += m_pBitFontInfo->rgBfi[iBfi].bWidth;
        }

        *pdwWidth = dwWidth;
    }
}

//----------------------------------------------------------------------------
void DrawText16
(
    D3DSURFACE_DESC*    pDesc,
    D3DLOCKED_RECT*     pLock,
    BitFontInfo*        m_pBitFontInfo,
    const WCHAR*        str,
    int                 iX,
    int                 iY,
    DWORD               dwFlags,
    DWORD               dwcolFore,
    DWORD               dwcolBack
)
{
    bool fdrawBkgnd = !(dwFlags & DRAWTEXT_TRANSPARENTBKGND);

    // rgColor[0] is background color
    WORD rgColor[2] = { LOWORD(dwcolBack), LOWORD(dwcolFore) };

    // pointer to font bytestream data
    BYTE *lpData = (BYTE *)&m_pBitFontInfo->rgBfi[m_pBitFontInfo->cBfis];
    // pointer to end of dest surface
    WORD *pwSurfaceMaxY = (WORD *)((BYTE *)pLock->pBits + pLock->Pitch * pDesc->Height);

    // go through the string
    for(const WCHAR *sz = str; *sz; sz++)
    {
        DWORD iBfi = *sz - 32;

        if(iBfi >= m_pBitFontInfo->cBfis)
            iBfi = 0;

        // get data for this char
        BitFontCharInfo *pbfi = &m_pBitFontInfo->rgBfi[iBfi];
        BYTE *lpBits = &lpData[pbfi->dwOffset];

        // where we're drawing
        WORD *pwSurface = (WORD *)((BYTE *)pLock->pBits + pLock->Pitch * iY) + iX;
        // max x for the line we're on
        WORD *pwSurfaceMaxX = (WORD *)((BYTE *)pLock->pBits + pLock->Pitch * iY) + pDesc->Width;

        // go through bitfont drawing each line
        for(DWORD y = 0; y < m_pBitFontInfo->dwFontHeight; y++)
        {
            DWORD dwWidth = pbfi->bWidth;

            while(dwWidth)
            {
                DWORD dwByte = *lpBits++;
                DWORD dwNumBits = min(dwWidth, 8);

                dwWidth -= dwNumBits;

                // check number of bits and clipping
                while(dwNumBits-- && (pwSurface < pwSurfaceMaxX))
                {
                    DWORD iIndex = !(dwByte & 0x80);

                    if(fdrawBkgnd || iIndex)
                        *pwSurface = rgColor[iIndex];

                    pwSurface++;
                    dwByte <<= 1;
                }
            }

            // get next line data
            pwSurface = (WORD *)((BYTE *)(pwSurface - pbfi->bWidth - dwWidth) + pLock->Pitch);
            pwSurfaceMaxX = (WORD *)((BYTE *)pwSurfaceMaxX + pLock->Pitch);

            // check clipping
            if(pwSurface >= pwSurfaceMaxY)
                break;
        }

        // move to next char position
        iX += pbfi->bWidth;
    }
}

//----------------------------------------------------------------------------
void DrawText32
(
    D3DSURFACE_DESC*    pDesc,
    D3DLOCKED_RECT*     pLock,
    BitFontInfo*        m_pBitFontInfo,
    const WCHAR*        str,
    int                 iX,
    int                 iY,
    DWORD               dwFlags,
    DWORD               dwcolFore,
    DWORD               dwcolBack
)
{
    bool fdrawBkgnd = !(dwFlags & DRAWTEXT_TRANSPARENTBKGND);
    DWORD rgColor[2] = { dwcolBack, dwcolFore };

    BYTE *lpData = (BYTE *)&m_pBitFontInfo->rgBfi[m_pBitFontInfo->cBfis];
    DWORD *pdwSurfaceMaxY = (DWORD *)((BYTE *)pLock->pBits + pLock->Pitch * pDesc->Height);

    for(const WCHAR *sz = str; *sz; sz++)
    {
        DWORD iBfi = *sz - 32;

        if(iBfi >= m_pBitFontInfo->cBfis)
            iBfi = 0;

        BitFontCharInfo *pbfi = &m_pBitFontInfo->rgBfi[iBfi];
        BYTE *lpBits = &lpData[pbfi->dwOffset];

        DWORD *pdwSurface = (DWORD *)((BYTE *)pLock->pBits + pLock->Pitch * iY) + iX;
        DWORD *pdwSurfaceMaxX = (DWORD *)((BYTE *)pLock->pBits + pLock->Pitch * iY) + pDesc->Width;

        for(DWORD y = 0; y < m_pBitFontInfo->dwFontHeight; y++)
        {
            DWORD dwWidth = pbfi->bWidth;

            while(dwWidth)
            {
                DWORD dwByte = *lpBits++;
                DWORD dwNumBits = min(dwWidth, 8);

                dwWidth -= dwNumBits;

                while(dwNumBits-- && (pdwSurface < pdwSurfaceMaxX))
                {
                    DWORD iIndex = !(dwByte & 0x80);

                    if(fdrawBkgnd || iIndex)
                        *pdwSurface = rgColor[iIndex];

                    pdwSurface++;
                    dwByte <<= 1;
                }
            }

            pdwSurface = (DWORD *)((BYTE *)(pdwSurface - pbfi->bWidth - dwWidth) + pLock->Pitch);
            pdwSurfaceMaxX = (DWORD *)((BYTE *)pdwSurfaceMaxX + pLock->Pitch);
            if(pdwSurface >= pdwSurfaceMaxY)
                break;
        }

        iX += pbfi->bWidth;
    }
}

//----------------------------------------------------------------------------
// Draw the specified text to a surface
void BitFont::DrawText
(
    IDirect3DSurface8 *pSurface,
    const WCHAR*    str,
    int             iX,
    int             iY,
    DWORD           dwFlags,
    D3DCOLOR        colFore,
    D3DCOLOR        colBack
)
{
    typedef void (*PFNDRAWTEXT)(
        D3DSURFACE_DESC*    pDesc,
        D3DLOCKED_RECT*     pLock,
        BitFontInfo*        m_pBitFontInfo,
        const WCHAR*        str,
        int                 iX,
        int                 iY,
        DWORD               dwFlags,
        DWORD               dwcolFore,
        DWORD               dwcolBack
    );

    PFNDRAWTEXT rgpfnDrawText[] =
    {
        DrawText16,
        DrawText32
    };

    D3DLOCKED_RECT lockRect;
    if(pSurface->LockRect(&lockRect, NULL,
            D3DLOCK_NOSYSLOCK | D3DLOCK_RAWDATA) != S_OK)
    {
        assert(FALSE);
        return;
    }

    D3DSURFACE_DESC desc;
    (void)pSurface->GetDesc(&desc);

    PVOID pBits = Swizzler_GetUnswizzledBits(&lockRect, &desc);
    if (pBits == NULL)
    {
        assert(FALSE);
        pSurface->UnlockRect();
        return;
    }

    PVOID pBitsOld = lockRect.pBits;
    lockRect.pBits = pBits;

    UINT BitsPerPixel =
        (desc.Format == D3DFMT_X8R8G8B8 
          || desc.Format == D3DFMT_A8R8G8B8
          || desc.Format == D3DFMT_LIN_X8R8G8B8
          || desc.Format == D3DFMT_LIN_A8R8G8B8) ? 32 : 16;

    // use the correct draw routine for the job
    rgpfnDrawText[BitsPerPixel == 32](
        &desc,
        &lockRect,
        m_pBitFontInfo,
        str,
        iX,
        iY,
        dwFlags,
        dwColorFromD3DXColor(desc.Format, colFore),
        dwColorFromD3DXColor(desc.Format, colBack)
    );

    lockRect.pBits = pBitsOld;
    Swizzler_SwizzleFromBits(pBits, &lockRect, &desc);

    // Relase the lock on the surface
    pSurface->UnlockRect();
}

//----------------------------------------------------------------------------
// Given a surfaceformet and color, return the dude as a native pixel
DWORD dwColorFromD3DXColor(D3DFORMAT dwSurfaceFormat, D3DCOLOR color)
{
    DWORD dwColor;
    D3DXCOLOR col(color);

    switch(dwSurfaceFormat)
    {
    // 32-bit
    case D3DFMT_A8R8G8B8:
    case D3DFMT_LIN_A8R8G8B8:
        dwColor =
            (((long)(col.a * 255)) << 24) |
            (((long)(col.r * 255)) << 16) |
            (((long)(col.g * 255)) << 8) |
            (long)(col.b * 255);
        break;

    // 32-bit
    case D3DFMT_X8R8G8B8:
    case D3DFMT_LIN_X8R8G8B8:
        dwColor =
            0xff000000L |
            (((long)(col.r * 255)) << 16) |
            (((long)(col.g * 255)) << 8) |
            (long)(col.b * 255);
        break;

    // 16-bit
    case D3DFMT_R5G6B5:
    case D3DFMT_LIN_R5G6B5:
        dwColor =
            (((long)(col.r * 255)) << 11) |
            (((long)(col.g * 255)) << 5) |
            (long)(col.b * 255);
        break;
    case D3DFMT_X1R5G5B5:
    case D3DFMT_LIN_X1R5G5B5:
        dwColor =
            (1 << 15) |
            (((long)(col.r * 255)) << 10) |
            (((long)(col.g * 255)) << 5) |
            (long)(col.b * 255);
        break;
    case D3DFMT_A4R4G4B4:
    case D3DFMT_LIN_A4R4G4B4:
        dwColor =
            (((long)(col.a * 15)) << 12) |
            (((long)(col.r * 15)) << 8) |
            (((long)(col.g * 15)) << 4) |
            (long)(col.b * 15);
        break;
    case D3DFMT_A1R5G5B5:
    case D3DFMT_LIN_A1R5G5B5:
        dwColor =
            (((long)(col.a > .9f)) << 15) |
            (((long)(col.r * 31)) << 10) |
            (((long)(col.g * 31)) << 5) |
            (long)(col.b * 31);
        break;

    default:
        dwColor = 0;
        assert(FALSE);
        break;
    }
    return dwColor;
};

//----------------------------------------------------------------------------
// Map our 'new' directly to LocalAlloc
void* __cdecl operator new(size_t size)
{
    return LocalAlloc(0, size);
}

//----------------------------------------------------------------------------
// Map our 'delete' directly to LocalFree   
VOID __cdecl operator delete(VOID* alloc)
{
    LocalFree(alloc);
}
