/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    memCheck.h

Abstract:

    Memory validator / scanner

Notes:

*****************************************************************************/

#ifndef _MEMCHECK_H_
#define _MEMCHECK_H_

#ifdef _VC
//#define DEVICE_TYPE ULONG
#define NTLEANANDMEAN
#define _X86_
#define _WIN32_WINNT 0x0500
#endif

#pragma warning(disable: 4200) // nonstandard extension used : zero-sized array in struct/union

#include <ntos.h>
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntexapi.h>
#include <scsi.h>
#include <ntddscsi.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntddcdvd.h>

#include <dm.h>
#include <xtl.h>
#include <stdio.h>

#include "../utils/hvsUtils.h"
#include "bitfont.h"

//
// utility functions
//
void DebugPrint(char* format, ...);
long GetNumber(char *string);

//
// Initialization calls
//
void Initialize(void);
HRESULT DeInitGraphics(void);
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
void GUI(void);
void GraphicPrint(IDirect3DSurface8* pd3ds, unsigned x, unsigned line, DWORD color, char* format, ...);


//
// Utility functions
//
void HexDump(const unsigned char * buffer, DWORD length);
WCHAR* Ansi2UnicodeHack(char *str);
BOOL CompareBuff( unsigned char *buff1, unsigned char *buff2, unsigned length );


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
// config settings
//
struct ConfigSettings : public CoreConfigSettings
{
    DWORD dwBlockSize;
    DWORD dwStartBlock;
    DWORD dwEndBlock;
    DWORD dwReadMode;
    DWORD dwPauseOnExit;
    DWORD dwEnableGraphics;
    DWORD dwGraphicsUpdateInterval;
    DWORD dwEnableUSB;
    DWORD dwEnableNetwork;
    DWORD dwDisableInterrupts;

    ConfigSettings()
    {
        dwBlockSize = 0;
        dwStartBlock = 0;
        dwEndBlock = 0;
        dwReadMode = 0;
        dwPauseOnExit = 0;
        dwEnableGraphics = 0;
        dwGraphicsUpdateInterval = 0;
        dwEnableUSB = 0;
        dwEnableNetwork = 0;
        dwDisableInterrupts = 0;
    }
};

extern ConfigSettings globalSettings;

//
// stats
//
extern DWORD dwNumPass;
extern DWORD dwNumFail;
extern DWORD dwNumBuffMisMatch;
extern unsigned __int64 ui64NumBlocksRead;
extern unsigned __int64 ui64Time;
extern DWORD dwNumTimesThrough;
extern DWORD dwTestPassNumber;
extern WCHAR g_pwszMemType[50];
extern DWORD g_dwTotalMemory;

//
// graphics
//
extern IDirect3D8*		    g_d3d;
extern IDirect3DDevice8*	g_pDevice;
extern BitFont				g_font;
extern IDirect3DTexture8*	g_pd3dtText;
extern TVertex				g_prText[4];
extern D3DLOCKED_RECT		g_d3dlr;

FORCEINLINE
void * __cdecl MEMMOVE (
        void * dst,
        const void * src,
        size_t count
        )
{
        void * ret = dst;

#if     defined(_M_MRX000) || defined(_M_ALPHA) || defined(_M_PPC) || defined(_M_IA64)
        {
        extern void RtlMoveMemory( void *, const void *, size_t count );

        RtlMoveMemory( dst, src, count );
        }
#else
        if (dst <= src || (char *)dst >= ((char *)src + count)) {
                /*
                 * Non-Overlapping Buffers
                 * copy from lower addresses to higher addresses
                 */
                while (count--) {
                        *(char *)dst = *(char *)src;
                        dst = (char *)dst + 1;
                        src = (char *)src + 1;
                }
        }
        else {
                /*
                 * Overlapping Buffers
                 * copy from higher addresses to lower addresses
                 */
                dst = (char *)dst + count - 1;
                src = (char *)src + count - 1;

                while (count--) {
                        *(char *)dst = *(char *)src;
                        dst = (char *)dst - 1;
                        src = (char *)src - 1;
                }
        }
#endif

        return(ret);
}

FORCEINLINE
UINT64
ReadTSC(
    VOID
    )
/*++

Routine Description:

    This routine reads processor's time-stamp counter.  The time-stamp counter
    is contained in a 64-bit MSR.  The high-order of 32 bits MSR are loaded
    into the EDX register, and the low-order 32 bits are loaded into the EAX
    register.  The processor increments the time-stamp counter MSR every
    clock cycle and resets it to 0 whenever the processor reset.

Arguments:

    None

Return Value:

    64-bit MSR of time-stamp counter

--*/
{
    __asm {
        rdtsc
    }
}

#endif //_MEMCHECK_H_
