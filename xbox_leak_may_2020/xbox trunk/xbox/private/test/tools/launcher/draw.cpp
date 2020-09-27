/*

Copyright (c) 2000  Microsoft Corporation

Module Name:

    draw.cpp

Abstract:

    simplified functions that call Matt's dxconio functions... Used by CPX

Author:

    Jason Gould (a-jasgou) May 2000

*/

#include "stdafx.h"

#include <Xfont.h>
#include <d3dx8math.h>
#include "draw.h"

namespace Launcher {

typedef struct MYVERTEX
    {
    D3DXVECTOR3 v;
    float       fRHW;
    D3DCOLOR    cDiffuse;
    } MYVERTEX, *PMYVERTEX;

static IDirect3DDevice8* gDevice = 0;
static IDirect3DSurface8* gBackBuffer = 0;
static IDirect3DVertexBuffer8* gVertexBuffer = 0;

static int gScreenWidth = 640; 
static int gScreenHeight = 480;
static float gWidth = 8;
static float gHeight = 15;
static DWORD gColor = 0xff00ff00;
static DWORD gBkColor = 0xff000000;
static XFONT* gFont = 0;


Bitmap::Bitmap()
    {
    memset(&bitmapHeader, 0, sizeof(BITMAPFILEHEADER));
    memset(&bitmapInfo, 0, sizeof(BITMAPINFOHEADER));
    palatte = NULL;
    image = NULL;
    }

Bitmap::Bitmap(char *filename)
    {
    memset(&bitmapHeader, 0, sizeof(BITMAPFILEHEADER));
    memset(&bitmapInfo, 0, sizeof(BITMAPINFOHEADER));
    palatte = NULL;
    image = NULL;

    LoadFile(filename);
    }

Bitmap::~Bitmap()
    {
    if(palatte) delete[] palatte;
    if(image) delete[] image;
    }


DWORD Bitmap::LoadFile(char *filename)
    {
    FILE *in = fopen(filename, "rb");

    if(!in)
        {
        DebugPrint("BMP: Error opening file!\r\n");
        return 0;
        }

    if(palatte) delete[] palatte;
    if(image) delete[] image;

    fread(&bitmapHeader, sizeof(BITMAPFILEHEADER), 1, in);
    fread(&bitmapInfo, sizeof(BITMAPINFOHEADER), 1, in);
    unsigned palletSize = 1 << bitmapInfo.biBitCount;

/*
    DebugPrint("BMP: Loading %s\r\n", filename);
    DebugPrint("BMP: biSize -           %lu\r\n", bitmapInfo.biSize);
    DebugPrint("BMP: biWidth -          %lu\r\n", bitmapInfo.biWidth);
    DebugPrint("BMP: biHeight -         %lu\r\n", bitmapInfo.biHeight);
    DebugPrint("BMP: biPlanes -         %lu\r\n", bitmapInfo.biPlanes);
    DebugPrint("BMP: biBitCount -       %lu\r\n", bitmapInfo.biBitCount);
    DebugPrint("BMP: biCompression -    %lu\r\n", bitmapInfo.biCompression);
    DebugPrint("BMP: biSizeImage -      %lu\r\n", bitmapInfo.biSizeImage);
    DebugPrint("BMP: biXPelsPerMeter -  %lu\r\n", bitmapInfo.biXPelsPerMeter);
    DebugPrint("BMP: biYPelsPerMeter -  %lu\r\n", bitmapInfo.biYPelsPerMeter);
    DebugPrint("BMP: biClrUsed -        %lu\r\n", bitmapInfo.biClrUsed);
    DebugPrint("BMP: biClrImportant -   %lu\r\n", bitmapInfo.biClrImportant);
    DebugPrint("BMP: palletSize -       %lu\r\n", palletSize);
*/

    palatte = new ColorTable[palletSize];
    if(!palatte)
        {
        fclose(in);
        return 0;
        }

    image = new unsigned char[bitmapInfo.biSizeImage];
    if(!image)
        {
        delete[] palatte;
        palatte = NULL;
        fclose(in);
        return 0;
        }

    fread(palatte, sizeof(ColorTable), palletSize, in);
    fread(image, sizeof(unsigned char), bitmapInfo.biSizeImage, in);

    fclose(in);

    return 1;
    }


DWORD Bitmap::Draw(DWORD offsetX, DWORD offsetY, DWORD trans /*=0xFFFF*/)
    {
    if(!palatte || !image) return 0;

    for(DWORD size=0; size<bitmapInfo.biSizeImage; size++)
        {
        if(image[size] == trans) drLine((float)(size%bitmapInfo.biWidth+offsetX), (float)(bitmapInfo.biHeight-size/bitmapInfo.biWidth+offsetY), (float)(size%bitmapInfo.biWidth+1+offsetX), (float)(bitmapInfo.biHeight-size/bitmapInfo.biWidth+1+offsetY), 0x00000000);
        else drLine((float)(size%bitmapInfo.biWidth+offsetX), (float)(bitmapInfo.biHeight-size/bitmapInfo.biWidth+offsetY), (float)(size%bitmapInfo.biWidth+1+offsetX), (float)(bitmapInfo.biHeight-size/bitmapInfo.biWidth+1+offsetY), BUILDARGB(0xFF, palatte[image[size]].red, palatte[image[size]].green, palatte[image[size]].blue));
        }

    /*
    MYVERTEX *pb;
    float maxx, minx;
    float maxy, miny;

    gDevice->BeginScene();
    gVertexBuffer->Lock(0, 128, (BYTE**)&pb, 0);

    if(IsBadWritePtr(pb, 128))
        {
        DebugPrint("Lock returned %p, which is BAD!\n", pb);
        return 1;
        }
    
    for(DWORD size = 0; size<bitmapInfo.biSizeImage; size++)
        {
        pb[size].v.x = (float)(size%bitmapInfo.biWidth+offsetX);
        pb[size].v.y = (float)(bitmapInfo.biHeight-size/bitmapInfo.biWidth+offsetY);
        pb[size].v.z = 0.1f;
        pb[size].fRHW = 0.9f;
        pb[size].cDiffuse = BUILDARGB(0xFF, palatte[image[size]].red, palatte[image[size]].green, palatte[image[size]].blue);
        }

    gVertexBuffer->Unlock();

    gDevice->DrawPrimitive(D3DPT_POINTLIST, 0, size);
    gDevice->EndScene();
        */

    return 1;
    }


void drBitmap(float x, float y, DWORD dwWidth, DWORD dwHeight, DWORD *bitmap)
    {
    /*
    for(DWORD points = 0; points<dwWidth*dwHeight; points++)
        {
        drLine(x+dwWidth, y+dwHeight, x+dwWidth+1, y+dwHeight+1, bitmap[points]);
        }
    */

    MYVERTEX *pb;
    float maxx, minx;
    float maxy, miny;

    gDevice->BeginScene();
    gVertexBuffer->Lock(0, 128, (BYTE**)&pb, 0);

    if(IsBadWritePtr(pb, 128))
        {
        DebugPrint("Lock returned %p, which is BAD!\n", pb);
        return;
        }
    
    for(DWORD points = 0; points<dwWidth*dwHeight; points++)
        {
        pb[points].v.x = x+points%dwWidth;
        pb[points].v.y = y+points/dwWidth;
        pb[points].v.z = .1f;
        pb[points].fRHW = .9f;
        pb[points].cDiffuse = bitmap[points];
        }

    gVertexBuffer->Unlock();

    gDevice->DrawPrimitive(D3DPT_POINTLIST, 0, points);
    gDevice->EndScene();
    }


/*************************************************************************************
Function:   drInit
Purpose:    initialize drawing stuff
Return:     none
*************************************************************************************/
void drInit(int width, int height)
    {
    gScreenWidth = width;
    gScreenHeight = height;
    gBackBuffer = NULL;

    if(gDevice == NULL)
        {
        // Create D3D 8.
        IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
        if(pD3D == NULL) return;

        // Set the screen mode.
        D3DPRESENT_PARAMETERS d3dpp;
        ZeroMemory(&d3dpp, sizeof(d3dpp));

        // E:\XBOX\private\genx\directx\dxg\obj\i386\d3d8types.h
        d3dpp.BackBufferWidth           = width;
        d3dpp.BackBufferHeight          = height;
        d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
        d3dpp.BackBufferCount           = 1;
        d3dpp.Windowed                  = false;    // Must be false for Xbox
        d3dpp.EnableAutoDepthStencil    = true;
        d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
        d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
        d3dpp.FullScreen_RefreshRateInHz= 60;    //or 72?
        d3dpp.hDeviceWindow             = NULL;   // Ignored on Xbox

        // Create the gDevice.
        if(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &gDevice) != S_OK)
            {
            pD3D->Release();
            return;
            }

        // Now we no longer need the D3D interface so let's free it.
        pD3D->Release();
        }

    gDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &gBackBuffer);

    gDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    gDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    gDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    gDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    gDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    gDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    gDevice->CreateVertexBuffer(sizeof(MYVERTEX)*4, 0, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &gVertexBuffer);
    gDevice->SetStreamSource(0, gVertexBuffer, sizeof(MYVERTEX));

    XFONT_OpenBitmapFont(L"t:\\media\\cour.xft", 4096, &gFont);

    if(gFont == 0)
        {
        XFONT_OpenBitmapFont(L"t:\\media\\arialb16.xft", 4096, &gFont);
        //DebugPrint("Opening arial: %x\n", );
        }

    if(gFont)
        {
        XFONT_SetTextColor(gFont, 0xffFFFFFF);
        XFONT_SetBkColor(gFont, 0x00000000);
        }
    else
        {
        DebugPrint("gFont == 0! Font not loaded!\n");
        }
    }


/*************************************************************************************
Function:   drDeinit
Purpose:    deinitialize the drawing stuff
Return:     none
*************************************************************************************/
void drDeinit()
    {
    gBackBuffer->Release();
    gDevice->Release();
    }


void drClearRect(float x1, float y1, float x2, float y2, DWORD color) 
    {
    D3DRECT rect;

    if (gDevice != NULL)
        {
        if(x1 < x2)
            {
            rect.x1 = (long)x1; 
            rect.x2 = (long)x2 + 1;
            }
        else
            {
            rect.x1 = (long)x2; 
            rect.x2 = (long)x1 + 1;
            }

        if(y1 < y2)
            {
            rect.y1 = (long)y1;
            rect.y2 = (long)y2 + 1;
            }
        else
            {
            rect.y1 = (long)y2;
            rect.y2 = (long)y1 + 1;
            }

        gDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
        }

    }


