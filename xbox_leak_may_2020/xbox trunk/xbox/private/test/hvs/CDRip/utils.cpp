/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    utils.cpp

Abstract:

    Helper functions

Notes:

*****************************************************************************/

#include "cdrip.h"

BOOL CompareBuff( unsigned char *buff1, unsigned char *buff2, unsigned length )
{
    BOOL bReturn = TRUE;

    for(unsigned i=0; i<length; i++)
    {
        if(buff1[i] != buff2[i])
        {
            // LogPrint("    %u: %02X %02X\n", i, buff1[i], buff2[i] );
            bReturn = FALSE;
            break;
        }
    }

    return bReturn;
}

/*

Routine Description:

    Checks to see if a file or directory exists

Arguments:

    char *filename - the name of the file or dir to search for

Return Value:

    true if it exists, false if it doesnt

*/
BOOL FileExists( const char* pszFilename )
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    BOOL fRetval = FALSE;

    hFind = FindFirstFile( pszFilename, &FindFileData );

    if( hFind != INVALID_HANDLE_VALUE )
        fRetval = true;

    FindClose( hFind );

    return fRetval;
}

struct DISCTYPECHECK
{
    char* szPath;
    char* szDiscType;
    int nDiscType;
};

static const DISCTYPECHECK rgddc [] =
{
    { "CDROM0:\\default.xbe",            "Title", DISC_TITLE },
    { "CDROM0:\\video_ts\\video_ts.ifo", "Video", DISC_VIDEO },
    { "CDROM0:\\track01.cda",            "Audio", DISC_AUDIO },
};


// Returns the type of disc that is inserted in to the Xbox DVD Rom Drive
int GetDiscType()
{
    int nDiscType = DISC_BAD;
    bool bRetry = true;

    if( g_DVDRom.IsOpen() )
        g_DVDRom.Close();

    OBJECT_STRING DeviceName;
    RtlInitObjectString(&DeviceName, "\\??\\CdRom0:");
    IoDismountVolumeByName( &DeviceName );

    NTSTATUS Status;
    ULONG TrayState;
    Status = HalReadSMCTrayState( &TrayState, NULL );
    if( NT_SUCCESS( Status ) && TrayState != SMC_TRAY_STATE_MEDIA_DETECT )
    {
        if( TrayState == SMC_TRAY_STATE_NO_MEDIA )
        {
            HANDLE hDevice;

            // At this point, the drive has told the SMC that media could not be
            // detected.  To decide whether this means that the tray is empty
            // versus the tray have unrecognized media, we need to send an IOCTL
            // to the device.

            hDevice = CreateFileA( "cdrom0:",
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL );

            if (hDevice != NULL)
            {
                BOOL fReturn;
                DWORD cbReturned;

                fReturn = DeviceIoControl( hDevice,
                                           IOCTL_CDROM_CHECK_VERIFY,
                                           NULL,
                                           0,
                                           NULL,
                                           0,
                                           &cbReturned,
                                           NULL );

                // If the device reports back that the unit is ready (which it
                // shouldn't since the SMC thinks the tray is empty) or if the
                // media is unrecognized, then the disc is bad.
                if( fReturn || ( GetLastError() == ERROR_UNRECOGNIZED_MEDIA ) )
                {
                    CloseHandle( hDevice );
                    return DISC_BAD;
                }

                CloseHandle( hDevice );
            }
        }
        return DISC_NONE;
    }

    g_DVDRom.Open();

    if( g_DVDRom.IsOpen() )
        return DISC_AUDIO;

    for( int i = 0; i < sizeof (rgddc) / sizeof (DISCTYPECHECK); )
    {
        if( FileExists( rgddc[i].szPath ) )
        {
            nDiscType = rgddc[i].nDiscType;
            break;
        }

        DWORD dwError = GetLastError();

        if( dwError != ERROR_FILE_NOT_FOUND && dwError != ERROR_PATH_NOT_FOUND )
        {
            DebugPrint( "GetDiscType():Error %d on %s\n", dwError, rgddc[i].szPath );

            if( bRetry )
            {
                DebugPrint( "GetDiscType():Retrying...\n" );
                bRetry = false;
                Sleep(100);
                continue;
            }
        }

        i++;
        bRetry = true;
    }

    return nDiscType;
}

