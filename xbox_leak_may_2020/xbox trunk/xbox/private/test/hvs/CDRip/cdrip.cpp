/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    CDRip.cpp

Abstract:

    Audio CD Ripping program

Notes:

*****************************************************************************/

#include "cdrip.h"
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
DWORD g_dwTrackRipStartTime = 0;
DWORD g_dwTrackRipEndTime = 0;
DWORD g_dwTotalRipTime = 0;
DWORD g_dwNumTimesThrough = 0;
DWORD g_dwTestPassNumber = 1;
DWORD g_dwNumPass = 0;
DWORD g_dwNumFail = 0;
DWORD g_dwNumTracksRipped = 0;
DWORD g_dwTime = 0;
DWORD g_dwCurrentTrack = 0;
float g_fMegabytesRipped = 0.0f;
char g_pszModel[64], g_pszSerial[64], g_pszFirmware[64];

//
// Globals
//
CNtIoctlCdromService g_DVDRom;
BYTE* g_pbyTrackBuffer = NULL;
LAUNCH_DATA g_launchInfo;
ToTestData* g_pInitialParams = NULL;
HANDLE g_hThread = NULL;
CRITICAL_SECTION g_CritSect;
BOOL g_bExitThread = FALSE;             // Used to signal our thread to exit
DWORD* g_adwTracks = NULL;
char* g_iniConfig = NULL;

void ParseTestConfig( char *variable, char *value, void *userParam )
{
    ConfigSettings* pSettings = (ConfigSettings*)userParam;

    if(strstr(variable, "cdtitle") != NULL)             strcpy(pSettings->pszCDTitle, value);
    else if(strstr(variable, "cdserial") != NULL)       strcpy(pSettings->pszCDSerial, value);
    else if(strstr(variable, "starttrack") != NULL)     pSettings->dwStartTrack = GetNumber(value);
    else if(strstr(variable, "endtrack") != NULL)       pSettings->dwEndTrack = GetNumber(value);
    else if(strstr(variable, "ripsequence") != NULL)    pSettings->dwRipSequence = GetNumber(value);
    else if(strstr(variable, "randseed") != NULL)       pSettings->dwRandSeed = GetNumber(value);
    else if(strstr(variable, "pauseonexit") != NULL)    pSettings->dwPauseOnExit = GetNumber(value);

    // DebugPrint( "var - '%hs' = '%hs'\r\n", variable, value );
}

