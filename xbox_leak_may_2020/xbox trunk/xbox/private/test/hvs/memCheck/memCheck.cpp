/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    memCheck.cpp

Abstract:

    Memory validator / scanner

Notes:

*****************************************************************************/

#include "memCheck.h"
#include <xbeimage.h>
#include <mm.h>
#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

#pragma intrinsic( memcpy )


HANDLE hDuke[XGetPortCount()] = { NULL, NULL, NULL, NULL };
HANDLE hSlot[XGetPortCount()*2] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

//
// config settings
//
ConfigSettings globalSettings;

//
// stats
//
DWORD dwNumTimesThrough = 0;
DWORD dwTestPassNumber = 1;
DWORD dwNumPass = 0;
DWORD dwNumFail = 0;
DWORD dwNumBuffMisMatch = 0;
unsigned __int64 ui64NumBlocksRead = 0;
unsigned __int64 ui64Time = ReadTSC();

//
// Info
//
WCHAR g_pwszMemType[50];
DWORD g_dwTotalMemory = 0;

void ParseTestConfig(char *variable, char *value, void *userParam)
{
    ConfigSettings* pSettings = (ConfigSettings*)userParam;

    if(strstr(variable, "blocksize") != NULL) pSettings->dwBlockSize = GetNumber(value);
    else if(strstr(variable, "startblock") != NULL) pSettings->dwStartBlock = GetNumber(value);
    else if(strstr(variable, "endblock") != NULL) pSettings->dwEndBlock = atol(value);
    else if(strstr(variable, "readmode") != NULL) pSettings->dwReadMode = GetNumber(value);
    else if(strstr(variable, "pauseonexit") != NULL) pSettings->dwPauseOnExit = GetNumber(value);
    else if(strstr(variable, "enablegraphics") != NULL) pSettings->dwEnableGraphics = GetNumber(value);
    else if(strstr(variable, "graphicsupdateinterval") != NULL) pSettings->dwGraphicsUpdateInterval = GetNumber(value);
    else if(strstr(variable, "enableusb") != NULL) pSettings->dwEnableUSB = GetNumber(value);
    else if(strstr(variable, "enablenetwork") != NULL) pSettings->dwEnableNetwork = GetNumber(value);
    else if(strstr(variable, "disableinterrupts") != NULL) pSettings->dwDisableInterrupts = GetNumber(value);

    // DebugPrint( "var - '%hs' = '%hs'\r\n", variable, value );
}