void GetDriveID(HANDLE hDevice, bool cdrom, char* model, char* serial, char* firmware)
{
    unsigned i;
    DWORD returned;
    char buffer[sizeof(ATA_PASS_THROUGH) + 512];
    PATA_PASS_THROUGH atapt = (PATA_PASS_THROUGH)buffer;
    bool closeHandle = false;

    if(hDevice == NULL)
    {
        OBJECT_ATTRIBUTES ObjA;
        OBJECT_STRING VolumeString;
        IO_STATUS_BLOCK IoStatusBlock;
        if(cdrom)
            RtlInitObjectString(&VolumeString, "\\Device\\cdrom0");
        else
            RtlInitObjectString(&VolumeString, "\\Device\\Harddisk0\\Partition0");
        InitializeObjectAttributes(&ObjA, &VolumeString, OBJ_CASE_INSENSITIVE, NULL, NULL);
        NtCreateFile(&hDevice, SYNCHRONIZE|GENERIC_READ, &ObjA, &IoStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING);
        closeHandle = true;
    }

    atapt->DataBufferSize = 512;
    atapt->DataBuffer = atapt + 1;

    atapt->IdeReg.bFeaturesReg     = 0;
    atapt->IdeReg.bSectorCountReg  = 0;
    atapt->IdeReg.bSectorNumberReg = 0;
    atapt->IdeReg.bCylLowReg       = 0;
    atapt->IdeReg.bCylHighReg      = 0;
    atapt->IdeReg.bDriveHeadReg    = 0;
    atapt->IdeReg.bHostSendsData   = 0;

    if(cdrom) atapt->IdeReg.bCommandReg = 0xA1;
    else atapt->IdeReg.bCommandReg = 0xEC;

    DeviceIoControl(hDevice, IOCTL_IDE_PASS_THROUGH, atapt, sizeof(ATA_PASS_THROUGH), atapt, sizeof(ATA_PASS_THROUGH), &returned, FALSE);
    PIDE_IDENTIFY_DATA IdData = (PIDE_IDENTIFY_DATA)atapt->DataBuffer;

    for(i=0; i<sizeof(IdData->ModelNumber); i+=2)
    {
        model[i + 0] = IdData->ModelNumber[i + 1];
        model[i + 1] = IdData->ModelNumber[i + 0];
    }
    model[i] = 0;

    for(i=0; i<sizeof(IdData->SerialNumber); i+=2)
    {
        serial[i + 0] = IdData->SerialNumber[i + 1];
        serial[i + 1] = IdData->SerialNumber[i + 0];
    }
    serial[i] = 0;

    for(i=0; i<sizeof(IdData->FirmwareRevision); i+=2)
    {
        firmware[i + 0] = IdData->FirmwareRevision[i + 1];
        firmware[i + 1] = IdData->FirmwareRevision[i + 0];
    }
    firmware[i] = 0;

    if(closeHandle) NtClose(hDevice);
}

static HANDLE g_hFile;
static short* g_psNormalizeMap;
DWORD g_dwMusicCompressPos;
DWORD g_dwMusicCompressLength;
DWORD g_dwEncodeReadBufferPos;
DWORD g_dwEncodeReadBufferLength;
LPBYTE g_lpbEncodeReadBuffer;

