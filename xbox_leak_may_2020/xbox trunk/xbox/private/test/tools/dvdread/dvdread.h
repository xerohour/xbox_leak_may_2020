/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    dvdread.h

Abstract:

    Low level disk test scanner.

Notes:
    dvd player code:
        "\xboxsrc\private\ui\dvd\library\hardware\drives\specific\xboxdvddrive.cpp"
    filesystem retry code:
        "\xboxsrc\private\ntos\idex\cdrom.c" (IdexCdRomFinishRequestSense)
    sense data format:
        "\xboxsrc\public\ddk\inc\scsi.h"


    DVD-X2 Supported Error Sense Codes
        Sense   Sense   Sense   Description
        Key	    Code    Code Qu
        ----------------------------------------------------------------------
        0x00	0x00	0x00	No Additional Sense information (= no error)
        0x01	0x80	0x00	Cache fill error, speed reduction requested
        0x02	0x04	0x00	Logical Unit not ready, cause not reported
        0x02	0x04	0x01	Logical Unit is in process of becoming ready
        0x02	0x30	0x00	Incompatible medium installed
        0x02	0x3A	0x00	Medium not present
        0x03	0x57	0x00	Unable to recover Table of Contents
        0x03	0x80	0x00	Read Error
        0x04	0x00	0x00	Hardware error
        0x04	0x08	0x03	Logical Unit Communication CRC Error - UDMA33
        0x05	0x21	0x00	Logical Block Address Out of Range
        0x05	0x55	0x00	System Resource Failure
        0x05	0x64	0x00	Illegal mode for this track
        0x05	0x6F	0x00	Authentication Failure - CSS
        0x05	0x80	0x00	Authentication Failure - Xbox
        0x05	0x81	0x00	Command error
        0x06	0x29	0x00	Power On, Reset or Bus Device Reset occurred
        0x06	0x2A	0x01	Mode Parameters changed
        0x06	0x2E	0x00	Insufficient time for operation

    Win32 Error code mapping:
        1/80:   ERROR_CRC                   23
        2/30:   ERROR_UNRECOGNIZED_MEDIA    1785
        2/other ERROR_NOT_READY             21
        3/30:   ERROR_UNRECOGNIZED_MEDIA    1785
        3/other ERROR_CRC                   23
        5/21:   ERROR_SECTOR_NOT_FOUND      27
        5/30:   ERROR_UNRECOGNIZED_MEDIA    1785
        5/6F:   ERROR_TOO_MANY_SECRETS      1381
        5/80:   ERROR_TOO_MANY_SECRETS      1381
        other:  ERROR_IO_DEVICE             1117

    IOCTL_CDROM_SET_SPINDLE_SPEED
        speed 2     ~3000rpm      2x -    5x
        speed 1     ~2000rpm    4/3x - 10/3x
        speed 0     ~1000rpm    2/3x -  5/3x

    Hard Drive Partition Sizes
        \Device\Harddisk0\Partition0 19541088
        \Device\Harddisk0\Partition1  9997568
        \Device\Harddisk0\Partition2  1023840
        \Device\Harddisk0\Partition3  1535776
        \Device\Harddisk0\Partition4  1535776
        \Device\Harddisk0\Partition5  1535776

*****************************************************************************/

#ifndef _DVDREAD_H_
#define _DVDREAD_H_

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

#include "bitfont.h"

//
// utility functions
//
void DebugPrint(char* format, ...);
long GetNumber(char *string);

enum READ_MODES
    {
    READMODE_DETECT = 0,
    READMODE_DVD,
    READMODE_CD,
    READMODE_HD,
    READMODE_MAX
    };

struct SCSIError
    {
    DWORD win32;
    SENSE_DATA sense;

    SCSIError() {win32=0; memset(&sense, 0, sizeof(SENSE_DATA)); }
    void Print(bool full=true)
        {
        unsigned char *s = (unsigned char*)&sense;
        if(full)
            DebugPrint("(%u [<%02X> %02X <%02X> %02X%02X%02X%02X %02X %02X%02X%02X%02X <%02X> <%02X> %02X %02X%02X%02X])",
                win32, s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8], s[9], s[10], s[11], s[12], s[13], s[14], s[15], s[16], s[17]);
        else
            DebugPrint("(%u [<%02X> -- <%02X> -------- -- -------- <%02X> <%02X> -- ------])",
                win32, s[0], s[2], s[12], s[13]);
        }
    };

class ErrorDistrubition
    {
    public:
        SCSIError error;
        DWORD count;
        ErrorDistrubition *next;

    public:
        ErrorDistrubition(SCSIError &e)
            {
            memcpy(&error, &e, sizeof(SCSIError));
            count = 1;
            next = NULL;
            }

        void AddError(SCSIError &e)
            {
            ErrorDistrubition *ptr = this;
            while(ptr)
                {
                if((ptr->error.sense.ErrorCode                     == e.sense.ErrorCode)           &&
                    (ptr->error.sense.Valid                        == e.sense.Valid)               &&
                    (ptr->error.sense.SenseKey                     == e.sense.SenseKey)            &&
                    (ptr->error.sense.Reserved                     == e.sense.Reserved)            &&
                    (ptr->error.sense.IncorrectLength              == e.sense.IncorrectLength)     &&
                    (ptr->error.sense.EndOfMedia                   == e.sense.EndOfMedia)          &&
                    (ptr->error.sense.FileMark                     == e.sense.FileMark)            &&
                    (ptr->error.sense.AdditionalSenseCode          == e.sense.AdditionalSenseCode) &&
                    (ptr->error.sense.AdditionalSenseCodeQualifier == e.sense.AdditionalSenseCodeQualifier))
                    {
                    ptr->count++;
                    return;
                    }
                else if(!ptr->next)
                    {
                    ptr->next = new ErrorDistrubition(e);
                    return;
                    }
                ptr = ptr->next;
                }
            }

        void RemoveError(SCSIError &e, DWORD count)
            {
            ErrorDistrubition *ptr = this;
            while(ptr)
                {
                if(memcmp(&ptr->error, &e, sizeof(SCSIError)) == 0)
                    {
                    ptr->count -= count;
                    return;
                    }
                ptr = ptr->next;
                }
            }

        void PrintErrors(void)
            {
            ErrorDistrubition *ptr = this;
            while(ptr)
                {
                DebugPrint("*** %9u Errors of ", ptr->count);
                ptr->error.Print(false);
                DebugPrint("\n");
                ptr = ptr->next;
                }
            }
    };