/*************************************************************************************
Function:   drCls
Purpose:    clears the screen
Return:     none
*************************************************************************************/
void drCls()
    {
    drClearRect(0, 0, 639, 479, 0xFF000000);
    }



/*************************************************************************************
Function:   drBox
Purpose:    draw a rectangle
Params:     x1, y1, x2, y2 --- the coordinates. Box is drawn from (x1,y1) to (x2,y2)
            col --- the RGBA color to draw the box with
Return:     none
*************************************************************************************/
void drLine(float x1, float y1, float x2, float y2, DWORD color)
    {
    drClearRect(x1, y1, x2, y2, color);
    }


/*************************************************************************************
Function:   drQuad
Purpose:    draw a quadrilateral
Params:     x* --- x coordinate for point * 
            y* --- y coordinate for point *
            c* --- color at point *
Return:     none
*************************************************************************************/
void drQuad(float x1, float y1, DWORD c1,
            float x2, float y2, DWORD c2,
            float x3, float y3, DWORD c3,
            float x4, float y4, DWORD c4)
    {
    MYVERTEX * pb;
    float maxx, minx;
    float maxy, miny;

    gDevice->BeginScene();
    gVertexBuffer->Lock(0,128,(BYTE**)&pb,0);
    
    if(IsBadWritePtr(pb, 128))
        {
        DebugPrint("Lock returned %p, which is BAD!\n", pb);
        return;
        }


    pb[0].v.x = x1; pb[0].v.y = y1; pb[0].v.z = .1f; pb[0].fRHW = .9f; pb[0].cDiffuse = c1;
    pb[1].v.x = x2; pb[1].v.y = y2; pb[1].v.z = .1f; pb[1].fRHW = .9f; pb[1].cDiffuse = c2;
    pb[2].v.x = x3; pb[2].v.y = y3; pb[2].v.z = .1f; pb[2].fRHW = .9f; pb[2].cDiffuse = c3;
    pb[3].v.x = x4; pb[3].v.y = y4; pb[3].v.z = .1f; pb[3].fRHW = .9f; pb[3].cDiffuse = c4;

    gVertexBuffer->Unlock();

    gDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
    gDevice->EndScene();
    }


