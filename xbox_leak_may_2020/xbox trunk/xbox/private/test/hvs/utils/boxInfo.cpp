/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    hvsUtils.cpp

Abstract:

    HVS Test Helper functions

Notes:

*****************************************************************************/

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
#include <xbeimage.h>
extern "C"
    {
    #include <init.h>
    }
#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

#include <xtl.h>
#include <stdio.h>

#include "hvsUtils.h"

extern "C"
    {
    DECLSPEC_IMPORT
    void
    WINAPI
    HalReadWritePCISpace(
        IN ULONG BusNumber,
        IN ULONG SlotNumber,
        IN ULONG RegisterNumber,
        IN PVOID Buffer,
        IN ULONG Length,
        IN BOOLEAN WritePCISpace
        );
    #define HalReadPCISpace(BusNumber, SlotNumber, RegisterNumber, Buffer, Length) \
        HalReadWritePCISpace(BusNumber, SlotNumber, RegisterNumber, Buffer, Length, FALSE)

    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalReadSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );

    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalWriteSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN WriteWordValue,
        IN ULONG DataValue
        );

    void*
    WINAPI
    MmMapIoSpace(
        IN ULONG_PTR PhysicalAddress,
        IN SIZE_T NumberOfBytes,
        IN ULONG Protect
        );

    void
    WINAPI
    MmUnmapIoSpace(
        IN PVOID BaseAddress,
        IN SIZE_T NumberOfBytes
        );
    }

typedef struct _PCI_SLOT_NUMBER {
    union {
        struct {
            ULONG   DeviceNumber:5;
            ULONG   FunctionNumber:3;
            ULONG   Reserved:24;
        } bits;
        ULONG   AsULONG;
    } u;
} PCI_SLOT_NUMBER, *PPCI_SLOT_NUMBER;

FORCEINLINE ULONG REG_RD32(VOID* Ptr, ULONG Addr)
    {
    return *((volatile DWORD*)((BYTE*)(Ptr) + (Addr)));
    }

static DWORD WriteSMC(unsigned char addr, unsigned char value)
    {
    return HalWriteSMBusValue(0x20, addr, FALSE, value);
    }

static DWORD ReadSMC(unsigned char addr)
    {
    DWORD value = 0xCCCCCCCC;
    DWORD status = HalReadSMBusValue(0x21, addr, FALSE, &value);
    return value;
    }

static const char * const vmodes[] =
    {
    "SDTV SCART Analog/Digital",
    "HDTV",
    "VGA",
    "RFU",
    "SDTV Analog/Digital",
    "Power Off",
    "SDTV Analog",
    "No Pack",
    "SMC Error"
    };

static const char * const hwversions[] =
    {
    "Unknown",                  // 0
    "DVT3 (old metal)",         // 1
    "DVT3 (new metal)",         // 2
    "DVT4, DVT5, XDK2 (FAB F)", // 3
    "DVT6, XDK2 (FAB G/H)",     // 4
    "QT"                        // 5
    };

static const char * const lang[] = 
    {
    "Unknown",
    "English",
    "Japanese",
    "German",
    "French",
    "Spanish",
    "Italian"
    };

