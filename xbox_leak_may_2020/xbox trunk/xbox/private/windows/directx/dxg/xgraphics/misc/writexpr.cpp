/*==========================================================================;
 *
 *  Copyright (C) 2000 - 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       writexpr.cpp
 *  Content:    Utility function to dump a single texture or surface to 
 *              a packed resource file (.xpr)
 *
 ****************************************************************************/

#if XBOX
#include <xtl.h>
#else
#include <windows.h>
#include <d3d8-xbox.h>
#endif

#include <stdio.h>
#include <xgraphics.h>
#include <debug.h>
#include "header.h"

namespace XGRAPHICS
{

#define AGP_FRAME_BUFFER_HIGH_NIBBLE    0xF0000000

extern "C"
HRESULT 
WINAPI XGWriteSurfaceOrTextureToXPR(IDirect3DResource8 *pResource, 
                                    const char *cPath, 
                                    BOOL bWriteSurfaceAsTexture)
{
    DWORD temp, Type, Width, Height, Depth, Levels, Pitch, Dimension;
    D3DFORMAT Format;
    bool isInVideoMemory, isCubeMap, isVolume;
    D3DPixelContainer *pContainer = (D3DPixelContainer *)pResource;
    BYTE chunk[2048];   // used to write a chunk of the image

    // check for proper resource type
    Type = pContainer->Common & D3DCOMMON_TYPE_MASK;
    if((Type != D3DCOMMON_TYPE_TEXTURE) && (Type != D3DCOMMON_TYPE_SURFACE))
    {
        DXGRIP("ERROR: Invalid resource type in XGWriteSurfaceOrTextureToXPR\n");
        return E_FAIL;
    }

    Format = (D3DFORMAT)(((pContainer->Format) & D3DFORMAT_FORMAT_MASK) >> D3DFORMAT_FORMAT_SHIFT);
    Dimension = ((pContainer->Format) & D3DFORMAT_DIMENSION_MASK) >> D3DFORMAT_DIMENSION_SHIFT;
    if((pContainer->Format) & D3DFORMAT_CUBEMAP)
        isCubeMap = TRUE;
    else
        isCubeMap = FALSE;

    if((Dimension == 3) && !isCubeMap)
        isVolume = TRUE;
    else
        isVolume = FALSE;
    
    isInVideoMemory = TRUE;

    DWORD dwBytesPerPixel = (g_TextureFormat[Format] & FMT_BITSPERPIXEL)/8;
    
    if(pContainer->Size == 0)
    {
        // swizzled texture or surface
        Levels = ((pContainer->Format) & D3DFORMAT_MIPMAP_MASK) >> D3DFORMAT_MIPMAP_SHIFT;
        Width  = 1 << ((pContainer->Format & D3DFORMAT_USIZE_MASK) >> D3DFORMAT_USIZE_SHIFT);
        Height = 1 << ((pContainer->Format & D3DFORMAT_VSIZE_MASK) >> D3DFORMAT_VSIZE_SHIFT);
        Depth  = 1 << ((pContainer->Format & D3DFORMAT_PSIZE_MASK) >> D3DFORMAT_PSIZE_SHIFT);
    }
    else
    {
        // linear texture or surface
        Levels = 1;
        Width = ((pContainer->Size) & D3DSIZE_WIDTH_MASK) + 1;
        Height = (((pContainer->Size) & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1;
        Depth = 1;
    }
    Pitch = 0;
    
    int dwResourceBytes = EncodeFormat(Width, 
                                       Height, 
                                       Depth, 
                                       Levels, 
                                       Format, 
                                       Pitch, 
                                       isCubeMap, 
                                       isVolume, 
                                       &temp, 
                                       &temp);
    
    // open the file and write out the data
    DWORD BytesWritten;
    
    HANDLE hFile = INVALID_HANDLE_VALUE;

    hFile = CreateFile(cPath,
                       GENERIC_WRITE, 
                       0, 
                       NULL, 
                       CREATE_ALWAYS, 
                       0, 
                       NULL);

    if(INVALID_HANDLE_VALUE == hFile)
    {
        char buf[256];
        sprintf(buf, "Unable to open file (%s):  error %x\n", cPath, GetLastError());
        DXGRIP(buf);
        return E_FAIL;
    }
    
    DWORD dwVal;

    // write the xpr header
    XPR_HEADER xprh;
    xprh.dwMagic = XPR_MAGIC_VALUE;
    xprh.dwTotalSize = dwResourceBytes + 2048;
    xprh.dwHeaderSize = 2048;

    WriteFile(hFile, &xprh, 3*sizeof(DWORD), &BytesWritten, NULL);
    
    DWORD Common = pContainer->Common;
    if((Type == D3DCOMMON_TYPE_SURFACE) && bWriteSurfaceAsTexture)
    {
        // clear type bits
        Common &= ~D3DCOMMON_TYPE_MASK;
        // set type as texture
        Common |= D3DCOMMON_TYPE_TEXTURE;
    }
    // clear the D3DCREATED bit
    Common &= ~D3DCOMMON_D3DCREATED;
    // write common field
    WriteFile(hFile, &Common, sizeof(DWORD), &BytesWritten, NULL);
    
    // write data field
    dwVal = 0;
    WriteFile(hFile, &dwVal, sizeof(DWORD), &BytesWritten, NULL);

    // write lock field
    dwVal = 1;
    WriteFile(hFile, &dwVal, sizeof(DWORD), &BytesWritten, NULL);
    
    // write Format and Size fields
    WriteFile(hFile, &(pContainer->Format), 2*sizeof(DWORD), &BytesWritten, NULL);

    // construct a virtual address pointer to the bits
    BYTE *pBits;
    D3DLOCKED_RECT lr;
    ((IDirect3DSurface8 *)pResource)->LockRect(&lr, NULL, D3DLOCK_TILED);
    pBits = (BYTE *)lr.pBits;

    if((Type == D3DCOMMON_TYPE_TEXTURE) || bWriteSurfaceAsTexture)
    {
        // write end-of-resources marker
        dwVal = 0xffffffff;
        WriteFile(hFile, &dwVal, sizeof(DWORD), &BytesWritten, NULL);
        // pad header to 2048
        WriteFile(hFile, chunk, 2048-9*sizeof(DWORD), &BytesWritten, NULL);
    }
    else
    {
        // write parent field
        dwVal = 0;
        WriteFile(hFile, &dwVal, sizeof(DWORD), &BytesWritten, NULL);
        // write end-of-resources marker
        dwVal = 0xffffffff;
        WriteFile(hFile, &dwVal, sizeof(DWORD), &BytesWritten, NULL);
        // pad header to 2048
        WriteFile(hFile, chunk, 2048-10*sizeof(DWORD), &BytesWritten, NULL);
    }
    while(dwResourceBytes > 0)
    {
        DWORD chunksize;
        chunksize = min(2048, dwResourceBytes);
        memcpy(chunk, pBits, chunksize);
        WriteFile(hFile, chunk, chunksize, &BytesWritten, NULL);
        dwResourceBytes -= chunksize;
        pBits += chunksize;
    }
    ((IDirect3DSurface8 *)pResource)->UnlockRect(); // finished with bits
    
    CloseHandle(hFile); // finished writing resource


    return S_OK;
}

}

