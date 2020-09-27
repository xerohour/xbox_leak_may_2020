/*++

Copyright (c) 2001-2002  Microsoft Corporation

Module Name:

    fatal.c

Abstract:

    This module implements the ExDisplayFatalError (aka UEM) and supporting routines.

--*/

#pragma code_seg("INIT")
#pragma data_seg("INIT_RW")
#pragma bss_seg("INIT_RW")
#pragma const_seg("INIT_RD")

#include "ntos.h"
#include "ani.h"

#include <xtl.h>
#include <xboxp.h>
#include <xconfig.h>
#include <smcdef.h>

#ifndef ARCADE

#pragma pack(1)

typedef struct _IMAGE_RUN1 {
    UINT fOne : 1;
    UINT Size : 3;
    UINT Intensity : 4;
} IMAGE_RUN1;

typedef struct _IMAGE_RUN2 {
    UINT fOne : 1;
    UINT fTwo : 1;
    UINT Size : 10;
    UINT Intensity : 4;
} IMAGE_RUN2;

typedef struct _IMAGE_RUN3 {
    UINT fOne : 1;
    UINT fTwo : 1;
    UINT Size : 18;
    UINT Intensity : 4;
} IMAGE_RUN3;

typedef struct _LED_VERT_TYPE {
    float x,y,z; 
    float w;
    ULONG color; 
} LED_VERT_TYPE;


#pragma pack()

#include "image.h"

const struct TheVerts { float x,y,z,w; float u, v; } Verts[] =
{
    {  0.0f,   0.0f, 0.5f, 1.0f,   0.0f,   0.0f},
    {640.0f, 480.0f, 0.5f, 1.0f, 320.0f, 240.0f},
    {  0.0f, 480.0f, 0.5f, 1.0f,   0.0f, 240.0f},
    {640.0f,   0.0f, 0.5f, 1.0f, 320.0f,   0.0f},
    {640.0f, 480.0f, 0.5f, 1.0f, 320.0f, 240.0f},
    {  0.0f,   0.0f, 0.5f, 1.0f,   0.0f,   0.0f},
};


//
// Macros to define LED line segments and characters
//

#define LEDSEGDEF(x1,y1,x2,y2)           ((x2<<6) | (y2<<4) | (x1<<2) | y1)
#define LEDCHARDEF(s0,s1,s2,s3,s4,s5,s6) ((s6 << 6) | (s5 << 5) | (s4 << 4) | (s3 << 3) | \
                                          (s2 << 2) | (s1 << 1) | s0)
                                          

//
// Define the line segments of the LED font
//
//   --0--
//   1   2
//   --3--
//   4   5
//   --6--
//

UCHAR LEDSegmentDef[] = {
    LEDSEGDEF(0,0, 1,0),
    LEDSEGDEF(0,0, 0,1),
    LEDSEGDEF(1,0, 1,1),
    LEDSEGDEF(0,1, 1,1),
    LEDSEGDEF(0,1, 0,2),
    LEDSEGDEF(1,1, 1,2),
    LEDSEGDEF(0,2, 1,2)
};

//
// Define the LED characters based on what line segments 
// should be turned on or off
//

UCHAR LEDCharDef[] = {
    LEDCHARDEF(1,1,1,0,1,1,1),    
    LEDCHARDEF(0,0,1,0,0,1,0),
    LEDCHARDEF(1,0,1,1,1,0,1),
    LEDCHARDEF(1,0,1,1,0,1,1),
    LEDCHARDEF(0,1,1,1,0,1,0),
    LEDCHARDEF(1,1,0,1,0,1,1),
    LEDCHARDEF(1,1,0,1,1,1,1),
    LEDCHARDEF(1,0,1,0,0,1,0),
    LEDCHARDEF(1,1,1,1,1,1,1),
    LEDCHARDEF(1,1,1,1,0,1,0)
};



IDirect3DDevice8* InitD3D(void)
{
    D3DPRESENT_PARAMETERS d3dpp;
    IDirect3DDevice8 *pDev;
    IDirect3D8 *pD3D = Direct3DCreate8(D3D_SDK_VERSION);

    if (pD3D == NULL)
    {
        return NULL;
    }

    pDev = NULL;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth                 = 640;
    d3dpp.BackBufferHeight                = 480;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = 1;
    d3dpp.Windowed                        = FALSE;
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_RefreshRateInHz      = 60;
    d3dpp.hDeviceWindow                   = NULL;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    IDirect3D8_CreateDevice(pD3D,
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            NULL,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &d3dpp,
            &pDev);

    IDirect3D8_Release(pD3D);

    return pDev;
}