static const char * const videostd[] =
    {
    "Unknown",
    "NTSC_M",
    "NTSC_J",
    "PAL_I"
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

static char* GetHWVersion(unsigned char &hwver);
static void DumpClocks(char *buffer, unsigned hwver);
static void GetPeripheral(char *buffer);
static void GetDriveID(char* device, char* model, char* serial, char* firmware);
static DWORD GetDiskType(void);


/*****************************************************************************

Routine Description:

    GetBoxInformation

    Retrieves detailed information on the current box (hw and sw versions 
    etc.)

Arguments:

    none

Return Value:

    char* - newly allocated string

Notes:

    Caller MUST delete[] the returned pointer !!!

*****************************************************************************/
char *GetBoxInformation(void)
    {
    DWORD buffLen;
    bool multiple;
    char *info;
    char *boxInfo = new char[8*KILOBYTE];
    if(!boxInfo) return NULL;
    info = boxInfo;

    char *tempBuff = new char[KILOBYTE];
    if(!tempBuff)
        {
        delete[] boxInfo;
        return NULL;
        }

    // hw version
    unsigned char baseInitTable = 0;
    info += sprintf(info, "HW Ver = %s %s\n", GetHWVersion(baseInitTable), (XboxHardwareInfo->Flags&XBOX_HW_FLAG_DEVKIT_KERNEL)?"DEVKIT":"RETAIL");
    info += sprintf(info, "GPU Ver = %02X\n", XboxHardwareInfo->GpuRevision);
    info += sprintf(info, "MCP Ver = %02X\n", XboxHardwareInfo->McpRevision);

    // get SMC version
    tempBuff[0] = (char)ReadSMC(0x01);
    tempBuff[1] = (char)ReadSMC(0x01);
    tempBuff[2] = (char)ReadSMC(0x01);
    tempBuff[3] = '\0';
    info += sprintf(info, "SMC Ver = %s\n", tempBuff);

    // get clock speeds
    DumpClocks(tempBuff, baseInitTable);
    info += sprintf(info, "Clock Speeds = %s\n", tempBuff);

    // get memory information
    DWORD memTest = ReadSMC(0x14);
    #define NV_PEXTDEV_BOOT_0                                0x00101000 // private\windows\directx\dxg\d3d8\se\nv_ref_2a.h
    #define XPCICFG_GPU_MEMORY_REGISTER_BASE_0               0xFD000000 // private\ntos\inc\xpcicfg.h
    if((REG_RD32((void*)XPCICFG_GPU_MEMORY_REGISTER_BASE_0, NV_PEXTDEV_BOOT_0) & 0x0000C0000) == 0)
        info += sprintf(info, "Memory Type = Micron\n");
    else
        info += sprintf(info, "Memory Type = Samsung\n");
    info += sprintf(info, "Memory Size = %s\n", ReadSMC(0x15)==1 ? "4 Meg chips" : "2 Meg chips");
    info += sprintf(info, "Memory Test = 0x%08X (%s)\n", memTest, memTest?"FAIL":"PASS");

    // get hard disk manufacturer
    memset(tempBuff, 0, KILOBYTE);
    GetDriveID("\\Device\\Harddisk0\\Partition0", tempBuff, tempBuff+384, tempBuff+768);
    info += sprintf(info, "HDD Info = %s, %s, %s\n", tempBuff, tempBuff+384, tempBuff+768);

    // get dvd drive information
    memset(tempBuff, 0, KILOBYTE);
    GetDriveID("\\Device\\CdRom0", tempBuff, tempBuff+384, tempBuff+768);
    info += sprintf(info, "DVD Info = %s, %s, %s\n", tempBuff, tempBuff+384, tempBuff+768);
    info += sprintf(info, "Media Type = %s\n", mediaTypeNames[GetDiskType()]);

    // hw flags "private/ntos/inc/init.h"
    tempBuff[0] = '\0';
    multiple=false;
    if(XboxHardwareInfo->Flags & XBOX_HW_FLAG_INTERNAL_USB_HUB)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "USB HUB");
        }
    if(XboxHardwareInfo->Flags & XBOX_480P_MACROVISION_ENABLED)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "480p");
        }
    info += sprintf(info, "HW Flags = %s\n", tempBuff);

    // language
    DWORD language = XGetLanguage();
    if(language >= ARRAYSIZE(lang)) language = 0;
    info += sprintf(info, "Language = %s\n", lang[language]);

    // game region
    DWORD region = XGetLanguage();
    if(region == XC_GAME_REGION_NA)
        info += sprintf(info, "Game Region = North America\n");
    else if(region == XC_GAME_REGION_JAPAN)
        info += sprintf(info, "Game Region = Japan\n");
    else if(region == XC_GAME_REGION_RESTOFWORLD)
        info += sprintf(info, "Game Region = Europe, etc.\n");
    else
        info += sprintf(info, "Game Region = Unknown\n");

    // av pack
    DWORD avpack = ReadSMC(0x04);
    if(avpack > 8) avpack = 8; // ReadSMC() can return 0xCCCCCCCC on error
    info += sprintf(info, "AV Pack = %s\n", vmodes[avpack]);

    // video standard
    DWORD vidStd = XGetVideoStandard();
    if(vidStd >= ARRAYSIZE(videostd)) vidStd = 0;
    info += sprintf(info, "Video Standard = %s\n", videostd[vidStd]);

    // get video flags
    DWORD vidFlags = XGetVideoFlags();
    multiple=false;
    tempBuff[0] = '\0';
    strcat(tempBuff, "(");
    if(vidFlags & XC_VIDEO_FLAGS_WIDESCREEN)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "WIDE");
        }
    if(vidFlags & XC_VIDEO_FLAGS_HDTV_720p)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "720p");
        }
    if(vidFlags & XC_VIDEO_FLAGS_HDTV_1080i)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "1080i");
        }
    if(vidFlags & XC_VIDEO_FLAGS_LETTERBOX)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "LETTERBOX");
        }
    if(vidFlags & XC_VIDEO_FLAGS_PAL_60Hz)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "PAL60Hz");
        }
    strcat(tempBuff, ")");
    info += sprintf(info, "Video Flags = %s\n", tempBuff);

    // get audio flags
    DWORD audioFlags = XGetAudioFlags();
    multiple=false;
    tempBuff[0] = '\0';
    if(audioFlags & XC_AUDIO_FLAGS_SURROUND)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "SURROUND");
        }
    if(audioFlags & XC_AUDIO_FLAGS_STEREO)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "STEREO");
        }
    if(audioFlags & XC_AUDIO_FLAGS_MONO)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "MONO");
        }
    if(audioFlags & XC_AUDIO_FLAGS_ENABLE_AC3)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "AC3");
        }
    if(audioFlags & XC_AUDIO_FLAGS_ENABLE_DTS)
        {
        if(multiple) multiple = !strcat(tempBuff, ", ");
        multiple = !!strcat(tempBuff, "DTS");
        }
    info += sprintf(info, "Audio Flags = %s\n", tempBuff);

    // USB Devices
    tempBuff[0] = '\0';
    GetPeripheral(tempBuff);
    info += sprintf(info, "Peripherals = %s\n", tempBuff);

    // Kernel and library version
    info += sprintf(info, "ROM Version = %u %s\n", XboxKrnlVersion->Build, (!!(0x8000 & XboxKrnlVersion->Qfe))?"checked":"free");
    info += sprintf(info, "XTL Version = %u %s\n", XeImageHeader()->XapiLibraryVersion->BuildVersion, XeImageHeader()->XapiLibraryVersion->DebugBuild?"checked":"free");

    // SMC info
    info += sprintf(info, "CPU Temp = %d\n", ReadSMC(0x09));
    info += sprintf(info, "AIR Temp = %d\n", ReadSMC(0x0A));
    info += sprintf(info, "Fan = %d\n", ReadSMC(0x10));
    info += sprintf(info, "SMC Error = %d\n", ReadSMC(0x0F));


    delete[] tempBuff;
    return boxInfo;
    }






