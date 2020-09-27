/*

Copyright (c) 2000  Microsoft Corporation

Module Name:

    stressmon.cpp

Abstract:

    The DLL exports two of the mandatory APIs: StartTest and EndTest.
    TESTINI.INI must be modified correctly so that the
    harness will load and call StartTest/EndTest.

    Please see "x-box test harness.doc" for more information.

Author:

    jpoley

Environment:

    XBox

Revision History:

*/


#include "stdafx.h"
#include <xtl.h>
#include <xbdm.h>
#include "StressMON.h"
#include "xtestlib.h"
#include "xnetref.h"

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntexapi.h>
#include <ntdddisk.h>
#include <ntddscsi.h>
#include <xbeimage.h>
extern "C"
    {
    #include <init.h>
    }
#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

#define ARRAYSIZE(a)        (sizeof(a) / sizeof(a[0]))


//
// Thread ID in multiple-thread configuration (not a Win32 thread ID)
//
// You can specify how many threads you want to run your test by
// setting the key NumberOfThreads=n under your DLL section. The
// default is only one thread.
//
static ULONG ThreadID = 0;

//
// Heap handle from HeapCreate
//
HANDLE StressMONHeapHandle;


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

FORCEINLINE ULONG
REG_RD32(VOID* Ptr, ULONG Addr)
    {
    return *((volatile DWORD*)((BYTE*)(Ptr) + (Addr)));
    }

namespace StressMON
    {
    DWORD stressMode = 0;

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
        "SDTV%20SCART%20Analog/Digital",
        "HDTV",
        "VGA",
        "RFU",
        "SDTV%20Analog/Digital",
        "Power%20Off",
        "SDTV%20Analog",
        "No%20Pack",
        "SMC%20Error"
        };

    static const char * const hwversions[] =
        {
        "Unknown",                  // 0
        "DVT3(old)",                // 1
        "DVT3(new)",                // 2
        "DVT4/5",                   // 3
        "DVT6",                     // 4
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

    static char* GetHWVersion(unsigned &hwver);
    static void DumpClocks(char *buffer, unsigned hwver);
    void GetPeripheral(char *buffer);
    void GetDriveID(char* device, char* model, char* serial, char* firmware);
    } // namespace StressMON


BOOL WINAPI StressMONDllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
    {
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(lpReserved);

    if(fdwReason == DLL_PROCESS_ATTACH)
        {
        StressMONHeapHandle = HeapCreate(0, 0, 0);

        if(!StressMONHeapHandle)
            {
            OutputDebugString(L"StressMON: Unable to create heap\n");
            }
        }
    else if(fdwReason == DLL_PROCESS_DETACH)
        {
        if(StressMONHeapHandle)
            {
            HeapDestroy(StressMONHeapHandle);
            }
        }

    return TRUE;
    }