void _cdecl main(void)
{
    //
    // Get the launch data (if any)
    //
    DWORD dwDataType = 0xCDCDCDCD;
    LAUNCH_DATA launchInfo;
    XGetLaunchInfo( &dwDataType, &launchInfo );
    ToTestData* pInitialParams = (ToTestData*)&launchInfo;

    //
    // cleanup old files
    //
    DeleteFile("T:\\testexit.txt");

    DWORD dwAddDevice, dwRemoveDevice;

    char *iniConfig = NULL;

    //
    // Grab the startup parameters
    //
    if( pInitialParams->titleID == HVSLAUNCHERID )
    {
        iniConfig = new char[strlen(pInitialParams->configSettings)+1];
        strcpy(iniConfig, pInitialParams->configSettings);
        ParseConfigSettings( pInitialParams->configSettings, &globalSettings, ParseTestConfig, &globalSettings );
    }
    else
    {
        FILE* pFile = fopen("D:\\memCheck.ini", "rb");
        if( NULL != pFile )
        {
            DWORD dwFilesize = _filelength( _fileno( pFile ) );
            iniConfig = new char[dwFilesize+1];
            if( 1 != fread( iniConfig, dwFilesize, 1, pFile ) ) DebugPrint( "Failed to read the starup params!!\r\n" );
            iniConfig[dwFilesize] = '\0';

            ParseConfigSettings( iniConfig, &globalSettings, ParseTestConfig, &globalSettings );
            if( 0 != fseek( pFile, 0, SEEK_SET ) ) DebugPrint( "Failed to seek properly!!\r\n" );
            if( 1 != fread( iniConfig, dwFilesize, 1, pFile ) ) DebugPrint( "Failed to read the starup params!!\r\n" );
            iniConfig[dwFilesize] = '\0';
            fclose( pFile );
        }
    }

    // Call SetSaveName BEFORE calling LogPrint or LogFlush
    SetSaveName( globalSettings.saveName, "memCheck.log" );
    LogFlush();

    Initialize();

    if( pInitialParams->titleID == HVSLAUNCHERID )
        LogPrint( "Launched from HVS\n  Test %u of %u\n\n", pInitialParams->testNumber, pInitialParams->totalTests );
    else
        LogPrint( "Running in stand a lone mode\n\n" );

    LogPrint("Memory Check (Built on " __DATE__ " at " __TIME__ ")\n");
    LogPrint("Copyright (C) Microsoft Corporation.  All rights reserved.\n\n");


    //
    // allocate buffer space
    //
    unsigned char *buff1 = (unsigned char*) VirtualAlloc( NULL,
                                                          globalSettings.dwBlockSize,
                                                          MEM_COMMIT,
                                                          PAGE_READWRITE );

    unsigned char *buff2 = (unsigned char*) VirtualAlloc( NULL,
                                                          globalSettings.dwBlockSize,
                                                          MEM_COMMIT,
                                                          PAGE_READWRITE );

    DebugPrint( "Buffer 1 Address: %0.8X - %0.8X\n", buff1, buff1 + globalSettings.dwBlockSize );
    DebugPrint( "Buffer 2 Address: %0.8X - %0.8X\n\n", buff2, buff2 + globalSettings.dwBlockSize );

    if( !buff1 || !buff2 )
    {
        DebugPrint("\n\n\nUnable to allocate enough buffer space (%u)\n", globalSettings.dwBlockSize * 2);
        DebugPrint("Rebooting...\n\n\n");
        if(pInitialParams->titleID == HVSLAUNCHERID)
        {
            ToLauncherData* outParams = (ToLauncherData*)&launchInfo;
            outParams->titleID = XeImageHeader()->Certificate->TitleID;
            outParams->numPass = 0;
            outParams->numFail = 0;
            sprintf(outParams->notes, "Unable to allocate enough space (%u).\n", globalSettings.dwBlockSize * 2);

            LaunchImage("D:\\default.xbe", &launchInfo);
        }
        else
        {
            XLaunchNewImage(NULL, NULL);
        }
        
        return;
    }

    //
    // Allocate the needed memory
    //
    UCHAR* pucAllMemory = NULL;
    MEMORYSTATUS memStatus;
    ZeroMemory( &memStatus, sizeof( memStatus ) );

    GlobalMemoryStatus( &memStatus );

    DebugPrint( "Length              - '%d'\r\n", memStatus.dwLength );
    DebugPrint( "Memory Load         - '%d'\r\n", memStatus.dwMemoryLoad );
    DebugPrint( "Total Physical      - '%d'\r\n", memStatus.dwTotalPhys );
    DebugPrint( "Available Physical  - '%d'\r\n", memStatus.dwAvailPhys );
    DebugPrint( "Total Page File     - '%d'\r\n", memStatus.dwTotalPageFile );
    DebugPrint( "Available Page File - '%d'\r\n", memStatus.dwAvailPageFile );
    DebugPrint( "Total Virtual       - '%d'\r\n", memStatus.dwTotalVirtual );
    DebugPrint( "Available Virtual   - '%d'\r\n", memStatus.dwAvailVirtual );

    pucAllMemory = (UCHAR*)MmMapIoSpace( 0, memStatus.dwTotalPhys, PAGE_READWRITE | PAGE_NOCACHE );
    if( NULL == pucAllMemory )
    {
        DebugPrint("\n\n\nUnable to allocate '%d' bytes of memory!!\n", memStatus.dwAvailPhys );
        DebugPrint("Rebooting...\n\n\n");

        if( pInitialParams->titleID == HVSLAUNCHERID )
        {
            ToLauncherData* outParams = (ToLauncherData*)&launchInfo;
            outParams->titleID = XeImageHeader()->Certificate->TitleID;
            outParams->numPass = 0;
            outParams->numFail = 0;
            outParams->notes[0] = '\0';
            sprintf(outParams->notes, "Unable to allocate '%d' bytes of memory\n", memStatus.dwAvailPhys );

            LaunchImage("D:\\default.xbe", &launchInfo);
        }
        else
        {
            XLaunchNewImage(NULL, NULL);
        }
    }

    DebugPrint( "\n\n\nAllocated (%u) bytes of memory\n", memStatus.dwTotalPhys );

    //
    // Check to see if the user wants to test all memory, or if they specified to much memory
    //
    if( ( 0 == globalSettings.dwEndBlock ) || ( globalSettings.dwEndBlock > memStatus.dwTotalPhys ) )
        globalSettings.dwEndBlock = memStatus.dwTotalPhys;

    //
    // run the test
    //
    unsigned int uiCurrentMemAddress = globalSettings.dwStartBlock;
    UCHAR* pucCurrentLocation = pucAllMemory + globalSettings.dwStartBlock;

    // LogPrint("    (uiCurrentMemAddress) -- %08X\n", &uiCurrentMemAddress );

    DebugPrint( "Pass Number: %u\n", dwTestPassNumber );

    BOOL bSkipBuff1 = FALSE;
    BOOL bSkipBuff2 = FALSE;
    BOOL bUpdateGraphics = TRUE;
    DWORD dwAdjustedBlockSize = 0;
    while( TRUE )
    {
        if( globalSettings.dwEnableUSB )
        {
            XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwAddDevice, &dwRemoveDevice );
            InputDukeInsertions( dwAddDevice, dwRemoveDevice );
            XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, &dwAddDevice, &dwRemoveDevice );
            InputMUInsertions( dwAddDevice, dwRemoveDevice );
        }

        // Check to see if we should stop
        if( globalSettings.stopAfter.duration )
        {
            unsigned __int64 ui64ClockTick = ReadTSC() - ui64Time;

            if( ( globalSettings.stopAfter.type == STOPAFTER_ITERATIONS && dwNumTimesThrough >= globalSettings.stopAfter.duration ) ||
                ( globalSettings.stopAfter.type == STOPAFTER_MINUTES    && ( ( ui64ClockTick / 733000000UI64 ) / 60 ) >= globalSettings.stopAfter.duration ) ||
                ( globalSettings.stopAfter.type == STOPAFTER_HOURS      && ( ( ( ui64ClockTick / 733000000UI64 ) / 60 ) / 60 ) >= globalSettings.stopAfter.duration ) )
            {
                break;
            }
        }

        // Update our graphics every n seconds
        unsigned __int64 ui64ClockTick = ReadTSC() - ui64Time;
        if( ( 0 == ( ui64ClockTick / 733000000UI64 ) % globalSettings.dwGraphicsUpdateInterval ) && globalSettings.dwEnableGraphics )
        {
            if( bUpdateGraphics )
            {
                bUpdateGraphics = FALSE;
                InitGraphics();
                UpdateGraphics( DISPLAYMODE_RUN_TIME );
                g_pDevice->PersistDisplay();
                DeInitGraphics();
            }
        }
        else
        {
            bUpdateGraphics = TRUE;
        }

        if( globalSettings.dwEnableUSB )
        {
            if( InputCheckDigitalButton(XINPUT_GAMEPAD_START) ) GUI();

            if( InputCheckExitCombo() )
            {
                break;
            }
        }

        // Check to see if we have read all the memory specified
        if( uiCurrentMemAddress >= globalSettings.dwEndBlock )
        {
            dwNumTimesThrough++;
            dwTestPassNumber++;
            if( globalSettings.stopAfter.type == STOPAFTER_ITERATIONS && dwNumTimesThrough >= globalSettings.stopAfter.duration )
                dwTestPassNumber--;

            DebugPrint( "Pass Number: %u\n", dwTestPassNumber );

            //
            // Reset all of the parameters to start another loop
            //
            uiCurrentMemAddress = globalSettings.dwStartBlock;
            pucCurrentLocation = pucAllMemory + globalSettings.dwStartBlock;

            continue;
        }

        memset( buff1, 0, globalSettings.dwBlockSize );
        memset( buff2, 0, globalSettings.dwBlockSize );

        dwAdjustedBlockSize = globalSettings.dwBlockSize;

        //
        // Check to see if the memory read would read in to my buffers
        //
        if( ( ( pucCurrentLocation + dwAdjustedBlockSize ) >= buff1 ) &&
            ( ( pucCurrentLocation + dwAdjustedBlockSize ) < ( buff1 + dwAdjustedBlockSize ) ) )
        {
            bSkipBuff1 = TRUE;
            dwAdjustedBlockSize = buff1 - pucCurrentLocation;
        }
        else if( ( ( pucCurrentLocation + dwAdjustedBlockSize ) >= buff2 ) &&
                 ( ( pucCurrentLocation + dwAdjustedBlockSize ) < ( buff2 + dwAdjustedBlockSize ) ) )
        {
            bSkipBuff2 = TRUE;
            dwAdjustedBlockSize = buff2 - pucCurrentLocation;
        }

        //
        // Check to see if our memory read would extend past the end of our memory range
        //
        if( ( pucCurrentLocation + dwAdjustedBlockSize ) > ( pucAllMemory + globalSettings.dwEndBlock ) )
        {
            dwAdjustedBlockSize = ( pucAllMemory + globalSettings.dwEndBlock ) - pucCurrentLocation;
            DebugPrint( "Adjusted Block Size = %u\n", dwAdjustedBlockSize );
        }

        // Disable interrupts if necessary
        if( globalSettings.dwDisableInterrupts )
            __asm cli

        //
        // read from memory
        //
        memcpy( buff1, pucCurrentLocation, dwAdjustedBlockSize );
        memcpy( buff2, pucCurrentLocation, dwAdjustedBlockSize );

        /*
        // This will introduce errors so that we can verify the test is working as expected
        unsigned __int64 ui64ClockTick = ReadTSC() - ui64Time;
        if( 0 == ( ui64ClockTick / 733000000UI64 ) % 15 )
            buff2[418] = ~buff1[418];
        */

        //
        // Compare the buffers to make sure they match
        //
        if( !CompareBuff( buff1, buff2, dwAdjustedBlockSize ) )
        {
            LogPrint("    (Read) Mem Location -- %08X\n", pucCurrentLocation );
            // _asm { int 3 }
            // DebugPrint( "\n\nBuffers did not match!!\r\n" );
            // HexDump( buff1, dwAdjustedBlockSize );
            // HexDump( buff2, dwAdjustedBlockSize );
            dwNumFail++;
        }
        else
        {
            //
            // Check to see if the user wants to test writing
            //
            if( globalSettings.dwReadMode == 1 ) // 1 = read only, so we should log a pass
            {
                dwNumPass++;
            }
            else
            {
                //
                // Write back to the same memory
                //
                memcpy( pucCurrentLocation, buff1, dwAdjustedBlockSize );

                //
                // Re-read from the newly written memory
                //
                memcpy( buff1, pucCurrentLocation, dwAdjustedBlockSize );

                //
                // Compare the buffers again
                //
                if( !CompareBuff( buff1, buff2, dwAdjustedBlockSize ) )
                {
                    LogPrint("    (Write) Mem Location -- %08X\n", pucCurrentLocation );
                    // DebugPrint( "\n\nBuffers did not match!!\r\n" );
                    // HexDump( buff1, dwAdjustedBlockSize );
                    // HexDump( buff2, dwAdjustedBlockSize );
                    dwNumFail++;
                }
                else
                {
                    dwNumPass++;
                }
            }
        }

        // Enable interrupts if necessary
        if( globalSettings.dwDisableInterrupts )
            __asm sti


        // DebugPrint( "Current Loc - '0x%0.8X'\r\n", pucCurrentLocation );
        
        ui64NumBlocksRead++;
        if( ( bSkipBuff1 ) || ( bSkipBuff2 ) )
        {
            bSkipBuff1 = FALSE;
            bSkipBuff2 = FALSE;
            
            pucCurrentLocation += dwAdjustedBlockSize + globalSettings.dwBlockSize;
            uiCurrentMemAddress += dwAdjustedBlockSize + globalSettings.dwBlockSize;
        }
        else
        {
            pucCurrentLocation += dwAdjustedBlockSize;
            uiCurrentMemAddress += dwAdjustedBlockSize;
        }
    } // While

    if( globalSettings.dwEnableGraphics )
    {
        InitGraphics();
        UpdateGraphics( DISPLAYMODE_RUN_TIME );
        g_pDevice->PersistDisplay();
    }

    LogPrint( "\nNumber of failures - '%u'\n", dwNumFail );
    LogPrint( "Number of passes   - '%u'\n", dwNumPass );

    //
    // Post results if necessary
    //
    if( ( globalSettings.dwEnableNetwork ) && ( strncmp(globalSettings.resultsServer, "http://", 7) == 0 ) )
    {
        //
        // Initialize the Network
        //
        XNetStartupParams xnsp = { sizeof(XNetStartupParams), XNET_STARTUP_BYPASS_SECURITY };
        XNetStartup(&xnsp);

        //
        // Initialize USB if it hasn't been already
        //
        if( !globalSettings.dwEnableUSB )
            XInitDevices(0, NULL);


        WSADATA wsaData;
        DBDATA results;
        unsigned short version = MAKEWORD(2, 2);
        WSAStartup( version, &wsaData );

        results.deviceType = "Memory";
        char pszMemType[MAX_PATH+1];
        pszMemType[MAX_PATH] = '\0';
        _snprintf( pszMemType, MAX_PATH, "%ls", g_pwszMemType );
        results.manufacturer = RightTrim( pszMemType );
        results.firmwareVersion = "";
        results.hardwareVersion = "";
        results.serialNumber = "";
        results.testName = "Memory Check";

        char variation[256];
        results.boxInformation = GetBoxInformation();
        results.configSettings = iniConfig;
        results.variation = variation;

        sprintf( variation, "Block Size: %u, Read Mode: %u, Start Block: %u, End Block: %u", globalSettings.dwBlockSize, globalSettings.dwReadMode, globalSettings.dwStartBlock, globalSettings.dwEndBlock );

        // TODO: Different results for READ & READ/WRITE tests
        // failures (read/write errors)
        results.status = 0;
        results.numOperations = dwNumFail;
        results.notes = "Read/Write Errors";
        if( results.numOperations )
            PostResults( globalSettings.resultsServer, &results );

        // passes
        results.status = 1;
        results.numOperations = dwNumPass;
        results.notes = "Successful Reads/Writes";
        if( results.numOperations )
            PostResults( globalSettings.resultsServer, &results );

        // cleanup
        delete[] results.boxInformation;
    }

    // Cleanup
    if( iniConfig != NULL ) delete[] iniConfig;
    if( buff1 != NULL ) VirtualFree( buff1, 0, MEM_RELEASE);
    if( buff2 != NULL ) VirtualFree( buff2, 0, MEM_RELEASE);
    if( pucAllMemory != NULL ) MmUnmapIoSpace( pucAllMemory, memStatus.dwTotalPhys );

    if( globalSettings.dwEnableGraphics )
    {
        if( globalSettings.dwPauseOnExit )
            Pause();
        else
            g_pDevice->PersistDisplay();            // persist the display across the reboot
    }

    //
    // test done, reboot
    //
    if( pInitialParams->titleID == HVSLAUNCHERID )
    {
        ToLauncherData* outParams = (ToLauncherData*)&launchInfo;
        outParams->titleID = XeImageHeader()->Certificate->TitleID;
        outParams->numPass = dwNumPass;
        outParams->numFail = dwNumFail;
        outParams->notes[0] = '\0';

        LaunchImage("D:\\default.xbe", &launchInfo);
    }
    else
        XLaunchNewImage(NULL, NULL);
}