void _cdecl main(void)
{
    // TODO: Calculate the MBits and MBits per second

    //
    // Get the launch data (if any)
    //
    DWORD dwDataType = 0xCDCDCDCD;
    XGetLaunchInfo( &dwDataType, &g_launchInfo );
    g_pInitialParams = (ToTestData*)&g_launchInfo;

    //
    // cleanup old files
    //
    DeleteFile("T:\\testexit.txt");

    DWORD dwAddDevice, dwRemoveDevice;

    //
    // Grab the startup parameters
    //
    if( g_pInitialParams->titleID == HVSLAUNCHERID )
    {
        g_iniConfig = new char[strlen(g_pInitialParams->configSettings)+1];
        strcpy(g_iniConfig, g_pInitialParams->configSettings);
        ParseConfigSettings( g_pInitialParams->configSettings, &globalSettings, ParseTestConfig, &globalSettings );
    }
    else
    {
        FILE* pFile = fopen("D:\\CDRip.ini", "rb");
        if( NULL != pFile )
        {
            DWORD dwFilesize = _filelength( _fileno( pFile ) );
            g_iniConfig = new char[dwFilesize+1];
            if( 1 != fread( g_iniConfig, dwFilesize, 1, pFile ) ) DebugPrint( "Failed to read the starup params!!\r\n" );
            g_iniConfig[dwFilesize] = '\0';

            ParseConfigSettings( g_iniConfig, &globalSettings, ParseTestConfig, &globalSettings );
            if( 0 != fseek( pFile, 0, SEEK_SET ) ) DebugPrint( "Failed to seek properly!!\r\n" );
            if( 1 != fread( g_iniConfig, dwFilesize, 1, pFile ) ) DebugPrint( "Failed to read the starup params!!\r\n" );
            g_iniConfig[dwFilesize] = '\0';
            fclose( pFile );
        }
    }

    // Call SetSaveName BEFORE calling LogPrint or LogFlush
    SetSaveName( globalSettings.saveName, "CDRip.log" );
    LogFlush();

    Initialize();

    if( g_pInitialParams->titleID == HVSLAUNCHERID )
        LogPrint( "Launched from HVS\n  Test %u of %u\n\n", g_pInitialParams->testNumber, g_pInitialParams->totalTests );
    else
        LogPrint( "Running in stand a lone mode\n\n" );

    LogPrint("CD Rip (Built on " __DATE__ " at " __TIME__ ")\n");
    LogPrint("Copyright (C) Microsoft Corporation.  All rights reserved.\n\n");

    // Make sure the proper type of disc is in the drive
    int nDiscType = GetDiscType();
    if( DISC_AUDIO != nDiscType )
    {
        LogPrint( "Invalid Disc Type.  Must be CD Audio Disc!! - '%d'\n", nDiscType );
        CleanUp();
        RebootToShell( 0, 1, "Invalid Media Type Detected\\nMake sure an Audio CD is in the DVD Drive before\\nyou launch the test\n" );
    }

    //
    // Open the DVD / CD Drive
    //
    if( FAILED( g_DVDRom.Open() ) )
    {
        LogPrint( "Unable to open the DVD Drive!!\n" );
        CleanUp();
        RebootToShell( 0, 1, "Unable to open the DVD Drive!!\n" );
    }

    XCDROM_TOC* pTOC = g_DVDRom.GetTOC();
    // Debug Print the TOC
    DebugPrint( "TOC\n" );
    for( int iToc = 0; iToc <= pTOC->LastTrack; iToc++ )
        DebugPrint( "  Track %03d Address: 0x%08X\n", iToc, pTOC->TrackAddr[iToc] );

    // Check if our user wants to set the last track to the end of the CD, or if they specified an invalid last track
    if( ( 0 == globalSettings.dwEndTrack ) || ( (int)globalSettings.dwEndTrack > g_DVDRom.GetTOC()->LastTrack ) )
        globalSettings.dwEndTrack = g_DVDRom.GetTOC()->LastTrack;

    // Check to see if the first track is greater than the last track
    if( globalSettings.dwStartTrack > globalSettings.dwEndTrack )
    {
        LogPrint( "Start Track (%u) was greater than End Track (%u), setting Start to End\n", globalSettings.dwStartTrack, globalSettings.dwEndTrack );
        globalSettings.dwStartTrack = globalSettings.dwEndTrack;
    }

    // Set our random seed
    srand( globalSettings.dwRandSeed );

    // Setup our Track Index
    DWORD dwNumTracks = globalSettings.dwEndTrack - globalSettings.dwStartTrack + 1;
    g_adwTracks = new DWORD[dwNumTracks];
    unsigned int uiCount = 0;
    for( unsigned int x = globalSettings.dwStartTrack; x <= globalSettings.dwEndTrack; x++ )
    {
        g_adwTracks[uiCount] = x;
        uiCount++;
    }

    // Handle random mode
    if( 1 == globalSettings.dwRipSequence )     // 0 = Sequential, 1 = Random
        RandomizeTracks( g_adwTracks );

    // Set our current track to the first track in our list
    SetCurrentTrack( 0 );

    //
    // print out the drive info, disk geometry, and config settings
    //
    ZeroMemory( g_pszModel, 64 );
    ZeroMemory( g_pszSerial, 64 );
    ZeroMemory( g_pszFirmware, 64 );
    GetDriveID( g_DVDRom.GetDeviceHandle(), TRUE, g_pszModel, g_pszSerial, g_pszFirmware );

	LogPrint("DRIVE: Model:             %s\n", g_pszModel );
	LogPrint("DRIVE: Serial:            %s\n", g_pszSerial );
	LogPrint("DRIVE: Firmware:          %s\n", g_pszFirmware );

    //
    // allocate buffer space
    //
    g_pbyTrackBuffer = new BYTE[BLOCK_SIZE];
    if( NULL == g_pbyTrackBuffer )
    {
        DebugPrint( "\n\n\nUnable to allocate enough buffer space (%u)\n", BLOCK_SIZE );
        DebugPrint( "Rebooting...\n\n\n" );
        CleanUp();
        RebootToShell( 0, 1, "Unable to allocate enough buffer space!!\n" );

        return;
    }

    g_dwTime = GetTickCount();

    //
    // run the test
    //
    DebugPrint( "Pass Number: %u\n", g_dwTestPassNumber );

    while( TRUE )
    {
        XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwAddDevice, &dwRemoveDevice );
        InputDukeInsertions( dwAddDevice, dwRemoveDevice );
        XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, &dwAddDevice, &dwRemoveDevice );
        InputMUInsertions( dwAddDevice, dwRemoveDevice );
        UpdateGraphics( DISPLAYMODE_RUN_TIME );

        // Check to see if we should stop
        if( globalSettings.stopAfter.duration )
        {
            if( ( globalSettings.stopAfter.type == STOPAFTER_ITERATIONS && g_dwNumTimesThrough >= globalSettings.stopAfter.duration ) ||
                ( globalSettings.stopAfter.type == STOPAFTER_MINUTES    && TICK2MIN( GetTickCount() - g_dwTime ) >= globalSettings.stopAfter.duration ) ||
                ( globalSettings.stopAfter.type == STOPAFTER_HOURS      && TICK2HOUR( GetTickCount() - g_dwTime ) >= globalSettings.stopAfter.duration ) )
            {
                break;
            }
        }

        if( InputCheckDigitalButton( XINPUT_GAMEPAD_START ) ) GUI();

        if( InputCheckExitCombo() )
            break;

        DWORD dwThreadStatus;
        GetExitCodeThread( g_hThread, &dwThreadStatus );

        if( ( STILL_ACTIVE != dwThreadStatus ) && 
            ( GetCurrentTrack() < dwNumTracks ) )
        {
            // Check for Copy Error

            // Close our thread handle if it's still open
            if( NULL != g_hThread )
            {
                CloseHandle( g_hThread );
                g_hThread = NULL;
            }
            
            DWORD dwCurrentTrack = GetCurrentTrack();

            // Start our thread copying a track
            g_hThread = CreateThread( NULL,
                                    0,
                                    AudioTrackCopyThread,
                                    &g_adwTracks[dwCurrentTrack],
                                    0,
                                    NULL );

            if( NULL == g_hThread )
            {
                DebugPrint( "Unable to create our thread!! Error - '0x%08X (%d)'\n", GetLastError(), GetLastError() );
                break;
            }
        }

        // Check to see if we have ripped all of the tracks necessary
        if( GetCurrentTrack() >= dwNumTracks )
        {
            g_dwNumTimesThrough++;
            g_dwTestPassNumber++;
            if( globalSettings.stopAfter.type == STOPAFTER_ITERATIONS && g_dwNumTimesThrough >= globalSettings.stopAfter.duration )
                g_dwTestPassNumber--;

            DebugPrint( "Pass Number: %u\n", g_dwTestPassNumber );

            //
            // Reset all of the parameters to start another loop
            //

            // Handle random mode
            if( 1 == globalSettings.dwRipSequence )     // 0 = Sequential, 1 = Random
                RandomizeTracks( g_adwTracks );
           
            SetCurrentTrack( 0 );

            continue;
        }
    } // While

    UpdateGraphics( DISPLAYMODE_RUN_TIME );

    LogPrint( "\nNumber of failures - '%u'\n", g_dwNumFail );
    LogPrint( "Number of passes   - '%u'\n", g_dwNumPass );

    // Figure out the test notes
    char pszNotes[MAX_PATH+1];
    pszNotes[MAX_PATH] = '\0';

    sprintf( pszNotes, "CD Title: %hs\\nCD Serial: %hs\\nRip Sequence: %hs\\nTotal Tracks Ripped: %u\\nTotal Time to Rip Tracks: %u seconds\\nTotal Megabytes Ripped: %0.2f\\nMegabytes Ripped per second: %0.2f\n", globalSettings.pszCDTitle, globalSettings.pszCDSerial, globalSettings.dwRipSequence?"Random":"Sequential", GetTracksRipped(), GetTotalRipTime() / 1000, GetMegabytesRipped(), GetMegabytesRippedPerSecond() );

    //
    // Post results if necessary
    //
    if( strncmp( globalSettings.resultsServer, "http://", 7 ) == 0 )
    {
        WSADATA wsaData;
        DBDATA results;
        unsigned short version = MAKEWORD(2, 2);
        WSAStartup( version, &wsaData );

        results.deviceType = "DVD";
        results.manufacturer = RightTrim( g_pszModel );
        results.firmwareVersion = RightTrim( g_pszFirmware );
        results.hardwareVersion = "";
        results.serialNumber = RightTrim( g_pszSerial );
        results.testName = "CD Rip";

        results.boxInformation = GetBoxInformation();
        results.configSettings = g_iniConfig;
        char variation[256];
        results.variation = variation;
        sprintf(variation, "Rip Sequence: %hs", globalSettings.dwRipSequence?"Random":"Sequential" );

        // passes
        results.status = 1;
        results.numOperations = g_dwNumPass;
        results.notes = pszNotes;
        PostResults( globalSettings.resultsServer, &results );

        // cleanup
        delete[] results.boxInformation;
    }

    // Cleanup
    CleanUp();

    // Close the drive
    g_DVDRom.Close();

    if( globalSettings.dwPauseOnExit )
        Pause();
    else
        g_pDevice->PersistDisplay();            // persist the display across the reboot

    //
    // test done, reboot
    //
    g_dwNumPass++;
    RebootToShell( g_dwNumPass, g_dwNumFail, pszNotes );
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
    XInitDevices(0, NULL);

    //
    // Network
    //
    XNetStartupParams xnsp = { sizeof(XNetStartupParams), XNET_STARTUP_BYPASS_SECURITY };
    XNetStartup(&xnsp);

    //
    // Graphics
    //
    InitGraphics();

    //
    // Thread
    //
    ZeroMemory( &g_CritSect, sizeof( g_CritSect ) );
    InitializeCriticalSection( &g_CritSect );
}