// Copy a Track from the Audio CD to the Hard Disc
int CopyAudioTrack( int nTrack ) // CCopySong* pCopySong, TCHAR* pszTrackName )
{
    float fReadProgress = 0.0f;
    int nError = COPY_NO_ERROR;

    CCDDAStreamer streamer( &g_DVDRom, 5 );

    DWORD dwStartPosition = g_DVDRom.GetTrackFrame( nTrack );
    DWORD dwStopPosition = g_DVDRom.GetTrackFrame( nTrack + 1 );
    DWORD dwLength = ( dwStopPosition - dwStartPosition ) * 1000 / CDAUDIO_FRAMES_PER_SECOND;

    if (dwStopPosition < dwStartPosition)
        dwLength = 0;

    // Check if disc is ejected
    if( !g_DVDRom.IsOpen() )
    {
        nError = COPY_DISC_EJECTED;
        dwLength = 0;
    }
    else if( dwLength == 0 )
        nError = COPY_DISC_READ_ERROR;

    if( nError )
        goto end_ripping;

    streamer.SetFrame( dwStartPosition );

    char pszDestFile[MAX_PATH+1];
    pszDestFile[MAX_PATH] = '\0';

    strcpy( pszDestFile, "z:\\AudioCDTrack.pcm" );
    HANDLE hDestFile = CreateFileA( pszDestFile,
                                    GENERIC_WRITE | GENERIC_READ,
                                    0,
                                    NULL,
                                    CREATE_ALWAYS,
                                    FILE_FLAG_SEQUENTIAL_SCAN,
                                    NULL );
    if (hDestFile == INVALID_HANDLE_VALUE)
    {
        DebugPrint( "CreateFile(%hs)) failed\n", pszDestFile );
        nError = COPY_HARDDISK_FULL; // hard disk full
        return nError;
    }

    // Allocate the clusters for the file now.
    LARGE_INTEGER liDistanceToMove;
    liDistanceToMove.QuadPart = (LONGLONG)( dwStopPosition - dwStartPosition ) * CDAUDIO_BYTES_PER_FRAME;
    if ( !SetFilePointerEx( hDestFile, liDistanceToMove, NULL, FILE_BEGIN ) ||
         !SetEndOfFile( hDestFile ) )
    {
        DebugPrint( "SetEndOfFile(%hs) failed\n", pszDestFile );
        nError = COPY_HARDDISK_FULL; // hard disk full
    }

    // Check if we have enough space for WMA file
    ULARGE_INTEGER luiFreeBytesAvailable;
    if ( !GetDiskFreeSpaceEx( "Z:\\", &luiFreeBytesAvailable, 0, 0 ) || \
         luiFreeBytesAvailable.QuadPart < (ULONGLONG)liDistanceToMove.QuadPart )
    {
        DebugPrint( "Not enough space for WMA file\n" );
        nError = COPY_HARDDISK_FULL;
    }

    SetFilePointer( hDestFile, 0, NULL, FILE_BEGIN );

    DebugPrint( "Copying track %d from CD to %hs...\n", nTrack, pszDestFile );

    DWORD dwBytesRemaining = liDistanceToMove.LowPart;
    short sMax = -32767;
    while ( ( !nError ) && ( dwBytesRemaining > 0 ) && ( !GetExitThread() ) )
    {
        int nRead = streamer.Read( g_pbyTrackBuffer, min( BLOCK_SIZE, dwBytesRemaining ) );
        if( nRead == 0 )
            break;

        if( nRead < 0 )
        {
            nError = g_DVDRom.IsOpen() ? COPY_DISC_READ_ERROR : COPY_DISC_EJECTED;
            break;
        }

        DWORD dwWrite;
        if ( !WriteFile( hDestFile, g_pbyTrackBuffer, nRead, &dwWrite, NULL ) )
        {
            nError = COPY_HARDDISK_FULL; // hard disk full
            break;
        }

        int nSamples = nRead / 2;
        const short* samples = (const short*)g_pbyTrackBuffer;
        for( int i = 0; i < nSamples; i += 1, samples += 1 )
        {
            short s = *samples;
            if (s < 0)
                s = -s;
            if (s > sMax)
                sMax = s;
        }

        fReadProgress = (float)( min( streamer.GetFrame(), dwStopPosition ) -
            dwStartPosition ) / ( dwStopPosition - dwStartPosition );

        dwBytesRemaining -= nRead;
    }

    if( !SetEndOfFile( hDestFile ) )
    {
        DebugPrint( "SetEndOfFile(%hs) failed\n", pszDestFile );
        nError = COPY_HARDDISK_FULL; // hard disk full
    }

    if( !CloseHandle( hDestFile ) )
    {
        DebugPrint( "CloseHandle(%hs) failed\n", pszDestFile );
        nError = COPY_HARDDISK_FULL; // hard disk full
    }

    // Don't try to compress the file if the user is trying to end the thread
    if( GetExitThread() )
        goto end_ripping;

    char pszDestWMAFile[MAX_PATH+1];
    pszDestWMAFile[MAX_PATH] = '\0';

    sprintf( pszDestWMAFile, "z:\\AudioCDTrack_%d.wma", nTrack );
    if( !nError )
    {
        DebugPrint( "Compression audio file...\n" );
        nError = CompressAudio( pszDestFile, "z:\\1stwma.wma", sMax ) ? COPY_NO_ERROR : COPY_HARDDISK_FULL;

        if( !nError )
        {
            if( !MoveFileEx( "z:\\1stwma.wma", pszDestWMAFile, 0 ) )
            {
                DebugPrint( "MoveFileEx(%hs, %hs) failed (%d)\n", "z:\\1stwma.wma", pszDestWMAFile, GetLastError() );
                
                // Delete our 1st WMA File
                DeleteFileA( "z:\\1stwma.wma" );
            }
        }
    }

    // Delete the PCM File
    DeleteFileA( pszDestFile );

    // Delete the WMA File
    DeleteFileA( pszDestWMAFile );

    // Update our Megabyte's ripped if the user hasn't aborted the rip
    if( !GetExitThread() )
    {
        float fMegabytesRipped = GetMegabytesRipped();
        SetMegabytesRipped( fMegabytesRipped + ( (float)liDistanceToMove.QuadPart / (1024.0f*1024.0f) ) );
    }

end_ripping:
    NOTHING;

    return nError;
}