static DWORD GetDiskType(void)
    {
    DWORD type = MEDIA_UNKNOWN;
    #define ALIGN_DOWN(length, type)    ((ULONG)(length) & ~(sizeof(type) - 1))
    #define ALIGN_UP(length, type)      (ALIGN_DOWN(((ULONG)(length) + sizeof(type) - 1), type))

    DWORD returned;
    NTSTATUS status;
    HANDLE handle;
    OBJECT_ATTRIBUTES ObjA;
    OBJECT_STRING VolumeString;
    IO_STATUS_BLOCK IoStatusBlock;

    RtlInitObjectString(&VolumeString, "\\Device\\CdRom0");
    InitializeObjectAttributes(&ObjA, &VolumeString, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = NtCreateFile(&handle,
                            SYNCHRONIZE|GENERIC_READ,
                            &ObjA,
                            &IoStatusBlock,
                            0,
                            FILE_ATTRIBUTE_NORMAL,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN,
                            FILE_SYNCHRONOUS_IO_NONALERT);


    DVD_READ_STRUCTURE readStruct;
    UCHAR readStructureOutput[ALIGN_UP(sizeof(DVD_DESCRIPTOR_HEADER) + sizeof(DVD_LAYER_DESCRIPTOR), ULONG)];

    memset(&readStruct, 0, sizeof(DVD_READ_STRUCTURE));
    memset(readStructureOutput, 0, sizeof(readStructureOutput));

    // try DVD Read Structure
    readStruct.Format = DvdPhysicalDescriptor;

    returned = 0;
    BOOL result = DeviceIoControl(handle, IOCTL_DVD_READ_STRUCTURE, &readStruct, sizeof(DVD_READ_STRUCTURE), readStructureOutput, sizeof(readStructureOutput), &returned, NULL);

    NtClose(handle);

    if(result)
        {
        DVD_LAYER_DESCRIPTOR *layerDescr = (DVD_LAYER_DESCRIPTOR *)(&readStructureOutput[sizeof(DVD_DESCRIPTOR_HEADER)]);
        BYTE layerType = (layerDescr->BookType) | (layerDescr->BookVersion << 4);
        if(layerDescr->LayerType == 1)
            {
            if(layerDescr->NumberOfLayers == 0)
                type = MEDIA_DVD_5_RO;
            else type = MEDIA_DVD_9_RO;
            }
        else
            {
            if(layerDescr->NumberOfLayers == 0)
                type = MEDIA_DVD_5_RW;
            else type = MEDIA_DVD_9_RW;
            }
        }

    // not a DVD disk...
    else
        {
	    CDROM_TOC toc;
        memset(&toc, 0, sizeof(CDROM_TOC));
		result = DeviceIoControl( handle, IOCTL_CDROM_READ_TOC, &toc, sizeof(CDROM_TOC), &toc, sizeof(CDROM_TOC), &returned, NULL);
        if(result)
            {
            if(toc.FirstTrack == toc.LastTrack) type = MEDIA_CDROM;
            type = MEDIA_CDDA;
            }
        else
            {
            type = MEDIA_UNKNOWN;
            }
        }

    NtClose(handle);
    return type;
    }



static void DumpClocks(char *buffer, unsigned hwver)
    {
    #define NV_PRAMDAC_NVPLL_COEFF 0x00680500 /* RW-4R */
    const float F_XTAL_135 = 13.5f;
    const float F_XTAL_166 = 16.6667f;

    PCI_SLOT_NUMBER SlotNumber;
    DWORD MPLLCoeff;
    DWORD m;
    DWORD n;
    DWORD p;
    DWORD fsbspeed;
    DWORD vcofreq;
    DWORD nvclk;
    DWORD nvpll;
    DWORD fsb_pdiv;
    DWORD mem_pdiv;
    DWORD mclk;
    DWORD cpuspeed;
    float XTAL;

    if(hwver >= 0x46) 
        {
        XTAL = F_XTAL_166;
        }
    else
        {
        XTAL = F_XTAL_135;
        }

    //
    // Read CR_CPU_MPLL_COEFF 
    //
    SlotNumber.u.AsULONG = 0;
    SlotNumber.u.bits.DeviceNumber = 0;
    SlotNumber.u.bits.FunctionNumber = 3;
    HalReadPCISpace(0, SlotNumber.u.AsULONG, 0x6C, &MPLLCoeff, sizeof(MPLLCoeff));

    m = MPLLCoeff & 0xFF;
    n = (MPLLCoeff >> 8) & 0xFF;

    if(m != 0)
        {
        //
        // Calculate cpu frequency
        //
        fsbspeed = (DWORD)((XTAL / m) * n);

        cpuspeed = (DWORD)((XTAL / m) * n * 11 / 2);
    
        //
        // Calculate nvclk
        //
        nvpll = REG_RD32((void*)XPCICFG_GPU_MEMORY_REGISTER_BASE_0, NV_PRAMDAC_NVPLL_COEFF);
        m = nvpll & 0xFF;
        n = (nvpll >> 8)  & 0xFF;
        p = (nvpll >> 16) & 0xFF;
    
        nvclk = (m != 0) ? (DWORD)((n * XTAL / (1 << p) / m)) : 0;
    
        //
        // Calculate vco
        //
        m = MPLLCoeff & 0xFF;
        n = (MPLLCoeff >> 8) & 0xFF;
        fsb_pdiv = (MPLLCoeff >> 16) & 0xF;
        
        vcofreq = (DWORD)((XTAL / m) * (fsb_pdiv * 2 * n));
    
        //
        // Calculate mclk
        //
        mem_pdiv = (MPLLCoeff >> 20) & 0xF;
        mclk = (DWORD)(vcofreq / (2 * mem_pdiv));

        sprintf(buffer, "Crystal=%s, FSB=%d, CPU=%d, NVCLK=%d, VCO=%d, MCLK=%d",
            (hwver >= 0x46) ? "16.6" : "13.5", fsbspeed, cpuspeed, nvclk, vcofreq, mclk);
        }
    else
        {
        sprintf(buffer, "unknown");
        }
    }

static char* GetHWVersion(unsigned char &hwver)
    {
    #define PAGE_SIZE                   4096
    #define ROM_SIZE                    (1024 * 1024)
    #define ROM_VERSION_OFFSET          30 // In DWORDs (0x78 is the absolute offset)
    #define ROM_VERSION_BYTE_OFFSET     0x78
    #define FLASH_BASE_ADDRESS          0xFFF00000
    #define FLASH_REGION_SIZE           (0xFFFFFFFF-FLASH_BASE_ADDRESS-1)

    LPBYTE RomBase = (LPBYTE)MmMapIoSpace(FLASH_BASE_ADDRESS, PAGE_SIZE, PAGE_READWRITE | PAGE_NOCACHE);

    if(RomBase == NULL) return (char*)hwversions[0];

    hwver = (unsigned char)(RomBase[ROM_VERSION_BYTE_OFFSET] & 0xFF);

    MmUnmapIoSpace(RomBase, PAGE_SIZE);

    //
    // Init table version < 0x40: Old DVT 3 Boxes with NV2A A02 old metal 7 (NVCLK 155, CPU 665)
    // Init table version = 0x40: New DVT 3 Boxes with NV2A A02 new metal 7 (Faster)
    // Init table version = 0x46: DVT4, DVT5, XDK2 (FAB F)
    // Init table version = 0x60: DVT6+, XDK2 (FAB G/H)
    // Init table version = 0x70: QT (MCP C03)
    // (private\ntos\recovery\recovery.cpp)
    //
    //DebugPrint("hwver: 0x%X\n", hwver);

    if(hwver <  0x40) return (char*)hwversions[1];
    if(hwver == 0x40) return (char*)hwversions[2];
    if(hwver == 0x46) return (char*)hwversions[3];
    if(hwver == 0x60) return (char*)hwversions[4];
    if(hwver == 0x70) return (char*)hwversions[5];
    return (char*)hwversions[0];
    }

static void GetPeripheral(char *buffer)
    {
    DWORD XIDMasks[] = {
        XDEVICE_PORT0_MASK,
        XDEVICE_PORT1_MASK,
        XDEVICE_PORT2_MASK,
        XDEVICE_PORT3_MASK
        };
    DWORD XMUMasks[] = {
        XDEVICE_PORT0_TOP_MASK,
        XDEVICE_PORT1_TOP_MASK,
        XDEVICE_PORT2_TOP_MASK,
        XDEVICE_PORT3_TOP_MASK,
        XDEVICE_PORT0_BOTTOM_MASK,
        XDEVICE_PORT1_BOTTOM_MASK,
        XDEVICE_PORT2_BOTTOM_MASK,
        XDEVICE_PORT3_BOTTOM_MASK
        };
    DWORD insertions, insertions2;
    unsigned i;
    unsigned port, slot;

    buffer[0] = '\0';

    bool dukeMask[4] = { false, false, false, false }; // Duke zeros bug

    // look for dukes
    insertions = XGetDevices(XDEVICE_TYPE_GAMEPAD);
    for(i=0; i<XGetPortCount(); i++)
        {
        port = i%XGetPortCount();
        if(insertions & XIDMasks[i])
            {
            buffer+=sprintf(buffer, "Duke(%d); ", port);
            dukeMask[port] = true;
            }
        }

    // check for mus
    insertions = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);
    for(i=0; i<ARRAYSIZE(XMUMasks); i++)
        {
        slot = i<XGetPortCount()?1:2;
        port = i%XGetPortCount();
        if(insertions & XMUMasks[i])
            {
            buffer+=sprintf(buffer, "MU(%d, %d); ", port, slot);
            }
        }

    /*
    // check for hawks
    insertions = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
    //insertions2 = XGetDevices(XDEVICE_TYPE_VOICE_HEADPHONE);
    for(i=0; i<ARRAYSIZE(XMUMasks); i++)
        {
        slot = i<XGetPortCount()?1:2;
        port = i%XGetPortCount();
        if(insertions & XMUMasks[i])
            {
            buffer+=sprintf(buffer, "Hawk(%d, %d); ", port, slot);
            }
        }
        */

    // look for dvd dongle
    /* 
    insertions = XGetDevices(XDEVICE_TYPE_IR_REMOTE);
    for(i=0; i<XGetPortCount(); i++)
        {
        port = i % XGetPortCount();
        if(insertions & XIDMasks[i])
            {
            buffer+=sprintf(buffer, "DVD(%d); ", port);
            }
        }
    */

    // look for keyboard
    #ifdef DEBUG_KEYBOARD
    insertions = XGetDevices(XDEVICE_TYPE_DEBUG_KEYBOARD);
    for(i=0; i<XGetPortCount(); i++)
        {
        port = i % XGetPortCount();
        if(insertions & XIDMasks[i])
            {
            buffer+=sprintf(buffer, "Keyboard(%d); ", port);
            }
        }
    #endif // DEBUG_KEYBOARD

    }