void CleanUp( void )
{
    // Check to see if our thread is still active.  If so, end it
    DWORD dwThreadStatus;
    GetExitCodeThread( g_hThread, &dwThreadStatus );

    if( STILL_ACTIVE == dwThreadStatus )
    {
        SetExitThread( TRUE );

        // Wait 5 seconds for the thread to exit
        DWORD dwWaitStatus = WaitForSingleObject( g_hThread, INFINITE );
        if( WAIT_FAILED == dwWaitStatus )
            DebugPrint( "CleanUp():WaitForSingleObject Failed!! - '0x%08X (%d)'\n", GetLastError(), GetLastError() );
    }

    // Close our thread handle if it's still open
    if( NULL != g_hThread )
    {
        CloseHandle( g_hThread );
        g_hThread = NULL;
    }

    DeleteCriticalSection( &g_CritSect );

    // Clean up any allocated memory
    if( NULL != g_adwTracks ) delete[] g_adwTracks;
    g_adwTracks = NULL;

    if( g_iniConfig != NULL ) delete[] g_iniConfig;
    g_iniConfig = NULL;

    if( NULL != g_pbyTrackBuffer ) delete[] g_pbyTrackBuffer;
    g_pbyTrackBuffer = NULL;
}


// Used to reboot to our launching program
void RebootToShell( DWORD dwNumPassed, DWORD dwNumFailed, char* pszNotes )
{
    if(g_pInitialParams->titleID == HVSLAUNCHERID)
    {
        ToLauncherData* outParams = (ToLauncherData*)&g_launchInfo;
        outParams->titleID = XeImageHeader()->Certificate->TitleID;
        outParams->numPass = dwNumPassed;
        outParams->numFail = dwNumFailed;
        strcpy( outParams->notes, pszNotes );

        LogPrint( pszNotes );

        LaunchImage("D:\\default.xbe", &g_launchInfo);
    }
    else
    {
        XLaunchNewImage(NULL, NULL);
    }
}

