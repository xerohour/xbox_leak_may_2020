/*==========================================================================;
 *
 *  Copyright (C) 2000 - 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       surfacetofile.cpp
 *  Content:    Utility function to dump surface contents to a .bmp file
 *
 ****************************************************************************/

#if XBOX
    #include "xgrphseg.h"   
    #include "xtl.h"
    
#else
    #include "windows.h"
    #include "d3d8-xbox.h"
#endif

#include "stdio.h"
#include "xgraphics.h"
#include "debug.h"

namespace XGRAPHICS
{

// only define the following structs if they haven't already been defined in
// another header file.
#ifndef BI_RGB

#pragma pack (push, 2)

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

typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#ifndef BI_BITFIELDS
#define BI_BITFIELDS  3L
#endif  // BI_BITFIELDS
#define BI_JPEG       4L
#define BI_PNG        5L

#pragma pack (pop)

#endif  // defined BI_RGB

extern "C"
HRESULT WINAPI XGWriteSurfaceToFile(IDirect3DSurface8 *pSurf, const char *cPath)
{
    // Write an image to disk in .bmp format
    
    // find out about the surface
    D3DSURFACE_DESC SurfDesc;
    pSurf->GetDesc(&SurfDesc);

    DWORD ImageSize = SurfDesc.Width * SurfDesc.Height * 3;
    
    BITMAPFILEHEADER fileheader;
    BITMAPINFOHEADER bmiheader;
    D3DLOCKED_RECT BBLock;
    INT row, col;

    // Only handle X8R8G8B8 and R5G6B5 formats
    switch(SurfDesc.Format)
    {
    case D3DFMT_LIN_X8R8G8B8:
    case D3DFMT_LIN_A8R8G8B8:
    case D3DFMT_LIN_R5G6B5:
    case D3DFMT_LIN_X1R5G5B5:
        break;  // valid format

    default:
        DXGRIP("WriteSurfaceToFile: Unsupported format\n");
        return E_FAIL;
    }
    
    bmiheader.biBitCount = 24;
    bmiheader.biCompression = BI_RGB;
    bmiheader.biSize = sizeof(BITMAPINFOHEADER);
    bmiheader.biWidth = SurfDesc.Width;
    bmiheader.biHeight = SurfDesc.Height;
    bmiheader.biPlanes = 1;
    bmiheader.biSizeImage = ImageSize;
    bmiheader.biXPelsPerMeter = 0;
    bmiheader.biYPelsPerMeter = 0;
    bmiheader.biClrUsed = 0;
    bmiheader.biClrImportant = 0;

    fileheader.bfType = 0x4d42;
    fileheader.bfSize = sizeof(BITMAPFILEHEADER) +
                        sizeof(BITMAPINFOHEADER) +
                        bmiheader.biSizeImage;
    fileheader.bfReserved1 = 0;
    fileheader.bfReserved2 = 0;
    fileheader.bfOffBits = sizeof(BITMAPFILEHEADER) +
                           sizeof(BITMAPINFOHEADER);


    // open the file and write the image
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
    
    WriteFile(hFile, &fileheader, sizeof(BITMAPFILEHEADER), &BytesWritten, NULL);
    WriteFile(hFile, &bmiheader, sizeof(BITMAPINFOHEADER), &BytesWritten, NULL);
    // lock the surface so it can be read
    if(FAILED(pSurf->LockRect(&BBLock, NULL, D3DLOCK_READONLY | D3DLOCK_TILED)))
    {
        DXGRIP("WriteSurfaceToFile: Could not lock surface\n");
        return E_FAIL;
    }

    // temporary buffer - first attempt to allocate a full-size buffer
    #define STACKBUFSIZE (512)
    BYTE stackpixbuf[STACKBUFSIZE];
    DWORD bytecount=0;
    DWORD tmpbufsize;
    BYTE *heapbuffer = new BYTE[ImageSize];
    BYTE *pixbuf;
    if(heapbuffer)
    {
        // successfully allocated full-size buffer, use it
        pixbuf = heapbuffer;
        tmpbufsize = ImageSize;
    }
    else
    {
        // couldn't allocate big buffer so use small buffer on the stack
        // (much slower)
        pixbuf = stackpixbuf;
        tmpbufsize = STACKBUFSIZE;
    }

    WORD *psrc16;
    DWORD *psrc;
    switch(SurfDesc.Format)
    {
    case D3DFMT_LIN_X8R8G8B8:
    case D3DFMT_LIN_A8R8G8B8:
        // convert from 32-bit to 24-bit
        for(row=SurfDesc.Height-1; row >= 0; row--)
        {
            psrc = (DWORD*)((BYTE*)BBLock.pBits + row*BBLock.Pitch);
            for(col=0; col < (INT)SurfDesc.Width; col++)
            {
                pixbuf[bytecount++] = (BYTE)((*psrc )      & 0xff);
                pixbuf[bytecount++] = (BYTE)((*psrc >>  8) & 0xff);
                pixbuf[bytecount++] = (BYTE)((*psrc >> 16) & 0xff);
                psrc++;
                if(bytecount >= (tmpbufsize-2))
                {
                    WriteFile(hFile, pixbuf, bytecount, &BytesWritten, NULL);
                    bytecount=0;
                }
            }
        }
        WriteFile(hFile, pixbuf, bytecount, &BytesWritten, NULL);
        bytecount=0;
        break;
    case D3DFMT_LIN_R5G6B5:
        // convert from 16-bit (565) to 24-bit (888)
        for(row=SurfDesc.Height-1; row >= 0; row--)
        {
            psrc16 = (WORD*)((BYTE*)BBLock.pBits + row*BBLock.Pitch);
            for(col=0; col < (INT)SurfDesc.Width; col++)
            {
                pixbuf[bytecount++] = (BYTE)((*psrc16 <<  3) & 0xf8);
                pixbuf[bytecount++] = (BYTE)((*psrc16 >>  3) & 0xfc);
                pixbuf[bytecount++] = (BYTE)((*psrc16 >>  8) & 0xf8);
                psrc16++;
                if(bytecount >= (tmpbufsize-2))
                {
                    WriteFile(hFile, pixbuf, bytecount, &BytesWritten, NULL);
                    bytecount=0;
                }
            }
        }
        WriteFile(hFile, pixbuf, bytecount, &BytesWritten, NULL);
        bytecount=0;
        break;
    
    case D3DFMT_LIN_X1R5G5B5:
        // convert from 16-bit (555) to 24-bit (888)
        for(row=SurfDesc.Height-1; row >= 0; row--)
        {
            psrc16 = (WORD*)((BYTE*)BBLock.pBits + row*BBLock.Pitch);
            for(col=0; col < (INT)SurfDesc.Width; col++)
            {
                pixbuf[bytecount++] = (BYTE)((*psrc16 <<  3) & 0xf8);
                pixbuf[bytecount++] = (BYTE)((*psrc16 >>  2) & 0xf8);
                pixbuf[bytecount++] = (BYTE)((*psrc16 >>  7) & 0xf8);
                psrc16++;
                if(bytecount >= (tmpbufsize-2))
                {
                    WriteFile(hFile, pixbuf, bytecount, &BytesWritten, NULL);
                    bytecount=0;
                }
            }
        }
        WriteFile(hFile, pixbuf, bytecount, &BytesWritten, NULL);
        bytecount=0;
        break;
    }

    pSurf->UnlockRect();
    
    CloseHandle(hFile); // finished writing image
    
    if(heapbuffer)
        delete [] heapbuffer; // finished with temporary buffer
    
    return S_OK;
}

}