struct ExcludeList
    {
    DWORD lba1, lba2;
    ExcludeList *next;

    ExcludeList() { lba1=lba2=0; next=NULL; }
    ExcludeList(char *pair)
        {
        next = NULL;
        lba1 = GetNumber(pair);
        char *tmp = strchr(pair, '-');
        if(tmp) lba2 = GetNumber(tmp+1);
        else lba2 = lba1;
        }
    void Add(char *pair)
        {
        ExcludeList *ptr = this;
        while(ptr)
            {
            if(!ptr->next)
                {
                ptr->next = new ExcludeList(pair);
                return;
                }
            ptr = ptr->next;
            }
        }
    bool InList(DWORD lba, DWORD readSize)
        {
        ExcludeList *ptr = this;
        while(ptr)
            {
            if(lba+readSize >= ptr->lba1 && lba <= ptr->lba2)
                {
                return true;
                }
            ptr = ptr->next;
            }
        return false;
        }
    };





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
void PrintPreRetryError(DWORD block, SCSIError &err);
void CompareBuff(unsigned char *buff1, unsigned char *buff2, unsigned char *buff3, unsigned length);


//
// DVD utils
//
typedef SCSIError (*READ_FUNC)(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer);
SCSIError ReadDVDBlocks(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer);
SCSIError ReadCDBlocks(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer);
SCSIError ReadCDBlocks2(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer);
SCSIError ReadHDBlocks(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer);
void SeekLBA(HANDLE hDevice, DWORD LBA);
void GetDriveID(HANDLE hDevice, bool cdrom, char* model, char* serial, char* firmware);
void ChangeSpeed(HANDLE hDevice, DWORD newSpeed);
bool CDTestReady(HANDLE hDevice);
void RequestSense(HANDLE hDevice, SCSIError &sense);
DWORD GetDiskType(HANDLE hDevice);


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
extern DWORD avgTime;
extern DISK_GEOMETRY gDiskGeometry;
extern DWORD successfulReadSectors;
extern int enableRetries;
extern DWORD spindleSpeed;
extern DWORD speedDelay0to1;
extern DWORD speedDelay1to2;
extern DWORD speedDelay2to1;
extern DWORD speedDelay1to0;
extern DWORD speedDelay0to2;
extern DWORD speedDelay2to0;
extern DWORD delayOn062E;
extern DWORD seekOn062E;
extern DWORD summaryInternalErrs;
extern ErrorDistrubition *errors;
extern DWORD lastReadTime;
extern DWORD avgTimeCount;
extern DWORD chunkSizeMin;
extern DWORD chunkSizeMax;
extern char deviceName[128];
extern DWORD readSize;
extern DWORD startLBA, endLBA ;
extern DWORD compareBuffers;
extern DWORD sequentialReads;
extern DWORD pauseOnMismatch;
extern DWORD dvdUseReadFile;
extern DWORD readMode;
extern DWORD stopAfter;
extern DWORD numberOfReads;
extern ExcludeList *exclude;
extern DWORD requestSense;
extern DWORD launchGUI;
extern DWORD avgSpan;
extern DWORD defaultSpeed;
extern DWORD seed;
extern DWORD mediaType;

//
// stats
//
extern DWORD numFailThenFail;
extern DWORD numFailThenFailSame;
extern DWORD numFailThenPass;
extern DWORD numPassThenFail;
extern DWORD numBuffMisMatch;
extern unsigned __int64 numBlocksRead;
extern DWORD time;
extern char model[64], serial[64], firmware[64];
extern DWORD numTimesThrough;

//
// graphics
//
extern IDirect3D8*		    g_d3d;
extern IDirect3DDevice8*	g_pDevice;
extern BitFont				g_font;
extern IDirect3DTexture8*	g_pd3dtText;
extern TVertex				g_prText[4];
extern D3DLOCKED_RECT		g_d3dlr;




//
// Low level IOCTL stuff
//
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


#define ALIGN_DOWN(length, type) \
    ((ULONG)(length) & ~(sizeof(type) - 1))

#define ALIGN_UP(length, type) \
    (ALIGN_DOWN(((ULONG)(length) + sizeof(type) - 1), type))

enum MEDIA_TYPES
	{
	MEDIA_UNKNOWN,
	MEDIA_CDDA,
	MEDIA_CDROM,
	MEDIA_DVD_5_RO,
	MEDIA_DVD_5_RW,
	MEDIA_DVD_9_RO,
	MEDIA_DVD_9_RW,
	MEDIA_HD
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
    "Hard Disk"
    };

#endif //_DVDREAD_H_