/*

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    hLog - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

*/
VOID WINAPI StressMONStartTest(HANDLE hLog)
    {
    DWORD ID = 0;
    ID = InterlockedIncrement((LPLONG) &ThreadID);

    // Initialize the network
    unsigned short version = MAKEWORD(2, 2);
    WSADATA wsaData;
    XNetAddRef();
    Sleep(5000); // Because XNetStartup isnt fully started up when the API returns
    WSAStartup(version, &wsaData);

    // pull server IP from testini.ini
    char serverIP[32];
    memset(serverIP, 0, 32);
    GetProfileStringA("StressMON", "server", "172.26.172.13", serverIP, 32);


    // post settings
    unsigned baseInitTable = 0;
    bool multiple=false;
    char data[2048];
    char post[2048];
    char machineName[40];
    char machineIP[40];
    char peripherals[1024];
    char smcver[4];
    char amode[128];
    char vmode[128];
    char memtype[32];
    char hwflags[64];
    char *hwver;
    char clocks[255];
    char cdModel[64], cdSerial[64], cdFirmware[64];
    char hdModel[64], hdSerial[64], hdFirmware[64];

    while(1)
        {
        data[0] = '\0';
        post[0] = '\0';
        machineName[0] = '\0';
        machineIP[0] = '\0';
        peripherals[0] = '\0';
        smcver[0] = '\0';
        amode[0] = '\0';
        vmode[0] = '\0';
        cdModel[0] = '\0';
        cdSerial[0] = '\0';
        cdFirmware[0] = '\0';
        hdModel[0] = '\0';
        hdSerial[0] = '\0';
        hdFirmware[0] = '\0';
        hwflags[0] = '\0';
        clocks[0] = '\0';

        // get machine name 
        DWORD temp=40;
        ZeroMemory(machineName, 40);
        DmGetXboxName(machineName, &temp);

        // get IP address
        XNADDR xnaddr;
        XNetGetDebugXnAddr(&xnaddr);
        sprintf(machineIP, "%d.%d.%d.%d", xnaddr.ina.S_un.S_un_b.s_b1, xnaddr.ina.S_un.S_un_b.s_b2, xnaddr.ina.S_un.S_un_b.s_b3, xnaddr.ina.S_un.S_un_b.s_b4);

        // get SMC version
        smcver[0] = (char)StressMON::ReadSMC(0x01);
        smcver[1] = (char)StressMON::ReadSMC(0x01);
        smcver[2] = (char)StressMON::ReadSMC(0x01);
        smcver[3] = '\0';

        // hw version
        hwver = StressMON::GetHWVersion(baseInitTable);

        // get clock speeds
        StressMON::DumpClocks(clocks, baseInitTable);

        // get disk manufacturer
        StressMON::GetDriveID("\\Device\\CdRom0", cdModel, cdSerial, cdFirmware);
        StressMON::GetDriveID("\\Device\\Harddisk0\\Partition0", hdModel, hdSerial, hdFirmware);

        // get memory type
        #define NV_PEXTDEV_BOOT_0                                0x00101000 // private\windows\directx\dxg\d3d8\se\nv_ref_2a.h
        #define XPCICFG_GPU_MEMORY_REGISTER_BASE_0               0xFD000000 // private\ntos\inc\xpcicfg.h
        if((REG_RD32((void*)XPCICFG_GPU_MEMORY_REGISTER_BASE_0, NV_PEXTDEV_BOOT_0) & 0x0000C0000) == 0) sprintf(memtype, "Micron");
        else sprintf(memtype, "Samsung");

        // hw config
        if(XboxHardwareInfo->Flags & XBOX_HW_FLAG_INTERNAL_USB_HUB)
            strcat(hwflags, "USB%%20HUB");

        // get video settings
        DWORD avpack = StressMON::ReadSMC(0x04);
        if(avpack > 8) avpack = 8; // ReadSMC() can return 0xCCCCCCCC on error
        DWORD language = XGetLanguage();
        if(language >= ARRAYSIZE(StressMON::lang)) language = 0;
        DWORD vidStd = XGetVideoStandard();
        if(vidStd >= ARRAYSIZE(StressMON::videostd)) vidStd = 0;
        DWORD vidFlags = XGetVideoFlags();
        multiple=false;
        strcat(vmode, "(");
        if(vidFlags & XC_VIDEO_FLAGS_WIDESCREEN) multiple = !!strcat(vmode, "WIDE");
        if(multiple) multiple = !strcat(vmode, ",%%20");
        if(vidFlags & XC_VIDEO_FLAGS_HDTV_720p) multiple = !!strcat(vmode, "720p");
        if(multiple) multiple = !strcat(vmode, ",%%20");
        if(vidFlags & XC_VIDEO_FLAGS_HDTV_1080i) multiple = !!strcat(vmode, "1080i");
        if(multiple) multiple = !strcat(vmode, ",%%20");
        if(vidFlags & XC_VIDEO_FLAGS_LETTERBOX) multiple = !!strcat(vmode, "LETTERBOX");
        if(multiple) multiple = !strcat(vmode, ",%%20");
        if(vidFlags & XC_VIDEO_FLAGS_PAL_60Hz) multiple = !!strcat(vmode, "PAL60Hz");
        strcat(vmode, ")");

        // get audio settings
        DWORD audioFlags = XGetAudioFlags();
        multiple=false;
        if(audioFlags & XC_AUDIO_FLAGS_SURROUND) multiple = !!strcat(amode, "SURROUND");
        if(multiple) multiple = !strcat(amode, ",%%20");
        if(audioFlags & XC_AUDIO_FLAGS_STEREO) multiple = !!strcat(amode, "STEREO");
        if(multiple) multiple = !strcat(amode, ",%%20");
        if(audioFlags & XC_AUDIO_FLAGS_MONO) multiple = !!strcat(amode, "MONO");
        if(multiple) multiple = !strcat(amode, ",%%20");
        if(audioFlags & XC_AUDIO_FLAGS_ENABLE_AC3) multiple = !!strcat(amode, "AC3");
        if(multiple) multiple = !strcat(amode, ",%%20");
        if(audioFlags & XC_AUDIO_FLAGS_ENABLE_DTS) multiple = !!strcat(amode, "DTS");

        // get peripherals
        StressMON::GetPeripheral(peripherals);


        // Pull out internal INI file settings
        DebugPrint("STRESSMON: **************************************************************\n");
        DebugPrint("STRESSMON: server=%s\n", serverIP);

        // create socket \ connect to server
        SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
        SOCKADDR_IN dest;
        dest.sin_family = PF_INET;
        dest.sin_port = htons(80);
        dest.sin_addr.s_addr = inet_addr(serverIP);

        if(connect(sock, (SOCKADDR*)&dest, sizeof(SOCKADDR)) == SOCKET_ERROR)
            {
            DebugPrint("STRESSMON: connect error: (ec: %u)\n", GetLastError());
            }
        else
            {
            unsigned length, dataLen;

            length = sprintf(post, 
                            "machine=%s&"
                            "dbgIP=%s&"
                            "romver=%u.%s&"
                            "swver=%u.%s&"
                            "hwver=%s%%20%s,%%20SMC%%3D%s,%%20GPU%%3D%X,%%20MCP%%3D%X&"
                            "speed=%s&"
                            "hwcfg=MEM%%3D%s,%%20DVD%%3D%s(%s),%%20HD%%3D%s(%s),%%20Flags(%s)&"
                            "vmode=AVPACK%%3D%s,%%20LANG%%3D%s,%%20STD%%3D%s,%%20MODE%%3D%s&"
                            "amode=%s&"
                            "smc=CPUTemp%%3D%d,%%20AIRTemp%%3D%d,%%20Fan%%3D%d,%%20Error%%3D%d&"
                            "periph=%s",
                            machineName, 
                            machineIP, 
                            XboxKrnlVersion->Build, (!!(0x8000 & XboxKrnlVersion->Qfe))?"checked":"free",
                            XeImageHeader()->XapiLibraryVersion->BuildVersion, XeImageHeader()->XapiLibraryVersion->DebugBuild?"checked":"free",
                            hwver, (XboxHardwareInfo->Flags&XBOX_HW_FLAG_DEVKIT_KERNEL)?"DEVKIT":"RETAIL", smcver, XboxHardwareInfo->GpuRevision, XboxHardwareInfo->McpRevision, 
                            clocks,
                            memtype, cdModel, cdFirmware, hdModel, hdFirmware, hwflags,
                            StressMON::vmodes[avpack], StressMON::lang[language], StressMON::videostd[vidStd], vmode,
                            amode,
                            StressMON::ReadSMC(0x09), StressMON::ReadSMC(0x0A), StressMON::ReadSMC(0x10), StressMON::ReadSMC(0x0F),
                            peripherals);
            dataLen = sprintf(data, 
                            "POST /lab/submit/ping.asp HTTP/1.1\r\n"
                            "Accept: */*\r\n"
                            "User-Agent: StressMON\r\n"
                            "Connection: Close\r\n"
                            "Host: %s\r\n"
                            "Content-Length: %u\r\n"
                            "Content-Type: application/x-www-form-urlencoded\r\n"
                            "\r\n"
                            "%s",
                            serverIP,
                            length,
                            post);


            DWORD err = send(sock, data, dataLen, 0);
            //DebugPrint("STRESSMON: post: %s\n", post);
            //DebugPrint("STRESSMON: sent %u, (ec: %u)\n", err, GetLastError());
            //err = recv(sock, data, 256, 0);
            //data[err] = '\0';
            //DebugPrint("%s\n", data);

            shutdown(sock, SD_BOTH);
            }

        closesocket(sock);

        DWORD waitTime = 5*60*1000; // wait 5 min
        while(waitTime > 0)
            {
            StressMON::GetPeripheral(peripherals);
            waitTime -= 1000;
            Sleep(1000); // sleep 1 sec
            }
        }

    WSACleanup();
    XNetRelease();
    }


