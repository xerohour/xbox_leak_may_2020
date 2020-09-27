/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    draw.cpp

Abstract:

    simplified functions that call Matt's dxconio functions... Used by CPX

Author:

    Jason Gould (a-jasgou) May 2000

--*/

#include "xtl.h"
#include "draw.h"
#include "Xfont.h"
//#include "bitfont.h"

extern "C" void DebugPrint(char*,...);

//#ifdef DEBUG
#define CHECK(code) do { int _ret__; _ret__ = code; if(_ret__ != D3D_OK) DebugPrint(#code "File %s Line %d, Failed: %d\n", __LINE__, __FILE__, _ret__); } while (0);
//#else
//#define CHECK(code) code;
//#endif

typedef struct MYVERTEX {
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

/*************************************************************************************
Function:   drInit
Purpose:	initialize drawing stuff
Return:     none
*************************************************************************************/
void drInit(int width, int height)
{
	gScreenWidth = width;
	gScreenHeight = height;

	HWND hwnd = 0;
	gBackBuffer = NULL;

	if (gDevice == NULL)
	{
		// Create D3D 8.

		IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);
		if (pD3D == NULL)
			return;

		// Set the screen mode.

		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));

		d3dpp.BackBufferWidth           = width;
		d3dpp.BackBufferHeight          = height;
		d3dpp.BackBufferFormat          = D3DFMT_X8R8G8B8;
		d3dpp.BackBufferCount           = 1;
		d3dpp.Windowed                  = false;    // Must be false for Xbox
		d3dpp.EnableAutoDepthStencil    = true;
        d3dpp.AutoDepthStencilFormat    = D3DFMT_D24S8;
		d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
		d3dpp.FullScreen_RefreshRateInHz= 60;	//or 72?
		d3dpp.hDeviceWindow             = hwnd;   // Ignored on Xbox

		// Create the gDevice.

		if (pD3D->CreateDevice
		(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hwnd,                                 // Ignored on Xbox
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp,
			&gDevice
		) != S_OK)
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

	gDevice->CreateVertexBuffer(sizeof(MYVERTEX)*4, 0, D3DFVF_XYZRHW | D3DFVF_DIFFUSE,
		D3DPOOL_DEFAULT, &gVertexBuffer);

	gDevice->SetStreamSource(0, gVertexBuffer, sizeof(MYVERTEX));

	
	
	DebugPrint("opening cour: %x\n", XFONT_OpenBitmapFont(L"t:\\media\\cour.xft", 4096, &gFont));

	// try D:
	if(gFont == 0)
	{
		DebugPrint("opening cour: %x\n", XFONT_OpenBitmapFont(L"d:\\media\\cour.xft", 4096, &gFont));
	}

	if(gFont == 0) {
		DebugPrint("Opening arial: %x\n", XFONT_OpenBitmapFont(L"t:\\media\\arialb16.xft", 4096, &gFont));
	}
	
	// again, try D:
	if(gFont == 0) {
		DebugPrint("Opening arial: %x\n", XFONT_OpenBitmapFont(L"d:\\media\\arialb16.xft", 4096, &gFont));
	}


	if(gFont == 0) {
		DebugPrint("gFont == 0! Font not loaded!\n");
	}


	if(gFont) XFONT_SetTextColor(gFont, 0xff00ff00);
	if(gFont) XFONT_SetBkColor(gFont, 0x00000000);
}

