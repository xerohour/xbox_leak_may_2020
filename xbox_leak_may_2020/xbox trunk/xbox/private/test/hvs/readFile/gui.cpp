/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    gui.cpp

Abstract:

    File System disk scanner.

Notes:

*****************************************************************************/
#include "readFile.h"

IDirect3D8*		    g_d3d		= NULL;
IDirect3DDevice8*	g_pDevice	= NULL;
BitFont				g_font;
IDirect3DTexture8*	g_pd3dtText	= NULL;
TVertex				g_prText[4];
D3DLOCKED_RECT		g_d3dlr;

#define STICK_DEAD_ZONE         6400
#define BUTTON_PRESS_VALUE      100
#define BUTTON_SPEEP_VALUE      150         // in ms

#define COLUMN_1_OFFSET         25
#define COLUMN_2_OFFSET         165

void GUI(void)
    {
    bool exit = false;
    XINPUT_STATE state;

    while(!exit)
        {
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
        XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addDevice, &removeDevice);
        InputMUInsertions(addDevice, removeDevice);

        for(unsigned port=0; port<XGetPortCount(); port++)
            {
            if(hDuke[port])
                {
                XInputGetState(hDuke[port], &state);
                if(state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) exit = true;
                }
            }

        UpdateGraphics(DISPLAYMODE_CONFIG_READONLY);

        Sleep(BUTTON_SPEEP_VALUE);
        }

    // sleep past any residual user input presses
    Sleep(BUTTON_SPEEP_VALUE);
    }

void GraphicPrint(IDirect3DSurface8* pd3ds, unsigned x, unsigned line, DWORD color, char* format, ...)
    {
    char szBuffer[256];

    va_list args;
    va_start(args, format);

    vsprintf(szBuffer, format, args);

    WCHAR *ptr = Ansi2UnicodeHack(szBuffer);

    g_font.DrawText(pd3ds, ptr, x, (line+1)*10, 0, color, 0);
    }