// Compress our audio file in to a WMA File
bool CompressAudio( char* pszSrcPath, char* pszDestPath, short sMax )
{
    g_hFile = CreateFileA( pszSrcPath,
                           GENERIC_READ,
                           0,
                           NULL,
                           OPEN_EXISTING,
                           0,
                           NULL );
    if( g_hFile == INVALID_HANDLE_VALUE )
    {
        DebugPrint( "CompressAudio: Cannot open source file: %s\n", pszSrcPath );
        return false;
    }

    g_dwMusicCompressLength = GetFileSize( g_hFile, NULL );

    tWMFEncodeStatus tStatus;
    U32_WMF cbBuffer;
    char szAlteredPath [MAX_PATH];

    HWMFENCODER hWMFEnc = WMFCreateEncoderUsingProfile( &tStatus,
                                                        pszDestPath,
                                                        Profile_AudioOnly_CDAudiophileQuality_128K,
                                                        2,
                                                        44100,
                                                        16,
                                                        g_dwMusicCompressLength,
                                                        &cbBuffer,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        WMF_NoPreProcess,
                                                        szAlteredPath );

    if( tStatus != WMF_Succeeded )
    {
        DebugPrint( "CompressAudio: WMFCreateEncoderUsingProfile failed (%d)\n", tStatus );
        g_dwMusicCompressLength = 0;
        return false;
    }

    ASSERT( hWMFEnc != NULL );

    BYTE* pbBuffer = new U8_WMF[cbBuffer];
    if( pbBuffer == NULL )
    {
        DebugPrint( "Not enough memory to compress audio!\n" );
        g_dwMusicCompressLength = 0;
        CloseHandle( g_hFile );
        WMFClose(hWMFEnc);
        return false;
    }

    g_dwEncodeReadBufferPos = 0;
    g_dwEncodeReadBufferLength = 0;
    g_lpbEncodeReadBuffer = new BYTE [cbBuffer];

    if( g_lpbEncodeReadBuffer == NULL )
    {
        DebugPrint( "Not enough memory to compress audio!\n" );
        g_dwMusicCompressLength = 0;
        CloseHandle( g_hFile );
        WMFClose(hWMFEnc);
        delete [] pbBuffer;
        return false;
    }

    if( sMax > 0 && sMax < 32767 )
    {
        g_psNormalizeMap = new short [65536];
        if( g_psNormalizeMap != NULL )
        {
            float f = 32767.0f / (float)sMax;
            for( int i = 0; i < 65536; i += 1 )
            {
                float s = (float)(i - 32768) / 32767.0f;
                g_psNormalizeMap[i] = (short)( ( s * f ) * 32767.0f );
            }
        }
    }

    tStatus = WMFEncode( hWMFEnc, pbBuffer, cbBuffer, 0, 0 );

    g_dwMusicCompressLength = 0;
    g_dwMusicCompressPos = 0;

    if( g_psNormalizeMap != NULL )
    {
        delete[] g_psNormalizeMap;
        g_psNormalizeMap = NULL;
    }

    if( g_lpbEncodeReadBuffer != NULL)
    {
        delete [] g_lpbEncodeReadBuffer;
        g_lpbEncodeReadBuffer = NULL;
    }

    if( tStatus != WMF_Succeeded )
    {
        DebugPrint( "CompressAudio: WMFEncode failed (%d)\n", tStatus );
        CloseHandle( g_hFile );
        WMFClose(hWMFEnc);
        delete [] pbBuffer;
        return false;
    }

    CloseHandle( g_hFile );
    DeleteFile( pszSrcPath );
    WMFClose( hWMFEnc );
    delete [] pbBuffer;

    DebugPrint( "CompressAudio: complete!\n" );

    return true;
}