BOOL InitVB(IDirect3DDevice8 *pDev)
{
    IDirect3DVertexBuffer8 *pVB;
    void *pVerts;

    if (FAILED(IDirect3DDevice8_CreateVertexBuffer(pDev,
            sizeof(Verts),
            D3DUSAGE_WRITEONLY,
            D3DFVF_XYZRHW | D3DFVF_TEX1,
            D3DPOOL_MANAGED,
            &pVB)))
    {
        return FALSE;
    }

    IDirect3DVertexBuffer8_Lock(pVB, 0, sizeof(Verts), (BYTE **)(&pVerts), 0);
    memcpy((void*)pVerts, (void*)Verts, sizeof(Verts));
    IDirect3DVertexBuffer8_Unlock(pVB);

    IDirect3DDevice8_SetStreamSource(pDev, 0, pVB, sizeof(Verts[0]));
    IDirect3DDevice8_SetVertexShader(pDev, D3DFVF_XYZRHW | D3DFVF_TEX1);

    return TRUE;
}

COLORREF ColorFromIntensity(UINT intensity4bits)
{
    UINT BaseR = 0x00;
    UINT BaseG = 0xFF;
    UINT BaseB = 0x00;

    UINT r = (UINT)((BaseR * intensity4bits) / 15.0f + 0.5f);
    UINT g = (UINT)((BaseG * intensity4bits) / 15.0f + 0.5f);
    UINT b = (UINT)((BaseB * intensity4bits) / 15.0f + 0.5f);

    return D3DCOLOR_ARGB(0xFF, r, g, b);
}

BOOL InitTexture(IDirect3DDevice8 *pDev)
{
    IDirect3DTexture8* pTex;
    D3DLOCKED_RECT lock;
    UINT x = 0, i, j;
    DWORD dwLine;
    DWORD dwAddr;
    IMAGE_RUN1 *pRun1;
    IMAGE_RUN2 *pRun2;
    IMAGE_RUN3 *pRun3;
    UINT Size;
    UINT Intensity;

    if (FAILED(IDirect3DDevice8_CreateTexture(pDev,
            320,
            240,
            1,
            0,
            D3DFMT_LIN_X8R8G8B8,
            0,
            &pTex)))
    {
        return FALSE;
    }

    IDirect3DTexture8_LockRect(pTex, 0, &lock, NULL, 0);
    dwLine = (DWORD)lock.pBits;
    dwAddr = dwLine;
    i = 0;

    while(i < sizeof(g_Image))
    {
        pRun1 = (IMAGE_RUN1*)&g_Image[i];
        if (pRun1->fOne)
        {
            Size = pRun1->Size;
            Intensity = pRun1->Intensity;
            i += 1;
        }
        else
        {
            pRun2 = (IMAGE_RUN2*)&g_Image[i];
            if (pRun2->fTwo)
            {
                Size = pRun2->Size;
                Intensity = pRun2->Intensity;
                i += 2;
            }
            else
            {
                pRun3 = (IMAGE_RUN3*)&g_Image[i];
                Size = pRun3->Size;
                Intensity = pRun3->Intensity;
                i += 3;
            }
        }

        for (j = 0; j < Size; j++)
        {
            if (Intensity == 0)
            {
                *(DWORD*)dwAddr = 0xFF000000;
            }
            else
            {
                *(DWORD*)dwAddr = ColorFromIntensity(Intensity);
            }

            x++;

            if (x >= 320)
            {
                x = 0;
                dwLine += lock.Pitch;
                dwAddr = dwLine;
            }
            else
            {
                dwAddr += sizeof(DWORD);
            }
        }
    }

    IDirect3DTexture8_UnlockRect(pTex, 0);

    IDirect3DDevice8_SetTexture(pDev, 0, (D3DBaseTexture*)pTex);

    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

    return TRUE;
}

#endif

VOID
ExDisplayFatalError(
    IN ULONG ErrorCode
    )
