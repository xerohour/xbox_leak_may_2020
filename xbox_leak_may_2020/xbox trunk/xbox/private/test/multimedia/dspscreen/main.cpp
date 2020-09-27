/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	main.cpp

Abstract:

	GP DSP tests

Author:

    georgioc

Environment:

	Xbox DVT only

Revision History:


--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <macros.h>
#include "dspscreen.h"
#include "bitfont.h"


//------------------------------------------------------------------------------
//	local functions
//------------------------------------------------------------------------------

void SaveEPRomToFile(WORD wCrc);
void EPRomCrcTest(BOOL bSaveRom);
void EPHangTest();



//------------------------------------------------------------------------------
//	Structures
//------------------------------------------------------------------------------
struct TVertex
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
    float u, v;         // Texture coordinates
};

static BitFont				g_font;
static TVertex				g_prText[4];
static IDirect3D8*		    g_d3d			= NULL;
static IDirect3DDevice8*	g_pDevice		= NULL;
static IDirect3DTexture8*	g_pd3dtText		= NULL;

//------------------------------------------------------------------------------
//	InitGraphics
//------------------------------------------------------------------------------
static HRESULT
InitGraphics(void)
/*++

Routine Description:

	Initializes the graphics

Arguments:

	None

Return Value:

	None

--*/
{
	HRESULT					hr;
	D3DPRESENT_PARAMETERS	d3dpp;
    D3DLOCKED_RECT			d3dlr;
	int						i;
	
	// Create an instance of a Direct3D8 object 
	g_d3d = Direct3DCreate8(D3D_SDK_VERSION);
	if(g_d3d == NULL)
		return E_FAIL;
	
	// Setup the present parameters
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.BackBufferWidth					= 640;
	d3dpp.BackBufferHeight					= 480;
	d3dpp.BackBufferFormat					= D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount					= 1;
	d3dpp.Flags								= 0;
	d3dpp.MultiSampleType					= D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect						= D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow						= NULL;
	d3dpp.Windowed							= FALSE;
	d3dpp.EnableAutoDepthStencil			= TRUE;
	d3dpp.AutoDepthStencilFormat			= D3DFMT_D24S8;
	d3dpp.Flags								= 0;
	d3dpp.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;
	d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the device
	hr = g_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, 
						     D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, 
						     &g_pDevice);
	if(FAILED(hr))
		return hr;

    // Create a buffer for the text
    hr = g_pDevice->CreateTexture(320, 240, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, 
									&g_pd3dtText);
    if(FAILED(hr))
        return hr;
	
    // Clear the texture
    g_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
    for (i = 0; i < 240; i++) {
        memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
    }
    g_pd3dtText->UnlockRect(0);

	// Text plane
    g_prText[0].x = 0.0f;
    g_prText[0].y = 480.0f;
    g_prText[0].u = 0.0f;
    g_prText[0].v = 240.0f;

    g_prText[1].x = 0.0f;
    g_prText[1].y = 0.0f;
    g_prText[1].u = 0.0f;
    g_prText[1].v = 0.0f;

    g_prText[2].x = 640.0f;
    g_prText[2].y = 0.0f;
    g_prText[2].u = 320.0f;
    g_prText[2].v = 0.0f;

    g_prText[3].x = 640.0f;
    g_prText[3].y = 480.0f;
    g_prText[3].u = 320.0f;
    g_prText[3].v = 240.0f;

    for (i = 0; i < 4; i++) {
        g_prText[i].z = 0.0f;
        g_prText[i].rhw = 1.0f;
    }

	return S_OK;
}


//------------------------------------------------------------------------------
//	main
//------------------------------------------------------------------------------
void 
__cdecl 
main(void)
/*++

Routine Description:

    the exe entry point

Arguments:

    None

Return Value:

    None

--*/
{
    BOOL bPass;
    XDEVICE_PREALLOC_TYPE	devType = { XDEVICE_TYPE_MEMORY_UNIT, 4 };

    InitGraphics();
    XInitDevices(1, &devType);

    //
    // indicate test has started
    //


	g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	g_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	g_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	g_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	g_pDevice->SetTexture(0, g_pd3dtText);
	g_pDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);

    g_pDevice->Clear(0, NULL, 
                     D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
                     0xff0000FF, 1.0f, 0);

    EPRomCrcTest(TRUE);

    Sleep(2000);

    EPHangTest();


    while(1);
}

void EPHangTest()
{
    BOOL bPass;
    IDirect3DSurface8*		pd3ds;
    D3DLOCKED_RECT			d3dlr;
    DWORD i=0;

    g_pDevice->Clear(0, NULL, 
                     D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
                     0xff0000FF, 1.0f, 0);

    g_pDevice->EndScene();
    g_pDevice->Present(NULL, NULL, NULL, NULL);

    bPass = XTestAudioDspScreening(NULL,(DOUBLE)5.0);
    if (!bPass) {    

        g_pDevice->Clear(0, NULL, 
                         D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
                         0xffFF0000, 1.0f, 0);

		// Clear the texture
		g_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
		for(i = 0; i < 240; i++) 
			memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
		g_pd3dtText->UnlockRect(0);

	    g_pd3dtText->GetSurfaceLevel(0, &pd3ds);
		g_font.DrawText(pd3ds, L"Audio Test failed due to EP hang detected", 25, 25, 0, 0xffffffff, 0);
		pd3ds->Release();
		g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, g_prText, sizeof(TVertex));


    } else {

        g_pDevice->BeginScene();
        g_pDevice->Clear(0, NULL, 
                         D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
                         0xff00FF00, 1.0f, 0);

        // Clear the texture
        g_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
        for(i = 0; i < 240; i++) 
            memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
        g_pd3dtText->UnlockRect(0);

        g_pd3dtText->GetSurfaceLevel(0, &pd3ds);
        g_font.DrawText(pd3ds, L"Audio Test Passed.No hangs detected", 25, 25, 0, 0xffffffff, 0);
        pd3ds->Release();
        g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, g_prText, sizeof(TVertex));


    }

    g_pDevice->EndScene();
    g_pDevice->Present(NULL, NULL, NULL, NULL);

}