static void GetDriveID(char* device, char* model, char* serial, char* firmware)
    {
    char *temp;
    unsigned i;
    DWORD returned;
    NTSTATUS status;
    HANDLE fileHandle;
    OBJECT_ATTRIBUTES ObjA;
    OBJECT_STRING VolumeString;
    IO_STATUS_BLOCK IoStatusBlock;
    char buffer[sizeof(ATA_PASS_THROUGH) + 512];
    PATA_PASS_THROUGH atapt = (PATA_PASS_THROUGH)buffer;


    RtlInitObjectString(&VolumeString, device);
    InitializeObjectAttributes(&ObjA,&VolumeString,OBJ_CASE_INSENSITIVE,NULL,NULL);

    status = NtCreateFile(&fileHandle,
                            SYNCHRONIZE|GENERIC_READ,
                            &ObjA,
                            &IoStatusBlock,
                            0,
                            FILE_ATTRIBUTE_NORMAL,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN,
                            FILE_SYNCHRONOUS_IO_NONALERT);

    atapt->DataBufferSize = 512;
    atapt->DataBuffer = atapt + 1;

    atapt->IdeReg.bFeaturesReg     = 0;
    atapt->IdeReg.bSectorCountReg  = 0;
    atapt->IdeReg.bSectorNumberReg = 0;
    atapt->IdeReg.bCylLowReg       = 0;
    atapt->IdeReg.bCylHighReg      = 0;
    atapt->IdeReg.bDriveHeadReg    = 0;
    atapt->IdeReg.bHostSendsData   = 0;

    if(strstr(device, "CdRom") != NULL) atapt->IdeReg.bCommandReg = 0xa1;
    else atapt->IdeReg.bCommandReg = 0xec;

    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             FALSE);
    NtClose(fileHandle);

    PIDE_IDENTIFY_DATA IdData = (PIDE_IDENTIFY_DATA)atapt->DataBuffer;

    for ( i=0; i<sizeof(IdData->ModelNumber); i+=2 ) {
        model[i + 0] = IdData->ModelNumber[i + 1];
        model[i + 1] = IdData->ModelNumber[i + 0];
    }
    model[i] = '\0';
    temp = strstr(model, "  ");
    if(temp) *temp = '\0';

    for ( i=0; i<sizeof(IdData->SerialNumber); i+=2 ) {
        serial[i + 0] = IdData->SerialNumber[i + 1];
        serial[i + 1] = IdData->SerialNumber[i + 0];
    }
    serial[i] = '\0';
    temp = strstr(serial, "  ");
    if(temp) *temp = '\0';

    for ( i=0; i<sizeof(IdData->FirmwareRevision); i+=2 ) {
        firmware[i + 0] = IdData->FirmwareRevision[i + 1];
        firmware[i + 1] = IdData->FirmwareRevision[i + 0];
    }
    firmware[i] = '\0';
    temp = strstr(firmware, "  ");
    if(temp) *temp = '\0';
    }