void RandomizeTracks( DWORD* adwTracks )
{
    DWORD dwNumTracks = globalSettings.dwEndTrack - globalSettings.dwStartTrack + 1;
    for( unsigned int x = 0; x < dwNumTracks; x++ )
    {
        int iRand = rand() % dwNumTracks;

        DWORD dwTemp = adwTracks[x];
        adwTracks[x] = adwTracks[iRand];
        adwTracks[iRand] = dwTemp;
    }

    // TODO: DEBUG -- REMOVE
    DebugPrint( "Track Order\n" );
    for( x = 0; x < dwNumTracks; x++ )
        DebugPrint( "  Track #%d\n", g_adwTracks[x] );
}

//////
// Thread Functions
//////
DWORD GetCurrentTrack( void )
{
    DWORD dwReturn = 0;
    EnterCriticalSection( &g_CritSect );
    dwReturn = g_dwCurrentTrack;
    LeaveCriticalSection( &g_CritSect );

    return dwReturn;
}

void SetCurrentTrack( DWORD dwCurrentTrack )
{
    EnterCriticalSection( &g_CritSect );
    g_dwCurrentTrack = dwCurrentTrack;
    LeaveCriticalSection( &g_CritSect );
}

void IncCurrentTrack( void )
{
    EnterCriticalSection( &g_CritSect );
    g_dwCurrentTrack++;
    LeaveCriticalSection( &g_CritSect );
}