FORCEINLINE ULONG
REG_RD32(VOID* Ptr, ULONG Addr)
{
    return *((volatile DWORD*)((BYTE*)(Ptr) + (Addr)));
}

void Initialize( void )
{
    //
    // USB
    //
    if( globalSettings.dwEnableUSB )
        XInitDevices(0, NULL);

    //
    // Network
    //
    /*
    if( globalSettings.dwEnableNetwork )
    {
        XNetStartupParams xnsp = { sizeof(XNetStartupParams), XNET_STARTUP_BYPASS_SECURITY };
        XNetStartup(&xnsp);
    }
    */

    //
    // Graphics
    //
    /*
    if( globalSettings.dwEnableGraphics )
        InitGraphics();
    */

    //
    // Memory Type
    //
    #define NV_PEXTDEV_BOOT_0                   0x00101000 // private\windows\directx\dxg\d3d8\se\nv_ref_2a.h
    #define XPCICFG_GPU_MEMORY_REGISTER_BASE_0  0xFD000000 // private\ntos\inc\xpcicfg.h

    // Get the total amount of memory available
    MEMORYSTATUS memStatus;

    ZeroMemory( &memStatus, sizeof( memStatus ) );
    ZeroMemory( g_pwszMemType, sizeof( WCHAR ) * 50 );

    GlobalMemoryStatus( &memStatus );

    g_dwTotalMemory = memStatus.dwTotalPhys / ( 1024 * 1024 );  // In Megabytes

    if( ( REG_RD32( (void*)XPCICFG_GPU_MEMORY_REGISTER_BASE_0, NV_PEXTDEV_BOOT_0 ) & 0x0000C0000 ) == 0 )
        _snwprintf( g_pwszMemType, 49, L"Micron" );
    else
        _snwprintf( g_pwszMemType, 49, L"Samsung" );

    // Update Log with possible failure info
    // Check to see if interrupts are disabled
    if( !globalSettings.dwDisableInterrupts )
        LogPrint( "Interrupts are enabled; failures are possible!!\n" );

    if( DmGetCurrentDmi() != NULL )
        LogPrint( "XBDM is enabled; failures are possible!!\n" );

    if( globalSettings.dwEnableUSB)
        LogPrint( "USB is enabled; failures are possible!!\n" );
}