I32_WMF WMFCBGetAudioData( U8_WMF** ppbAudioDataBuffer, U32_WMF nInputBufferLen, I64_WMF iSampleOffset, I32_WMF iNumSamplesWanted )
{
    DWORD nBytesNeeded;
    DWORD nBytesRead;
    DWORD nThrowAwayBytes;
    LPVOID lpvReadData;

    DebugPrint( "WMFCBGetAudioData: %d %d\n", (int)iSampleOffset, (int)iNumSamplesWanted );

    g_dwMusicCompressPos = (DWORD)iSampleOffset * sizeof( I16_WMF ) * 2;

    //
    // The WMA encoder ends up re-reading the same regions of the file over and
    // over, so to reduce I/O overhead, we add a buffer to keep the portions of
    // the file that we've already read.  The encoder appears to only read the
    // file in increasing sample offsets, so the cache only moves in one
    // direction.
    //

    nBytesNeeded = iNumSamplesWanted * sizeof( I16_WMF ) * 2;

    if( ( g_dwMusicCompressPos >= g_dwEncodeReadBufferPos ) &&
        ( g_dwMusicCompressPos < g_dwEncodeReadBufferPos + g_dwEncodeReadBufferLength ) )
    {

        nThrowAwayBytes = g_dwMusicCompressPos - g_dwEncodeReadBufferPos;

        g_dwEncodeReadBufferLength -= nThrowAwayBytes;

        MoveMemory( g_lpbEncodeReadBuffer, g_lpbEncodeReadBuffer + nThrowAwayBytes,
            g_dwEncodeReadBufferLength );

    } else {
        g_dwEncodeReadBufferLength = 0;
    }

    g_dwEncodeReadBufferPos = g_dwMusicCompressPos;

    if( g_dwEncodeReadBufferLength < nBytesNeeded ) {

        if( SetFilePointer( g_hFile, g_dwEncodeReadBufferPos +
            g_dwEncodeReadBufferLength, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER ) {
            return 0;
        }

        lpvReadData = g_lpbEncodeReadBuffer + g_dwEncodeReadBufferLength;

        if ( !ReadFile( g_hFile, lpvReadData, nBytesNeeded - g_dwEncodeReadBufferLength,
            &nBytesRead, NULL ) ) {
            return 0;
        }

        nBytesRead &= ~1;
        g_dwEncodeReadBufferLength += nBytesRead;

        if( g_psNormalizeMap != NULL ) {

            short* ps = (short*)lpvReadData;
            short* map = &g_psNormalizeMap[32768];
            for (UINT i = 0; i < nBytesRead; i += 2, ps += 1)
                *ps = map[*ps];
        }
    }

    nBytesRead = nBytesNeeded;

    if( nBytesRead > g_dwEncodeReadBufferLength ) {
        nBytesRead = g_dwEncodeReadBufferLength;
    }

    CopyMemory( *ppbAudioDataBuffer, g_lpbEncodeReadBuffer, nBytesRead );

    return (I32_WMF) nBytesRead;
}

// Thread Proc which will copy our song
DWORD WINAPI AudioTrackCopyThread( LPVOID lpParameter )
{
    int iTrackNum = *((int*)lpParameter);

    g_dwTrackRipStartTime = GetTickCount();
    int nReturn = CopyAudioTrack( iTrackNum - 1 );
    g_dwTrackRipEndTime = GetTickCount();

    // Make sure we increment our counters
    if( !GetExitThread() )
    {
        // Update our Total Rip Time
        DWORD dwTotRipTime = GetTotalRipTime();
        SetTotalRipTime( dwTotRipTime + ( g_dwTrackRipEndTime - g_dwTrackRipStartTime ) );
        g_dwTrackRipStartTime = 0;
        g_dwTrackRipEndTime = 0;

        IncTracksRipped();
        IncCurrentTrack();
    }

    return nReturn;
}