DWORD GetTracksRipped( void )
{
    DWORD dwReturn = 0;
    EnterCriticalSection( &g_CritSect );
    dwReturn = g_dwNumTracksRipped;
    LeaveCriticalSection( &g_CritSect );

    return dwReturn;
}

void SetTracksRipped( DWORD dwTracksRipped )
{
    EnterCriticalSection( &g_CritSect );
    g_dwNumTracksRipped = dwTracksRipped;
    LeaveCriticalSection( &g_CritSect );
}

void IncTracksRipped( void )
{
    EnterCriticalSection( &g_CritSect );
    g_dwNumTracksRipped++;
    LeaveCriticalSection( &g_CritSect );
}

BOOL GetExitThread( void )
{
    BOOL bReturn;
    EnterCriticalSection( &g_CritSect );
    bReturn = g_bExitThread;
    LeaveCriticalSection( &g_CritSect );

    return bReturn;
}

void SetExitThread( BOOL bThreadExit )
{
    EnterCriticalSection( &g_CritSect );
    g_bExitThread = bThreadExit;
    LeaveCriticalSection( &g_CritSect );
}


DWORD GetTotalRipTime( void )
{
    DWORD dwReturn;

    EnterCriticalSection( &g_CritSect );
    dwReturn = g_dwTotalRipTime;
    LeaveCriticalSection( &g_CritSect );

    return dwReturn;
}

void SetTotalRipTime( DWORD dwRipTime )
{
    EnterCriticalSection( &g_CritSect );
    g_dwTotalRipTime = dwRipTime;
    LeaveCriticalSection( &g_CritSect );
}


float GetMegabytesRipped( void )
{
    float fReturn;

    EnterCriticalSection( &g_CritSect );
    fReturn = g_fMegabytesRipped;
    LeaveCriticalSection( &g_CritSect );

    return fReturn;
}

void SetMegabytesRipped( float fMegabytesRipped )
{
    EnterCriticalSection( &g_CritSect );
    g_fMegabytesRipped = fMegabytesRipped;
    LeaveCriticalSection( &g_CritSect );
}


float GetMegabytesRippedPerSecond()
{
    float fMegabytesRipped = GetMegabytesRipped();
    
    // Let's make sure we don't divide by zero
    if( ( 0 == fMegabytesRipped ) || ( 0 == GetTotalRipTime() ) )
        return 0;

    return( fMegabytesRipped / (float)( GetTotalRipTime() / 1000 ) );

}