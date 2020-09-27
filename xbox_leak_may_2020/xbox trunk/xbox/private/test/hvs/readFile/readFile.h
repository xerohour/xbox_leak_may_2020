/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    readFile.h

Abstract:

    File System disk scanner.

*****************************************************************************/

#ifndef _READFILE_H_
#define _READFILE_H_

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
#include <ntexapi.h>
#include <scsi.h>
#include <ntddscsi.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntddcdvd.h>

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
void CompareBuff(unsigned char *buff1, unsigned char *buff2, unsigned char *buff3, unsigned length);
DWORD MapDrive( char cDriveLetter, char* pszPartition );
DWORD UnMapDrive( char cDriveLetter );


typedef bool (*PARSE_FUNC)(char *path, WIN32_FIND_DATA &data);
bool ParseFiles(char *path, PARSE_FUNC funct);



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

//
// config settings
//
struct ConfigSettings : public CoreConfigSettings
    {
    char drive[128];
    DWORD compareBuffers;
    DWORD numberOfReads;
    DWORD pauseOnMismatch;
    DWORD pauseOnExit;
    DWORD readSize;
    DWORD dir;

    ConfigSettings()
        {
        strcpy(drive, "A:");
        compareBuffers = 0;
        numberOfReads = 2;
        pauseOnMismatch = 0;
        pauseOnExit = 0;
        readSize = 1024;
        dir = 0;

        stopAfter.type = STOPAFTER_ITERATIONS;
        stopAfter.duration = 1;
        }
    };

extern ConfigSettings globalSettings;


//
// stats
//
extern DWORD numFail;
extern DWORD numPass;
extern DWORD numBuffMisMatch;
extern DWORD numTimesThrough;
extern unsigned __int64 numFiles;
extern unsigned __int64 numFilesRead;
extern DWORD time;
extern char model[64], serial[64], firmware[64];
extern WIN32_FIND_DATA findData;
extern double percentDone;
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


//
// Low level IOCTL stuff
//
/*
#define CTL_CODE(DeviceType, Function, Method, Access) ( \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) )

#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_DVD                 0x00000033
#define FILE_DEVICE_MASS_STORAGE        0x0000002d
#define FILE_DEVICE_CONTROLLER          0x00000004

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

#define FILE_ANY_ACCESS                 0

#define IOCTL_DVD_BASE                  FILE_DEVICE_DVD
#define IOCTL_DVD_START_SESSION         CTL_CODE(IOCTL_DVD_BASE, 0x0400, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_READ_KEY              CTL_CODE(IOCTL_DVD_BASE, 0x0401, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_SEND_KEY              CTL_CODE(IOCTL_DVD_BASE, 0x0402, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_END_SESSION           CTL_CODE(IOCTL_DVD_BASE, 0x0403, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_SET_READ_AHEAD        CTL_CODE(IOCTL_DVD_BASE, 0x0404, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_GET_REGION            CTL_CODE(IOCTL_DVD_BASE, 0x0405, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_SEND_KEY2             CTL_CODE(IOCTL_DVD_BASE, 0x0406, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_DVD_READ_STRUCTURE        CTL_CODE(IOCTL_DVD_BASE, 0x0450, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CDROM_BASE                FILE_DEVICE_CD_ROM
#define IOCTL_CDROM_GET_DRIVE_GEOMETRY  CTL_CODE(IOCTL_CDROM_BASE, 0x0013, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CDROM_SET_SPINDLE_SPEED   CTL_CODE(IOCTL_CDROM_BASE, 0x0021, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CDROM_CHECK_VERIFY        CTL_CODE(IOCTL_CDROM_BASE, 0x0200, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_SCSI_BASE                 FILE_DEVICE_CONTROLLER
#define IOCTL_SCSI_PASS_THROUGH_DIRECT  CTL_CODE(IOCTL_SCSI_BASE, 0x0405, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
//#define IOCTL_IDE_PASS_THROUGH          CTL_CODE(IOCTL_SCSI_BASE, 0x040a, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define CDAUDIO_BYTES_PER_FRAME         2352
#define CDAUDIO_BYTES_PER_SECOND        176400
#define CDAUDIO_BYTES_PER_MINUTE        10584000
#define CDAUDIO_FRAMES_PER_SECOND       75
#define CDAUDIO_FRAMES_PER_MINUTE       4500
#define CDAUDIO_SECTOR_OFFSET           150

// from private\ntos\idex\idex.h:
#define IDE_ATAPI_RAW_CD_SECTOR_SIZE            2352
#define IDE_ATAPI_CD_SECTOR_SHIFT               11
*/


#define ALIGN_DOWN(length, type) \
    ((ULONG)(length) & ~(sizeof(type) - 1))

#define ALIGN_UP(length, type) \
    (ALIGN_DOWN(((ULONG)(length) + sizeof(type) - 1), type))

#endif //_READFILE_H_