/*************************************************************************************
Function:   drDeinit
Purpose:	deinitialize the drawing stuff
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
		if(x1 < x2) {
			rect.x1 = (long)x1; 
			rect.x2 = (long)x2 + 1;
		} else {
			rect.x1 = (long)x2; 
			rect.x2 = (long)x1 + 1;
		}

		if(y1 < y2) {
			rect.y1 = (long)y1;
			rect.y2 = (long)y2 + 1;
		} else {
			rect.y1 = (long)y2;
			rect.y2 = (long)y1 + 1;
		}

		gDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
	}

}

/*************************************************************************************
Function:   drCls
Purpose:	clears the screen
Return:     none
*************************************************************************************/
void drCls()
{
	D3DRECT rect;
	rect.x1 = rect.y1 = 0;
	rect.x2 = rect.y2 = 2000;

	gDevice->Clear(1, &rect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
}



/*************************************************************************************
Function:   drBox
Purpose:	draw a rectangle
Params:     x1, y1, x2, y2 --- the coordinates. Box is drawn from (x1,y1) to (x2,y2)
            col --- the RGBA color to draw the box with
Return:     none
*************************************************************************************/
void drLine(float x1, float y1, float x2, float y2, DWORD color)
{
/**/

	drClearRect(x1, y1, x2, y2, color);

/*/

  
	MYVERTEX * pb;
	
	CHECK(gDevice->SetRenderState(D3DRS_LASTPIXEL, FALSE));

	CHECK(gDevice->BeginScene());
	CHECK(gVertexBuffer->Lock(0,128,(BYTE**)&pb,0));
	
	if(IsBadWritePtr(pb, 128))
	{
		DebugPrint("Lock returned %p, which is BAD!\n", pb);
		return;
	}

	pb[0].v.x = x1  ; pb[0].v.y = y1  ; pb[0].v.z = .1f; pb[0].fRHW = .9f; pb[0].cDiffuse = color;
	pb[1].v.x = x1  ; pb[1].v.y = y1  ; pb[1].v.z = .1f; pb[1].fRHW = .9f; pb[1].cDiffuse = color;
	pb[2].v.x = x2  ; pb[2].v.y = y2  ; pb[2].v.z = .1f; pb[2].fRHW = .9f; pb[2].cDiffuse = color;

	CHECK(gVertexBuffer->Unlock());

//	m.Ambient.r = m.Diffuse.r = m.Emissive.r = m.Specular.r = (float)((color      ) & 255) / 255;
//	m.Ambient.g = m.Diffuse.g = m.Emissive.g = m.Specular.g = (float)((color >>  8) & 255) / 255;
//	m.Ambient.b = m.Diffuse.b = m.Emissive.b = m.Specular.b = (float)((color >> 16) & 255) / 255;
//	m.Ambient.a = m.Diffuse.a = m.Emissive.a = m.Specular.a = (float)((color >> 24)      ) / 255;
//	
//	gDevice->SetMaterial(&m);



	CHECK(gDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1));
	CHECK(gDevice->EndScene());

	CHECK(gDevice->SetRenderState(D3DRS_LASTPIXEL, TRUE));

/**/

}


/*************************************************************************************
Function:   drQuad
Purpose:	draw a quadrilateral
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

	CHECK(gDevice->BeginScene());
	CHECK(gVertexBuffer->Lock(0,128,(BYTE**)&pb,0));
	
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
Purpose:	draw a rectangle
Params:     x1, y1, x2, y2 --- the coordinates. Box is drawn from (x1,y1) to (x2,y2)
            col --- the RGBA color to draw the box with
Return:     none
*************************************************************************************/
void drBox(float x1, float y1, float x2, float y2, DWORD color)
{
	MYVERTEX * pb;
	float maxx, minx;
	float maxy, miny;

	CHECK(gDevice->BeginScene());
	CHECK(gVertexBuffer->Lock(0,128,(BYTE**)&pb,0));
	
	if(IsBadWritePtr(pb, 128))
	{
		DebugPrint("Lock returned %p, which is BAD!\n", pb);
		return;
	}

	if(x1 < x2) {
		minx = x1;
		maxx = x2;
	} else {
		minx = x2;
		maxx = x1;
	}

	if(x1 < x2) {
		miny = y1;
		maxy = y2;
	} else {
		miny = y2;
		maxy = y1;
	}

//	maxx++;
//	maxy++; 

	if(x1 == x2) {
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
Purpose:	set default font size and color for drPrintf
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
Purpose:	get the width, height, and color of the current default font
Params:     pwidth, pheight, pcolor --- pointers to memory to receive the data
            col --- the RGBA color to draw the box with
Return:     none
*************************************************************************************/
void drGetSize(
    OPTIONAL OUT float*pwidth, 
	OPTIONAL OUT float*pheight, 
	OPTIONAL OUT DWORD*pcolor,
	OPTIONAL OUT DWORD*pbkcolor
	)
{
	if(pwidth)*pwidth=gWidth;
	if(pheight)*pheight=gHeight;
	if(pcolor)*pcolor=gColor;
	if(pcolor)*pbkcolor=gBkColor;
}

/*************************************************************************************
Function:   drGetStrSize
Purpose:	get the width, in pixels, of a string
Params:     str --- pointer to the unicode string to measure
			len --- the number of chars in that string, or -1, if null-terminated
Return:     length in pixels of the string
*************************************************************************************/
int drGetStrSize(
	IN LPCWSTR str, 
	IN int len
	) 
{
	unsigned outlen;
	XFONT_GetTextExtent(gFont, str, len, &outlen);
	return outlen;
}

/*************************************************************************************
Function:   drPrintf
Purpose:	prints formatted text, using the default font size and color
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
	if(gFont) if(h != 0) DebugPrint("XFONT_TextOut: %x\n", h);

	va_end(v);
	return i;
}

/*************************************************************************************
Function:   drShowScreen
Purpose:	draw the screen buffer to the monitor
Params:     none
Return:     0, or error code
*************************************************************************************/
void drShowScreen()
{
						#ifdef _UseDxConio_
							xDisplayConsole();
                        #endif

	if (gDevice != NULL) {
		gDevice->Present(NULL, NULL, NULL, NULL);
	}

}

/*************************************************************************************
Function:   drBitBlt
Purpose:	draw the inputed bit array to the screen
Params:     fx - the upper left x coordinate on screen
            fy - the upper left y coordinate on screen
			ulWidth - the width of the bitmap
			ulHeight - the height of the bitmap
			pData - a pointer to the array of pixel values
Return:     none
*************************************************************************************/

void drBitBlt( float fx, float fy, ULONG ulWidth, ULONG ulHeight, const BYTE* pData )
{
	for ( ULONG y = 0; y < ulHeight; y++ )
	{
		for ( ULONG x = 0; x < ulWidth; x++ )
		{
			if ( 0 != pData[y * ulWidth + x] )
			{
				drLine( fx + x, fy + y, fx + x, fy + y, (DWORD) pData[y * ulWidth + x] );
			}
		}
	}

}

DWORD drScaledFloatToRgba(float r, float g, float b, float a)
{
	return ((BYTE)(r * 255) + (((BYTE)(g * 255)) << 8) + (((BYTE)(b * 255)) << 16) + (((BYTE)(a * 255)) << 24));
}

DWORD drUnscaledFloatToRgba(float r, float g, float b, float a) 
{
	return ((BYTE)r + ((BYTE)g << 8) + ((BYTE)b << 16) + ((BYTE)a << 24));
}

DWORD drHslToRgb(float h, float s, float l, float al)
{
	float r, g, b, a = al;

	if (h > 360)	//bug, h could be > 720... I don't really care. % doesn't seem to work here.
		h += 360;
	if (h < 0)
		h += 360;

	if((h >= 0) && (h < 60)) {
		r = l / 100;
		b = r - (r * s / 100);
		g = b + (r - b)*(h - 0)/60;
	} 
	else if((h >= 60) && (h < 120)) {
		g = l / 100;
		b = g - (g * s / 100);
		r = b + (g - b) * (120 - h) / 60;
	} 
	else if((h >=120) && (h <180)) {
		g = l / 100;
		r = g - (g * s / 100);
		b = r + (g - r)*(h-120)/60;
	} 
	else if((h >=180) && (h < 240)) {
		b = l / 100;
		r = b - (b * s / 100);
		g = r + (b - r) * (240 - h) / 60;
	} 
	else if((h >= 240) && (h < 300)) {
		b = l / 100;
		g = b - (b * s / 100);
		r = g + (b - g)*(h-240)/60;
	} 
	else if((h >= 300) && (h < 360)) {
		r = l / 100;
		g = r - (r * s / 100);
		b = g + (r - g) * (360 - h) / 60;
	}
	return ::drScaledFloatToRgba(r,g,b,a);
}