/*++

Routine Description:

    This routine displays a fatal error message (AKA UEM).

Arguments:

    ErrorCode - The error code to be displayed 

Return Value:

    None.

--*/
{
    IDirect3DDevice8 *pDev;
    ULONG Segment;
    ULONG VertexNumber;
    UCHAR SegDef;
    UCHAR CharDef;
    ULONG Digit;
    ULONG Position;
    ULONG Scratch;
    EEPROM_LAYOUT EEPROMData;
    NTSTATUS Status;
    ULONG SettingType;
    ULONG SettingLength;
    XBOX_UEM_INFO* UEMInfo;
    BOOL PowerCycle;

    RtlZeroMemory(XboxCERTKey, XBOX_KEY_LENGTH);

    AniSetLogo(NULL, 0);
    AniTerminateAnimation();

    //
    // Read the EEPROM to get the UEM info
    //

    UEMInfo = (XBOX_UEM_INFO*)&(EEPROMData.UEMInfo[0]);

    Status = ExQueryNonVolatileSetting(XC_MAX_ALL, &SettingType, &EEPROMData,
        sizeof(EEPROMData), &SettingLength);

    if (NT_SUCCESS(Status)) {

        PowerCycle = FALSE;

        //
        // If the last UEM code stored in EEPROM is zero, write the current UEM
        // code (manufacturing only), set the matching bit in the UEM history,
        // and power cycle (manufacturing only).  Otherwise, clear the last code
        // and display the error.
        //

        if (UEMInfo->LastCode == FATAL_ERROR_NONE) {

            if (XboxGameRegion & XC_GAME_REGION_MANUFACTURING) {

                UEMInfo->LastCode = (UCHAR)ErrorCode;
                PowerCycle = TRUE;
            }

            //
            // Update the history bitmap for errors 5 and above
            //
            
            if (ErrorCode >= 5) {
                UEMInfo->History |= (1 << (ErrorCode - 5));
            }

        } else {

            UEMInfo->LastCode = FATAL_ERROR_NONE;
        }

        //
        // Write the EEPROM back out
        //

        ExSaveNonVolatileSetting(XC_MAX_ALL, SettingType, &EEPROMData, SettingLength);

        //
        // If a power cycle is required, ask the SMC to perform the power cycle
        //

        if (PowerCycle) {
            Status = HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_RESET,
                SMC_RESET_ASSERT_POWERCYCLE);
        }
    }

#ifndef ARCADE
    pDev = InitD3D();

    if (pDev != NULL) {
        if (InitVB(pDev) && InitTexture(pDev)) {

            IDirect3DDevice8_BeginScene(pDev);

            //
            // Display the texture with the multilingual text
            //

            IDirect3DDevice8_DrawPrimitive(pDev, D3DPT_TRIANGLELIST, 0,
                (sizeof(Verts) / sizeof(Verts[0])) / 3);

            //
            // Setup the state to display the error code
            //

            IDirect3DDevice8_SetVertexShader(pDev, D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            IDirect3DDevice8_SetTextureStageState(pDev, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
            
            //
            // Display the two digit error code
            //

            for (Digit = 0; Digit < 2; Digit++) {
                
                Position = 80 - Digit * 25;
                CharDef = LEDCharDef[ErrorCode % 10];

                for (Segment = 0; Segment < 7; Segment++) {
                    
                    SegDef = LEDSegmentDef[Segment];
                     
                    if ((CharDef & (1 << Segment)) != 0) {
                        
                        //
                        // Draw the line segment 
                        //

                        IDirect3DDevice8_Begin(pDev, D3DPT_LINELIST);

                        for (VertexNumber = 0; VertexNumber < 2; VertexNumber++) {
                            
                            IDirect3DDevice8_SetVertexData4f(pDev, D3DVSDE_VERTEX,  
                                (float)(Position + ((SegDef >> (VertexNumber * 4 + 2)) & 0x3) * 12),
                                (float)(50 + ((SegDef >> (VertexNumber * 4)) & 0x3) * 16),
                                0.5f, 2.0f);
                        }

                        IDirect3DDevice8_End(pDev);
                    }
                }

                ErrorCode /= 10;
            }

            IDirect3DDevice8_EndScene(pDev);

            IDirect3DDevice8_Present(pDev, NULL, NULL, NULL, NULL);

            IDirect3DDevice8_PersistDisplay(pDev);
        }
    }
#endif

    //
    // Keep the system alive so that we can continue to handle the tray eject
    // interrupt.
    //

    PsTerminateSystemThread(STATUS_SUCCESS);
}