void EPRomCrcTest(BOOL bSaveROM)
{

    BOOL bPass;
    IDirect3DSurface8*		pd3ds;
    D3DLOCKED_RECT			d3dlr;
    DWORD i=0;
    WORD wCrc = 0, wXramCrc = 0, wCopyXramCrc = 0;
    WCHAR buffer[256];

    memset(buffer,0,sizeof(buffer));

    bPass = XTestAudioDspScreeningCRC(&wCrc,&wXramCrc,&wCopyXramCrc);

    //
    // get the contents of the EP xram (that contains the ROM image) onto a file
    // in an MU
    //

    if (bSaveROM) {

        SaveEPRomToFile(wCrc);

    }

    if (!bPass) {    
        
        //
        // failed the ROM crc test
        //

        g_pDevice->Clear(0, NULL, 
                         D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
                         0xffFF0000, 1.0f, 0);

        switch (wCrc) {
        case 0:

			// Clear the texture
			g_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
			for(i = 0; i < 240; i++) 
				memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
			g_pd3dtText->UnlockRect(0);

		    g_pd3dtText->GetSurfaceLevel(0, &pd3ds);
			g_font.DrawText(pd3ds, L"Audio Test failed due to EP dma mismatch", 25, 25, 0, 0xffffffff, 0);
			pd3ds->Release();
			g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, g_prText, sizeof(TVertex));


            break;
        default:


			// Clear the texture
			g_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
			for(i = 0; i < 240; i++) 
				memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
			g_pd3dtText->UnlockRect(0);

		    g_pd3dtText->GetSurfaceLevel(0, &pd3ds);

            swprintf(buffer,L"Audio Test failed.");
			g_font.DrawText(pd3ds, buffer, 25, 25, 0, 0xffffffff, 0);

             
            swprintf(buffer,L"Expected crc(0x%x), wCrc(0x%x)",
                     EP_ROM_CRC,
                     wCrc);

            g_font.DrawText(pd3ds, buffer, 25, 45, 0, 0xffffffff, 0);

            swprintf(buffer,L"wXramCrc(0x%x),wCopyXramCrc(0x%x)",
                     wXramCrc,
                     wCopyXramCrc);

            g_font.DrawText(pd3ds, buffer, 25, 65, 0, 0xffffffff, 0);

			pd3ds->Release();
			g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, g_prText, sizeof(TVertex));

            break;
        }



    } else {

        g_pDevice->BeginScene();
        g_pDevice->Clear(0, NULL, 
                         D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
                         0xff00FF00, 1.0f, 0);

        // Clear the texture
        g_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
        for(i = 0; i < 240; i++) 
            memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
        g_pd3dtText->UnlockRect(0);

        g_pd3dtText->GetSurfaceLevel(0, &pd3ds);
        g_font.DrawText(pd3ds, L"Audio Test Passed.EP rom crc and dma results are good", 25, 25, 0, 0xffffffff, 0);
        pd3ds->Release();
        g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, g_prText, sizeof(TVertex));


    }

    g_pDevice->EndScene();
    g_pDevice->Present(NULL, NULL, NULL, NULL);

}

void SaveEPRomToFile(WORD wCrc)
{

    CHAR chDrive;
    PDWORD pDspXMem = (PDWORD) (0xfe851000);
    DWORD dwValue = 0, i = 0;
    CHAR buffer[256];
    CHAR number[32];
    PCHAR fileBuffer;

    fileBuffer = new CHAR[EP_ROM_SIZE*sizeof(DWORD)*16];
    if (fileBuffer == NULL) {
        return;
    }

    memset(number,0,sizeof(number));
    memset(buffer,0,sizeof(buffer));
    memset(fileBuffer,0,EP_ROM_SIZE*sizeof(DWORD));

    //
    // wait for MU to get plugged in
    //

    while (TRUE) {
        dwValue = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);
        if (dwValue & XDEVICE_PORT0_TOP_MASK) {
            break;
        }
    }
    
    dwValue = XMountMU(XDEVICE_PORT0,XDEVICE_TOP_SLOT,&chDrive);
    if (dwValue != ERROR_SUCCESS) {
        return;
    }

    HANDLE hFile;
    sprintf(buffer,"%c:\\rom%x.txt",chDrive,wCrc);
    hFile = CreateFile((LPCSTR)buffer,
                       GENERIC_ALL,
                       0,
                       NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        return;
    }

    //
    // now write in a column each word of the DSP rom contents(currently in xram)
    //

    DWORD dwCount = 0;

    for (i=0;i<EP_ROM_SIZE;i++) {

        memset(number,0,sizeof(number));
        memset(buffer,0,sizeof(buffer));
        
        _itoa(*pDspXMem++,number,16);
        sprintf(buffer,"\n0x00%s,",number);
        memcpy((PCHAR)(fileBuffer+dwCount),buffer,strlen(buffer));
        dwCount += strlen(buffer);

    }

    if (!WriteFile(hFile,
              fileBuffer,
              dwCount,
              &dwValue,
              NULL)) {

        delete [] fileBuffer;
        CloseHandle(hFile);
        return;
    }

    delete [] fileBuffer;
    CloseHandle(hFile);

    XUnmountMU(XDEVICE_PORT0,XDEVICE_TOP_SLOT);
}