// TODO can remove this code when bug is fixed
void DukeZerosBug(void)
    {
    DebugPrint("\r\n\r\n\r\n\r\n");
    DebugPrint("*****************************************************************************\r\n");
    DebugPrint("Encountered Duke Zeros Bug: Please notify jeffalex\r\n");
    DebugPrint("*****************************************************************************\r\n");
    __asm int 3;
    }


void StressMON::DumpClocks(char *buffer, unsigned hwver)
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

        sprintf(buffer, "Crystal%%3D%s,%%20FSB%%3D%d,%%20CPU%%3D%d,%%20NVCLK%%3D%d,%%20VCO%%3D%d,%%20MCLK%%3D%d",
            (hwver >= 0x46) ? "16.6" : "13.5", fsbspeed, cpuspeed, nvclk, vcofreq, mclk);
        }
    else
        {
        sprintf(buffer, "unknown");
        }
    }

char* StressMON::GetHWVersion(unsigned &hwver)
    {
    #define PAGE_SIZE                   4096
    #define ROM_SIZE                    (1024 * 1024)
    #define ROM_VERSION_OFFSET          30 // In DWORDs (0x78 is the absolute offset)
    #define ROM_VERSION_BYTE_OFFSET     0x78
    #define FLASH_BASE_ADDRESS          0xFFF00000
    #define FLASH_REGION_SIZE           (0xFFFFFFFF-FLASH_BASE_ADDRESS-1)

    LPBYTE RomBase = (LPBYTE)MmMapIoSpace(FLASH_BASE_ADDRESS, PAGE_SIZE, PAGE_READWRITE | PAGE_NOCACHE);

    if(RomBase == NULL) return (char*)hwversions[0];

    hwver = RomBase[ROM_VERSION_BYTE_OFFSET];

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

void StressMON::GetPeripheral(char *buffer)
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
            buffer+=sprintf(buffer, "Duke(%d);%%20", port);
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
            buffer+=sprintf(buffer, "MU(%d,%%20%d);%%20", port, slot);

            // Duke zeros bug
            // TODO can remove this code when bug is fixed
            if(dukeMask[port] == false)
                {
                DukeZerosBug();
                }
            }
        }

    // check for hawks
    insertions = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
    //insertions2 = XGetDevices(XDEVICE_TYPE_VOICE_HEADPHONE);
    for(i=0; i<ARRAYSIZE(XMUMasks); i++)
        {
        slot = i<XGetPortCount()?1:2;
        port = i%XGetPortCount();
        if(insertions & XMUMasks[i])
            {
            buffer+=sprintf(buffer, "Hawk(%d,%%20%d);%%20", port, slot);
            }
        }

    // look for dvd dongle
    /* 
    insertions = XGetDevices(XDEVICE_TYPE_IR_REMOTE);
    for(i=0; i<XGetPortCount(); i++)
        {
        port = i % XGetPortCount();
        if(insertions & XIDMasks[i])
            {
            buffer+=sprintf(buffer, "DVD(%d);%%20", port);
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
            buffer+=sprintf(buffer, "Keyboard(%d);%%20", port);
            }
        }
    #endif // DEBUG_KEYBOARD

    }


void StressMON::GetDriveID(char* device, char* model, char* serial, char* firmware)
    {
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
                            FILE_SHARE_READ, FILE_OPEN,
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
    model[i] = 0;
    //DebugPrint("Model Number: %s\n", model);

    for ( i=0; i<sizeof(IdData->SerialNumber); i+=2 ) {
        serial[i + 0] = IdData->SerialNumber[i + 1];
        serial[i + 1] = IdData->SerialNumber[i + 0];
    }
    serial[i] = 0;
    //DebugPrint("Serial Number: %s\n", serial);

    for ( i=0; i<sizeof(IdData->FirmwareRevision); i+=2 ) {
        firmware[i + 0] = IdData->FirmwareRevision[i + 1];
        firmware[i + 1] = IdData->FirmwareRevision[i + 0];
    }
    firmware[i] = 0;
    //DebugPrint("Firmware Revision: %s\n", firmware);
    }



/*

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

*/
VOID WINAPI StressMONEndTest(void)
    {
    ThreadID = 0;
    }


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( stressmon )
#pragma data_seg()

BEGIN_EXPORT_TABLE( stressmon )
    EXPORT_TABLE_ENTRY( "StartTest", StressMONStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", StressMONEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", StressMONDllMain )
END_EXPORT_TABLE( stressmon )