void UpdateGraphics(unsigned mode /*=DISPLAYMODE_RUN_TIME*/)
    {
    IDirect3DSurface8*	pd3ds;

    // Clear the device
    g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0);

    // Clear the texture
    g_pd3dtText->LockRect(0, &g_d3dlr, NULL, 0);
    for(unsigned i=0; i<240; i++)
        memset((LPBYTE)g_d3dlr.pBits + i * g_d3dlr.Pitch, 0, 320 * 4);
    g_pd3dtText->UnlockRect(0);

    // Get the surface to draw the text to
    g_pd3dtText->GetSurfaceLevel(0, &pd3ds);

    //
    // run time status mode
    //
    if(mode == DISPLAYMODE_RUN_TIME || mode == DISPLAYMODE_PAUSE)
        {
	    GraphicPrint(pd3ds, COLUMN_1_OFFSET, 1, 0xFF4444FF, "FILE READ");
	    GraphicPrint(pd3ds, COLUMN_1_OFFSET, 2, 0xFF4444FF, "Model: %s", model);
	    GraphicPrint(pd3ds, COLUMN_1_OFFSET, 3, 0xFF4444FF, "Serial: %s", serial);
	    GraphicPrint(pd3ds, COLUMN_1_OFFSET, 4, 0xFF4444FF, "Firmware: %s", firmware);
        
	    GraphicPrint(pd3ds, COLUMN_1_OFFSET, 5, 0xFF4444FF, "Media: %s", mediaTypeNames[mediaType]);

        unsigned line = 6;
        GraphicPrint(pd3ds, COLUMN_1_OFFSET, line, 0xFFFFFFFF, "Files Read: %I64u (%I64u on disk)", numFilesRead, numFiles);
        GraphicPrint(pd3ds, COLUMN_2_OFFSET, line, 0xFFFFFFFF, " Run Time: %u sec", (GetTickCount() - time)/1000);

        GraphicPrint(pd3ds, COLUMN_1_OFFSET, ++line, 0xFFFFFFFF, "Current File: %s", findData.cFileName);
        if(findData.nFileSizeLow >= 30000)
            GraphicPrint(pd3ds, COLUMN_2_OFFSET, line, 0xFFFFFFFF, " (%.02lf %%)", percentDone);

        if(globalSettings.stopAfter.duration != 0 && globalSettings.stopAfter.type == STOPAFTER_ITERATIONS)
            GraphicPrint(pd3ds, COLUMN_1_OFFSET,  ++line, 0xFFFFFFFF, "Pass %u of %u", numTimesThrough, globalSettings.stopAfter.duration);
        else
            GraphicPrint(pd3ds, COLUMN_1_OFFSET,  ++line, 0xFFFFFFFF, "Pass %u", numTimesThrough);


        GraphicPrint(pd3ds, COLUMN_1_OFFSET, ++line, 0xFFFFFFFF, "Buffer Mismatches: %u", numBuffMisMatch);
        GraphicPrint(pd3ds, COLUMN_2_OFFSET,   line, 0xFFFFFFFF, " Failures: %u", numFail);


        if(mode == DISPLAYMODE_RUN_TIME)
            {
	        GraphicPrint(pd3ds, COLUMN_1_OFFSET,  19, 0xFFFFFFFF,  "Press START to view the current settings");
	        GraphicPrint(pd3ds, COLUMN_1_OFFSET,  20, 0xFFFFFFFF,  "Press Left Trigger + Right Trigger + Black to reboot");
            }
        else if(mode == DISPLAYMODE_PAUSE)
            {
	        GraphicPrint(pd3ds, COLUMN_1_OFFSET,  19, 0xFFFFFF00,  "PAUSED");
	        GraphicPrint(pd3ds, COLUMN_1_OFFSET,  20, 0xFFFFFF00,  "Press BACK to continue");
            }
        }

    //
    // config setup gui
    //
    else if(mode == DISPLAYMODE_CONFIG_READONLY)
        {
	    GraphicPrint(pd3ds, COLUMN_1_OFFSET, 1, 0xFF777777,  "Drive: %s", globalSettings.drive);
	    GraphicPrint(pd3ds, COLUMN_1_OFFSET, 2, 0xFF777777,  "Read Size: %u bytes", globalSettings.readSize);
	    GraphicPrint(pd3ds, COLUMN_2_OFFSET, 1, 0xFF777777,  " Number of Reads: %u", globalSettings.numberOfReads);
	    GraphicPrint(pd3ds, COLUMN_2_OFFSET, 2, 0xFF777777,  " Compare Buffers: %u", globalSettings.compareBuffers);

	    GraphicPrint(pd3ds, COLUMN_1_OFFSET,  20, 0xFFFFFFFF,  "Press BACK to continue");
        }

    // Release the surface
    pd3ds->Release();

    // Draw the textured primitive (with the text)
    g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, g_prText, sizeof(TVertex));

    // End the scene and present it
    g_pDevice->EndScene();
    g_pDevice->Present(NULL, NULL, NULL, NULL);
    }



bool InputCheckExitCombo(void)
    {
    XINPUT_STATE state;

    for(unsigned port=0; port<XGetPortCount(); port++)
        {
        if(hDuke[port])
            {
            XInputGetState(hDuke[port], &state);
            if(state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > BUTTON_PRESS_VALUE &&
                state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > BUTTON_PRESS_VALUE &&
                state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > BUTTON_PRESS_VALUE)
                {
                LogPrint("\r\n\r\nFound Exit Combo on gamepad %u - rebooting\r\n", port+1);
                return true;
                }
            }
        }

    // check for exit file
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile("T:\\testexit.txt", &FindFileData);
    FindClose(hFind);
    if(hFind != INVALID_HANDLE_VALUE)
        {
        LogPrint("\r\n\r\nFound Exit File (testexit.txt) - rebooting\r\n");
        return true;
        }

    return false;
    }