/*************************************************************************************
Function:   drBox
Purpose:    draw a rectangle
Params:     x1, y1, x2, y2 --- the coordinates. Box is drawn from (x1,y1) to (x2,y2)
            col --- the RGBA color to draw the box with
Return:     none
*************************************************************************************/
void drBox(float x1, float y1, float x2, float y2, DWORD color)
    {
    MYVERTEX * pb;
    float maxx, minx;
    float maxy, miny;

    gDevice->BeginScene();
    gVertexBuffer->Lock(0,128,(BYTE**)&pb,0);
    
    if(IsBadWritePtr(pb, 128))
        {
        DebugPrint("Lock returned %p, which is BAD!\n", pb);
        return;
        }

    if(x1 < x2)
        {
        minx = x1;
        maxx = x2;
        }
    else
        {
        minx = x2;
        maxx = x1;
        }

    if(x1 < x2)
        {
        miny = y1;
        maxy = y2;
        }
    else
        {
        miny = y2;
        maxy = y1;
        }

//    maxx++;
//    maxy++; 

    if(x1 == x2)
        {
        maxx++;
        }

    pb[0].v.x = minx; pb[0].v.y = miny; pb[0].v.z = .1f; pb[0].fRHW = .9f; pb[0].cDiffuse = color;
    pb[1].v.x = maxx; pb[1].v.y = miny; pb[1].v.z = .1f; pb[1].fRHW = .9f; pb[1].cDiffuse = color;
    pb[2].v.x = maxx; pb[2].v.y = maxy; pb[2].v.z = .1f; pb[2].fRHW = .9f; pb[2].cDiffuse = color;
    pb[3].v.x = minx; pb[3].v.y = maxy; pb[3].v.z = .1f; pb[3].fRHW = .9f; pb[3].cDiffuse = color;

    gVertexBuffer->Unlock();

    gDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
    gDevice->EndScene();
    }




/*************************************************************************************
Function:   drSetSize
Purpose:    set default font size and color for drPrintf
Params:     width, height --- the size of each letter of the font, in pixels
            col --- the RGBA color to draw text with
Return:     none
*************************************************************************************/
void drSetSize(float width, float height, DWORD col, DWORD bkcol)
    {
    gWidth = width; gHeight = height; 
    gColor = col; 
    gBkColor = bkcol;

    if(gFont) XFONT_SetTextColor(gFont, gColor);
    if(gFont) XFONT_SetBkColor(gFont, gBkColor);
    }


/*************************************************************************************
Function:   drGetSize
Purpose:    get the width, height, and color of the current default font
Params:     pwidth, pheight, pcolor --- pointers to memory to receive the data
            col --- the RGBA color to draw the box with
Return:     none
*************************************************************************************/
void drGetSize(OPTIONAL OUT float *pwidth, OPTIONAL OUT float *pheight, OPTIONAL OUT DWORD *pcolor, OPTIONAL OUT DWORD *pbkcolor)
    {
    if(pwidth)*pwidth=gWidth;
    if(pheight)*pheight=gHeight;
    if(pcolor)*pcolor=gColor;
    if(pcolor)*pbkcolor=gBkColor;
    }


/*************************************************************************************
Function:   drPrintf
Purpose:    prints formatted text, using the default font size and color
Params:     x1, y1 --- coordinates of the upper-left point to start the text
            format --- the WIDE_CHAR formatted string of text
            ... --- optional params for the formatted text
Return:     the number of chars printed
*************************************************************************************/
int drPrintf(float x1, float y1, TCHAR* format, ...)
    {
    int i;
    HRESULT h;
    va_list v;
    TCHAR wstr[256];

    va_start(v, format);
    i = wvsprintf(wstr, format, v);

    if(gFont) h = XFONT_TextOut(gFont, gBackBuffer, wstr, i, (long)x1, (long)y1);

    va_end(v);
    return i;
    }


/*************************************************************************************
Function:   drShowScreen
Purpose:    draw the screen buffer to the monitor
Params:     none
Return:     0, or error code
*************************************************************************************/
void drShowScreen()
    {
    #ifdef _UseDxConio_
        xDisplayConsole();
    #endif

    if (gDevice != NULL)
        {
        gDevice->Present(NULL, NULL, NULL, NULL);
        }
    }

} // namespace Launcher


