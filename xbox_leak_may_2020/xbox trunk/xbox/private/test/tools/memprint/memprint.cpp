/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    memprint.cpp

Abstract:

    Memory Foot Print

Notes:

*****************************************************************************/

#include "memprint.h"

#define SLEEPTIME   7000

HANDLE hDuke[XGetPortCount()];
HANDLE hSlot[XGetPortCount()*2];

#define LOOP_MAX    1024

bool warmBoot = false;

XDEVICE_PREALLOC_TYPE deviceTypes[] = { {XDEVICE_TYPE_GAMEPAD, 1}, {XDEVICE_TYPE_MEMORY_UNIT, 1} };
XNetStartupParams xnsp = { sizeof(XNetStartupParams), XNET_STARTUP_BYPASS_SECURITY };
WSADATA wsaData;
unsigned short version = MAKEWORD(2, 2);
HANDLE hTemp[LOOP_MAX];
DWORD dataType = 0xCDCDCDCD;
LAUNCH_DATA launchInfo;
char alternateDrive;
unsigned i;
IDirectSound *      g_dsound    = NULL;
IDirect3D8*		    g_d3d		= NULL;
IDirect3DDevice8*	g_pDevice	= NULL;
D3DPRESENT_PARAMETERS d3dpp;

MM_STATISTICS mmStats[2];
char mmStatsIndex = 0;
void GetMemoryStats(void)
    {
    memset(&mmStats[mmStatsIndex], 0, sizeof(MM_STATISTICS));
    mmStats[mmStatsIndex].Length = sizeof(MM_STATISTICS);
    MmQueryStatistics(&mmStats[mmStatsIndex]);

    mmStatsIndex = !mmStatsIndex;
    }

void LogMemory(MM_STATISTICS &m, char *note)
    {
    DebugPrint("%s Memory Status: %s\n", (warmBoot?"WARMBOOT:":"COLDBOOT:"), note);
    DebugPrint("   Total:       %12lu B (%d pages)\n", m.TotalPhysicalPages*PAGE_SIZE, m.TotalPhysicalPages);
    DebugPrint("   Available:   %12lu B (%d pages)\n", m.AvailablePages*PAGE_SIZE, m.AvailablePages);
    DebugPrint("   Cache:       %12lu B (%d pages)\n", m.CachePagesCommitted*PAGE_SIZE, m.CachePagesCommitted);
    DebugPrint("   Pool:        %12lu B (%d pages)\n", m.PoolPagesCommitted*PAGE_SIZE, m.PoolPagesCommitted);
    DebugPrint("   Stack:       %12lu B (%d pages)\n", m.StackPagesCommitted*PAGE_SIZE, m.StackPagesCommitted);
    DebugPrint("   Image:       %12lu B (%d pages)\n", m.ImagePagesCommitted*PAGE_SIZE, m.ImagePagesCommitted);
    }

void LogMemoryDifference(char *note)
    {
    GetMemoryStats();

    DebugPrint("%s Memory Consumption: %s\n", (warmBoot?"WARMBOOT:":"COLDBOOT:"), note);
    DebugPrint("   Avail Delta: %12ld B (%d pages)\n", mmStats[!mmStatsIndex].AvailablePages*PAGE_SIZE - mmStats[mmStatsIndex].AvailablePages*PAGE_SIZE, mmStats[!mmStatsIndex].AvailablePages - mmStats[mmStatsIndex].AvailablePages);
    DebugPrint("   Pool Delta:  %12ld B (%d pages)\n", mmStats[!mmStatsIndex].PoolPagesCommitted*PAGE_SIZE - mmStats[mmStatsIndex].PoolPagesCommitted*PAGE_SIZE, mmStats[!mmStatsIndex].PoolPagesCommitted - mmStats[mmStatsIndex].PoolPagesCommitted);
    }

#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

void _cdecl main(void)
    {
    //
    // memory snapshot at beginning of main
    //
    Sleep(SLEEPTIME);
    GetMemoryStats();

    //
    // XGetLaunchInfo
    //
    memset(&launchInfo, 0, sizeof(LAUNCH_DATA));
    XGetLaunchInfo(&dataType, &launchInfo);

    Sleep(SLEEPTIME);
    if(dataType == LDT_FROM_DASHBOARD || dataType == 0xCDCDCDCD)
        warmBoot = false;
    else // dataType == LDT_TITLE || LDT_FROM_DEBUGGER_CMDLINE
        warmBoot = true;

    DebugPrint("\n\nMEMPRINT\nKernel Version: %u.%s\nXTL Version: %u.%s\n", XboxKrnlVersion->Build, (!!(0x8000 & XboxKrnlVersion->Qfe))?"checked":"free", XeImageHeader()->XapiLibraryVersion->BuildVersion, XeImageHeader()->XapiLibraryVersion->DebugBuild?"devkit":"retail");
    LogMemory(mmStats[!mmStatsIndex], "main"); // log main info first

    DebugPrint("Dump of the memory manager db:\n");
    HexDump((const unsigned char*)0x83FF0000, 64*1024);

    LogMemoryDifference("XGetLaunchInfo()");

    //
    // initializing usb with 1 duke and 1 mu
    //
    XInitDevices(sizeof(deviceTypes) / sizeof(XDEVICE_PREALLOC_TYPE), deviceTypes);
    Sleep(SLEEPTIME);
    LogMemoryDifference("XInitDevice(1 duke 1 mu)");

    //
    // initialize D3D
    //
    g_d3d = Direct3DCreate8(D3D_SDK_VERSION);
    Sleep(SLEEPTIME);
    LogMemoryDifference("Direct3DCreate8(D3D_SDK_VERSION)");

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
    g_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pDevice);
    Sleep(SLEEPTIME);
    LogMemoryDifference("CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, ...)");

    //
    // initialize DSound
    //
    DirectSoundCreate(NULL, &g_dsound, NULL);
    Sleep(SLEEPTIME);
    LogMemoryDifference("DirectSoundCreate()");

    //
    // initialize network
    //
    XNetStartup(&xnsp);
    Sleep(SLEEPTIME);
    LogMemoryDifference("XNetStartup(XNET_STARTUP_BYPASS_SECURITY)");

    //
    // initialize network
    //
    WSAStartup(version, &wsaData);
    Sleep(SLEEPTIME);
    LogMemoryDifference("WSAStartup(2.2)");

    //
    // XMountUtilityDrive
    //
    XMountUtilityDrive(true);
    Sleep(SLEEPTIME);
    LogMemoryDifference("XMountUtilityDrive(true)");

    //
    // XMountAlternateTitle
    //
    /*
    XMountAlternateTitle("T:\\", 0xAAAACCCC, &alternateDrive);
    Sleep(SLEEPTIME);
    LogMemoryDifference("XMountAlternateTitle(T:\\, 0xAAAACCCC)");

    XUnmountAlternateTitle(alternateDrive);
    Sleep(SLEEPTIME);
    LogMemoryDifference("XUnmountAlternateTitle()");\
    */


    //
    // done
    //
    GetMemoryStats();
    LogMemory(mmStats[!mmStatsIndex], "end of test");
    if(!warmBoot)
        {
        // run the test again from a warm boot
        DebugPrint("\n\n\n");
        XLaunchNewImage("D:\\default.xbe", &launchInfo);
        }

    XLaunchNewImage("D:\\DoneWithTest.xbe", &launchInfo); // this causes us to go into the dash w/ an eject disk message
    }

