/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    memprint.h

Abstract:

    Memory Foot Print

Notes:

*****************************************************************************/

#ifndef _MEMPRINT_H_
#define _MEMPRINT_H_

#ifdef _VC
//#define DEVICE_TYPE ULONG
#define NTLEANANDMEAN
#define _X86_
#define _WIN32_WINNT 0x0500
#endif

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntexapi.h>
#include <scsi.h>
#include <ntddscsi.h>
#include <ntdddisk.h>
//#include <ntddcdrm.h>
#include <heap.h>
extern "C"
    {
    #include <ntos.h>
    #include <xbeimage.h>
    }


#include <xtl.h>
#include <stdio.h>

#include "bitfont.h"

//#define PAGE_SIZE 4096

//
// utility functions
//
void DebugPrint(char* format, ...);



//
// Initialization calls
//
void Initialize(void);
HRESULT InitGraphics(void);

//
// User input
// 
void InputDukeInsertions(DWORD add, DWORD remove=0);
void InputMUInsertions(DWORD add, DWORD remove=0);
bool InputCheckExitCombo(void);
bool InputCheckButton(int button);
bool InputCheckDigitalButton(int button);
void Pause(void);
int ThumbStickScale(int stick, int max);
void AdjustValue(DWORD &value, DWORD min, DWORD max, XINPUT_STATE &input, DWORD maxThumbStickScale);


//
// Graphics / GUI methods
//
enum DISPLAY_MODE
    {
    DISPLAYMODE_PAUSE,
    DISPLAYMODE_RUN_TIME,
    DISPLAYMODE_CONFIG_READONLY,
    DISPLAYMODE_CONFIG_INTERACTIVE
    };
void UpdateGraphics(unsigned mode = DISPLAYMODE_RUN_TIME);
void GUI(bool interactive);
void GraphicPrint(IDirect3DSurface8* pd3ds, unsigned x, unsigned line, DWORD color, char* format, ...);


//
// Utility functions
//
void HexDump(const unsigned char * buffer, DWORD length);
WCHAR* Ansi2UnicodeHack(char *str);


//
// User input
//
extern HANDLE hDuke[XGetPortCount()];
extern HANDLE hSlot[XGetPortCount()*2];
static const DWORD USBPortMasks[] = {
    XDEVICE_PORT0_MASK,
    XDEVICE_PORT1_MASK,
    XDEVICE_PORT2_MASK,
    XDEVICE_PORT3_MASK
    };
static const DWORD USBSlotMasks[] = {
    XDEVICE_PORT0_BOTTOM_MASK,
    XDEVICE_PORT0_TOP_MASK,
    XDEVICE_PORT1_BOTTOM_MASK,
    XDEVICE_PORT1_TOP_MASK,
    XDEVICE_PORT2_BOTTOM_MASK,
    XDEVICE_PORT2_TOP_MASK,
    XDEVICE_PORT3_BOTTOM_MASK,
    XDEVICE_PORT3_TOP_MASK
    };


//
// graphics
//
extern IDirectSound *       g_dsound;
extern IDirect3D8*		    g_d3d;
extern IDirect3DDevice8*	g_pDevice;
extern BitFont				g_font;
extern IDirect3DTexture8*	g_pd3dtText;
extern TVertex				g_prText[4];
extern D3DLOCKED_RECT		g_d3dlr;


//
// stats
//
extern bool warmBoot;



#endif //_MEMPRINT_H_