bool InputCheckButton(int button)
    {
    XINPUT_STATE state;

    for(unsigned port=0; port<XGetPortCount(); port++)
        {
        if(hDuke[port])
            {
            XInputGetState(hDuke[port], &state);
            if(state.Gamepad.bAnalogButtons[button] > BUTTON_PRESS_VALUE)
                {
                return true;
                }
            }
        }

    return false;
    }


bool InputCheckDigitalButton(int button)
    {
    XINPUT_STATE state;

    for(unsigned port=0; port<XGetPortCount(); port++)
        {
        if(hDuke[port])
            {
            XInputGetState(hDuke[port], &state);
            if(state.Gamepad.wButtons & button)
                {
                return true;
                }
            }
        }

    return false;
    }


void Pause(void)
    {
    OutputDebugStringA("*****************************************************************************\n");
    OutputDebugStringA("* Entered Paused State. Hit the BACK button to resume...\n");
    OutputDebugStringA("*****************************************************************************\n");
    while(!InputCheckDigitalButton(XINPUT_GAMEPAD_BACK))
        {
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
        XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addDevice, &removeDevice);
        InputMUInsertions(addDevice, removeDevice);

        UpdateGraphics(DISPLAYMODE_PAUSE);

        Sleep(BUTTON_SPEEP_VALUE);
        }
    }


void InputDukeInsertions(DWORD add, DWORD remove/*=0*/)
    {
    for(unsigned port=0; port<XGetPortCount(); port++)
        {
        if(remove & USBPortMasks[port])
            {
            if(hDuke[port]) XInputClose(hDuke[port]);
            hDuke[port] = NULL;
            }
        if(add & USBPortMasks[port])
            {
            hDuke[port] = XInputOpen(XDEVICE_TYPE_GAMEPAD, port, 0, NULL);
            //LogPrint("Adding Duke in Port %u\n", port+1);
            }
        }
    }


void InputMUInsertions(DWORD add, DWORD remove/*=0*/)
    {
    for(unsigned i=0; i<XGetPortCount()*2; i++)
        {
        DWORD port = i/2;
        DWORD slot = i%2==0?XDEVICE_BOTTOM_SLOT:XDEVICE_TOP_SLOT;
        if(remove & USBSlotMasks[i])
            {
            if(hSlot[i]) XUnmountMU(XMUPortFromDriveLetter((char)hSlot[i]), XMUSlotFromDriveLetter((char)hSlot[i]));
            hSlot[i] = NULL;
            }
        if(add & USBSlotMasks[i])
            {
            char drive;
            if(XMountMU(port, slot, &drive) == ERROR_SUCCESS)
                {
                hSlot[i] = (HANDLE)drive;
                //LogPrint("Adding MU in Port %u, Slot %u\n", XMUPortFromDriveLetter((char)hSlot[i])+1, XMUSlotFromDriveLetter((char)hSlot[i])+1);
                }
            }
        }
    }


/*

Routine Description:

	Initializes the graphics

Arguments:

	None

Return Value:

	None

*/
HRESULT InitGraphics(void)
    {
    HRESULT	hr;
    D3DPRESENT_PARAMETERS d3dpp;
    int i;
    
    // Create an instance of a Direct3D8 object 
    g_d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if(g_d3d == NULL)
        return E_FAIL;
    
    // Setup the present parameters: 640x480x24
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
    hr = g_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pDevice);
    if(FAILED(hr))
        return hr;
    
    // Create a buffer for the text
    hr = g_pDevice->CreateTexture(320, 240, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &g_pd3dtText);
    if(FAILED(hr))
        return hr;
    
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
    
    for(i=0; i<4; i++) 
        {
        g_prText[i].z	= 0.0f;
        g_prText[i].rhw	= 1.0f;
        }
    
    // Setup stuff
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
    
    return S_OK;
    }
