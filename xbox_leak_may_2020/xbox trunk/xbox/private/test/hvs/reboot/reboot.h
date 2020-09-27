/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    reboot.h

Abstract:

    File System disk scanner.

*****************************************************************************/

#ifndef _REBOOT_H_
#define _REBOOT_H_

#ifdef _VC
//#define DEVICE_TYPE ULONG
#define NTLEANANDMEAN
#define _X86_
#define _WIN32_WINNT 0x0500
#endif

#pragma warning(disable: 4200) // nonstandard extension used : zero-sized array in struct/union

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntos.h>
#include <ntexapi.h>
#include <scsi.h>
#include <ntddscsi.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntddcdvd.h>
#include <smcdef.h>

#include <xtl.h>
#include <stdio.h>

#include "../utils/hvsUtils.h"
#include "bitfont.h"

#define CLEARFLAG(var, flag)    (var &= ~(flag))
#define SETFLAG(var, flag)      (var |=  (flag))

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

enum MEDIA_TYPES
	{
	MEDIA_UNKNOWN,
	MEDIA_CDDA,
	MEDIA_CDROM,
	MEDIA_DVD_5_RO,
	MEDIA_DVD_5_RW,
	MEDIA_DVD_9_RO,
	MEDIA_DVD_9_RW,
	MEDIA_HD,
    MEDIA_MU
	};

static const char *mediaTypeNames[] = 
    {
    "Unknown",
    "CD DA",
    "CD ROM",
    "DVD 5 RO",
    "DVD 5 RW",
    "DVD 9 RO",
    "DVD 9 RW",
    "Hard Disk",
    "MU"
    };

struct Stats
    {
    bool launchedFromHVS;
    DWORD numSoftReboots;
    DWORD numHardReboots;
    DWORD numQuickReboots;
    DWORD numSMCReboots;
    DWORD failures;
    DWORD runTime;
    DWORD mediaType;
    unsigned __int64 mediaSize;
    char driveD[MAX_PATH];

    Stats()
        {
        launchedFromHVS = false;
        numSoftReboots = 0;
        numHardReboots = 0;
        numQuickReboots = 0;
        numSMCReboots = 0;
        failures = 0;
        runTime = 0;
        mediaType = MEDIA_UNKNOWN;
        mediaSize = 0;
        memset(driveD, 0, MAX_PATH);
        }
    };

enum REBOOT_TYPES
    {
    REBOOT_SOFT,
    REBOOT_HARD,
    REBOOT_QUICK,
    REBOOT_SMC,
    REBOOT_RANDOM
    };

//
// config settings
//
struct ConfigSettings : public CoreConfigSettings
    {
    DWORD rebootType;
    DWORD seed;

    ConfigSettings()
        {
        seed = 0;
        rebootType = REBOOT_QUICK;

        stopAfter.type = STOPAFTER_ITERATIONS;
        stopAfter.duration = 5;
        }
    };

extern ConfigSettings globalSettings;


//
// stats
//
extern Stats stats;
extern char model[64], serial[64], firmware[64];
extern DWORD mediaType;
extern DWORD addDevice, removeDevice;


//
// graphics
//
extern IDirect3D8*		    g_d3d;
extern IDirect3DDevice8*	g_pDevice;
extern BitFont				g_font;
extern IDirect3DTexture8*	g_pd3dtText;
extern TVertex				g_prText[4];
extern D3DLOCKED_RECT		g_d3dlr;


void GetDriveID(HANDLE hDevice, bool cdrom, char* model, char* serial, char* firmware);
DWORD GetDiskType(HANDLE hDevice);
DWORD MapDrive(char cDriveLetter, char* pszPartition);
DWORD UnMapDrive(char cDriveLetter);
DWORD GetMapDrive(char cDriveLetter, char *partition);
bool FileExists(const char *filename);



#define ALIGN_DOWN(length, type) \
    ((ULONG)(length) & ~(sizeof(type) - 1))

#define ALIGN_UP(length, type) \
    (ALIGN_DOWN(((ULONG)(length) + sizeof(type) - 1), type))

#endif //_REBOOT_H_


