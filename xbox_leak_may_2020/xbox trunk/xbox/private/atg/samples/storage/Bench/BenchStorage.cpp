//-----------------------------------------------------------------------------
// File: BenchStorage.cpp
//
// Desc: Benchmark Xbox storage
//
// Hist: 04.25.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "BenchStorage.h"
#include "File.h"
#include <XbStorageDevice.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include "Resource.h"

#pragma warning( push, 3 )  // Suppress VC warnings when compiling at W4
#pragma warning( disable: 4702 )
#include <list>
#include <vector>
#pragma warning( pop )

#ifdef _DEBUG
    #define VERIFY( e ) assert( e );
#else
    #define VERIFY( e ) (VOID)(e);
#endif




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_MEMORY_UNITS = 2 * XGetPortCount();

//#define QUICK_TEST_MODE
#ifdef QUICK_TEST_MODE
    const DWORD TEST_FILE_SIZE          = 1024 * 1024 * 1;      // 1 MB
    const DWORD MAX_DEVICE_POLLS        = 1000;
    const DWORD MIN_BUFFER_SIZE         = 16 * 1024;            // 16 K
    const DWORD MIN_BUFFER_SIZE_API     = 32 * 1024;            // 32 K
    const DWORD MAX_BUFFER_SIZE         = 1024 * 128;           // 128 K
    const DWORD DEFAULT_BUFFER_SIZE     = 1024 * 64;            // 64 K
    const DWORD MIN_CACHE_SIZE          = 64 * 1024;            // 64 K
    const DWORD MAX_CACHE_SIZE          = 1024 * 1024 * 1;      // 1 MB
    const DWORD API_PASSES              = 100;
    const DWORD MAX_RANDOM_SEEKS        = 100;
    const DWORD MAX_SEQUENTIAL_SEEKS    = 100;
    const DWORD MAX_RANDOM_READS        = 100;
    const DWORD MAX_RANDOM_WRITES       = 20;
    const DWORD MAX_SEQUENTIAL_READS    = 100;
    const DWORD MAX_SEQUENTIAL_WRITES   = 20;
    const DWORD REGION_PASSES           = 10;
    const DWORD ENUM_SAVE_GAMES         = 10;
    const DWORD ENUM_DIR_TREE           = 6;       // root folders
    const DWORD ENUM_OPEN_TREE          = 2;       // root folders
    const DWORD SAVE_SUBFOLDER_RATIO    = 4;       // 1 in 4
    const DWORD MAX_NICKNAMES           = 200;
#else
    const DWORD TEST_FILE_SIZE          = 1024 * 1024 * 4;      // 4 MB
    const DWORD MAX_DEVICE_POLLS        = 1000000;
    const DWORD MIN_BUFFER_SIZE         = 4;
    const DWORD MIN_BUFFER_SIZE_API     = 128;
    const DWORD MAX_BUFFER_SIZE         = 1024 * 512;           // 512 K
    const DWORD DEFAULT_BUFFER_SIZE     = 1024 * 64;            // 64 K
    const DWORD MIN_CACHE_SIZE          = 4 * 1024;             // 4 K
    const DWORD MAX_CACHE_SIZE          = 1024 * 1024 * 4;      // 4 MB
    const DWORD API_PASSES              = 10000;
    const DWORD MAX_RANDOM_SEEKS        = 100000;
    const DWORD MAX_SEQUENTIAL_SEEKS    = 100000;
    const DWORD MAX_RANDOM_READS        = 10000;
    const DWORD MAX_RANDOM_WRITES       = 2000;
    const DWORD MAX_SEQUENTIAL_READS    = 10000;
    const DWORD MAX_SEQUENTIAL_WRITES   = 2000;
    const DWORD REGION_PASSES           = 1000;
    const DWORD ENUM_SAVE_GAMES         = 100;
    const DWORD ENUM_DIR_TREE           = 12;       // root folders
    const DWORD ENUM_OPEN_TREE          = 4;        // root folders
    const DWORD SAVE_SUBFOLDER_RATIO    = 4;        // 1 in 4
    const DWORD MAX_NICKNAMES           = 200;
#endif

const CHAR* const strTEST_FILE                 = "BenchmarkTest.Garbage";
const CHAR* const strTEST_SAVE_GAME            = "Save";
const CHAR* const strDEVICE_CHANGES            = "DeviceChanges";
const CHAR* const strBUFFERS_READ_FILE         = "FileReadBuffers";
const CHAR* const strBUFFERS_WRITE_FILE        = "FileWriteBuffers";
const CHAR* const strASYNC_READ_NOTIFY_FILE    = "AsyncReadNotify";
const CHAR* const strASYNC_WRITE_NOTIFY_FILE   = "AsyncWriteNotify";
const CHAR* const strASYNC_READ_CALLBACK_FILE  = "AsyncReadCallback";
const CHAR* const strASYNC_WRITE_CALLBACK_FILE = "AsyncWriteCallback";
const CHAR* const strAPI_OPEN_FILE             = "ApiOpen";
const CHAR* const strAPI_CLOSE_FILE            = "ApiClose";
const CHAR* const strAPI_READ_FILE             = "ApiRead";
const CHAR* const strAPI_WRITE_FILE            = "ApiWrite";
const CHAR* const strAPI_SEEK_FILE             = "ApiSeek";
const CHAR* const strRANDOM_SEEK_FILE          = "RandomSeek";
const CHAR* const strSEQ_SEEK_FILE             = "SequentialSeek";
const CHAR* const strRANDOM_READ_FILE          = "RandomRead";
const CHAR* const strRANDOM_WRITE_FILE         = "RandomWrite";
const CHAR* const strSEQ_READ_FILE             = "SequentialRead";
const CHAR* const strSEQ_WRITE_FILE            = "SequentialWrite";
const CHAR* const strREGION_READ_FILE          = "RegionRead";
const CHAR* const strREGION_WRITE_FILE         = "RegionWrite";
const CHAR* const strCACHE_RANDOM_READ_FILE    = "CacheRandomRead";
const CHAR* const strCACHE_SEQ_READ_FILE       = "CacheSeqRead";
const CHAR* const strCACHE_RANDOM_WRITE_FILE   = "CacheRandomWrite";
const CHAR* const strCACHE_SEQ_WRITE_FILE      = "CacheSeqWrite";
const CHAR* const strENUM_SAVES_FILE           = "EnumSaves";
const CHAR* const strENUM_DIRS_FILE            = "EnumDirs";
const CHAR* const strENUM_SOUNDTRACKS_FILE     = "EnumSoundtracks";
const CHAR* const strENUM_NICKNAMES_FILE       = "EnumNicknames";
const CHAR* const strENUM_OPEN_FILE            = "EnumOpenFile";

const WCHAR* const strTEST_SAVE_CONTAINER      = L"Garbage";
const WCHAR* const strNICKNAME_PREFIX          = L"Nickname";

// Change these values to limit/expand testing
const BOOL  TEST_MU = FALSE;
const BOOL  TEST_HD = TRUE;
const BOOL  TEST_DVD = FALSE; // TRUE;




//-----------------------------------------------------------------------------
// Name: AutoHandle
// Desc: Local class for doing automatic handle cleanup
//-----------------------------------------------------------------------------
class AutoHandle
{

    HANDLE m_Handle;

public:

    AutoHandle( HANDLE h = NULL )
    : 
        m_Handle( h )
    {
    }

    ~AutoHandle()
    {
        Close();
    }

    operator HANDLE()
    {
        return m_Handle;
    }

    VOID Close()
    {
        if( m_Handle != NULL )
        {
            VERIFY( CloseHandle( m_Handle ) );
            m_Handle = NULL;
        }
    }

private:

    // Disabled
    AutoHandle( const AutoHandle& );
    AutoHandle& operator=( const AutoHandle& );

};




//-----------------------------------------------------------------------------
// Name: AutoHeapPtr
// Desc: Local class for doing automatic heap cleanup
//-----------------------------------------------------------------------------
class AutoHeapPtr
{

    HANDLE m_Heap;
    BYTE* m_pData;

public:

    AutoHeapPtr()
    : 
        m_Heap( GetProcessHeap() ),
        m_pData( NULL )
    {
    }

    ~AutoHeapPtr()
    {
        Free();
    }

    BYTE* Alloc( DWORD dwBytes )
    {
        Free();
        m_pData = ( BYTE* )( HeapAlloc( m_Heap, 0, dwBytes ) );
        return m_pData;
    }

    VOID Free()
    {
        if( m_pData != NULL )
        {
            VERIFY( HeapFree( m_Heap, 0, m_pData ) );
            m_pData = NULL;
        }
    }

private:

    // Disabled
    AutoHeapPtr( const AutoHeapPtr& );
    AutoHeapPtr& operator=( const AutoHeapPtr& );

};




//-----------------------------------------------------------------------------
// Name: TestFile
// Desc: Local class for large benchmark testing file. Automatically
//       deletes the file when it goes out of scope.
//-----------------------------------------------------------------------------
class TestFile
{

    CHAR m_strFile[64];
    BOOL m_bExists;

public:

    TestFile( CHAR chDrive )
    :
        m_bExists( FALSE )
    {
        m_strFile[0] = chDrive;
        m_strFile[1] = ':';
        m_strFile[2] = '\\';
        lstrcpyA( m_strFile+3, strTEST_FILE );
    }

    ~TestFile()
    {
        Delete();
    }

    BOOL Create()
    {
        Delete();

        File file;
        if( !file.Create( m_strFile, GENERIC_WRITE ) )
            return FALSE;

        // Create a buffer with incrementing DWORDS
        AutoHeapPtr Buffer;
        BYTE* pBuffer = Buffer.Alloc( TEST_FILE_SIZE );
        DWORD* pWalk = (DWORD*)( pBuffer );
        for( DWORD i = 0; i < TEST_FILE_SIZE / sizeof(DWORD); ++i, ++pWalk )
            *pWalk = i;

        // Write the buffer
        FLOAT fElapsed;
        BOOL bSuccess = file.Write( pBuffer, TEST_FILE_SIZE, fElapsed );
        assert( bSuccess );
        if( bSuccess )
            m_bExists = TRUE;
        return bSuccess;
    }

    const CHAR* GetFileName() const
    {
        return m_strFile;
    }

    VOID Delete()
    {
        if( m_bExists )
        {
            VERIFY( DeleteFile( m_strFile ) );
            m_bExists = FALSE;
        }
    }

    static BOOL IsValid( const BYTE* pBuffer, DWORD dwBytes )
    {
        assert( pBuffer != NULL );
        assert( dwBytes >= sizeof(DWORD) );
        assert( dwBytes % sizeof(DWORD) == 0 );

        // Validate the buffer against any DWORD-aligned portion of the file
        DWORD* pWalk = (DWORD*)( pBuffer );
        DWORD dwValue = *pWalk;
        ++pWalk;
        for( DWORD i = 1; i < dwBytes / sizeof(DWORD); ++i, ++pWalk )
        {
            assert( *pWalk == dwValue + 1 );
            if( *pWalk != dwValue + 1 )
                return FALSE;
            dwValue = *pWalk;
        }
        return TRUE;
    }

};




//-----------------------------------------------------------------------------
// Name: ResultsFile
// Desc: Local class for writing benchmark output. File is tab delimited for
//       easy importing into Excel. File is always written to user data region
//       (U).
//-----------------------------------------------------------------------------
class ResultsFile
{

    File m_File;

public:

    ResultsFile()
    :
        m_File()
    {
    }

    BOOL Create( const CHAR* strFileName, CHAR chDrive, 
                 const CHAR* strHeader1 = NULL, const CHAR* strHeader2 = NULL, 
                 const CHAR* strHeader3 = NULL, const CHAR* strHeader4 = NULL )
    {
        CHAR strDrive[2] = { chDrive, '\0' };
        CHAR strPath[ MAX_PATH ];
        lstrcpyA( strPath, "U:\\" );
        lstrcatA( strPath, strFileName );
        lstrcatA( strPath, strDrive );
        lstrcatA( strPath, ".csv" );
        BOOL bSuccess = m_File.Create( strPath, GENERIC_WRITE );
        if( !bSuccess )
            return FALSE;

        FLOAT fElapsed;
        if( strHeader1 != NULL )
        {
            bSuccess &= m_File.Write( strHeader1, lstrlenA( strHeader1 ), fElapsed );
            bSuccess &= m_File.Write( ",", 1, fElapsed );
        }
        if( strHeader2 != NULL )
        {
            bSuccess &= m_File.Write( strHeader2, lstrlenA( strHeader2 ), fElapsed );
            bSuccess &= m_File.Write( ",", 1, fElapsed );
        }
        if( strHeader3 != NULL )
        {
            bSuccess &= m_File.Write( strHeader3, lstrlenA( strHeader3 ), fElapsed );
            bSuccess &= m_File.Write( ",", 1, fElapsed );
        }
        if( strHeader4 != NULL )
        {
            bSuccess &= m_File.Write( strHeader4, lstrlenA( strHeader4 ), fElapsed );
            bSuccess &= m_File.Write( ",", 1, fElapsed );
        }
        bSuccess &= m_File.Write( "\n", 1, fElapsed );

        return bSuccess;
    }

    VOID Write( DWORD i, FLOAT j )
    {
        // Comma-delimited
        FLOAT fElapsed;
        CHAR strOutput[64];
        DWORD nChars = sprintf( strOutput, "%lu,%f\n", i, j );
        m_File.Write( strOutput, nChars, fElapsed );
    }

    VOID Write( DWORD i, FLOAT j, FLOAT k )
    {
        // Comma-delimited
        FLOAT fElapsed;
        CHAR strOutput[64];
        DWORD nChars = sprintf( strOutput, "%lu,%f,%f\n", i, j, k );
        m_File.Write( strOutput, nChars, fElapsed );
    }

    VOID Write( DWORD i, FLOAT j, FLOAT k, FLOAT m )
    {
        // Comma-delimited
        FLOAT fElapsed;
        CHAR strOutput[64];
        DWORD nChars = sprintf( strOutput, "%lu,%f,%f,%f\n", i, j, k, m );
        m_File.Write( strOutput, nChars, fElapsed );
    }

    VOID Write( FLOAT i, FLOAT j, FLOAT k )
    {
        // Comma-delimited
        FLOAT fElapsed;
        CHAR strOutput[64];
        DWORD nChars = sprintf( strOutput, "%f,%f,%f", i, j, k );
        m_File.Write( strOutput, nChars, fElapsed );
    }

private:

    // Disabled
    ResultsFile( const ResultsFile& );
    ResultsFile& operator=( const ResultsFile& );

};




//-----------------------------------------------------------------------------
// Name: SavedGames
// Desc: Generates pseudo saved games
//-----------------------------------------------------------------------------
class SavedGames
{

    DWORD m_dwSeed;
    DWORD m_dwSavedGames;
    CHAR  m_chDrive;

public:

    SavedGames( DWORD dwSeed, DWORD dwSavedGames, CHAR chDrive )
    :
        m_dwSeed( dwSeed ),
        m_dwSavedGames( dwSavedGames ),
        m_chDrive( chDrive )
    {
    }

    ~SavedGames()
    {
        Destroy();
    }

    BOOL Create()
    {
        srand( m_dwSeed );
        CHAR strPath[4] = "x:\\";
        strPath[0] = m_chDrive;

        // Determine the maximum number of clusters available per saved
        // game. Each saved game requires at least two clusters: one for
        // the directory and one for the meta data file.
        DWORD dwClusterSize = XGetDiskClusterSize( strPath );
        DWORD dwMaxClusters = TEST_FILE_SIZE / m_dwSavedGames /
                              dwClusterSize;
        assert( dwMaxClusters >= 2 );
        DWORD dwGameClusters = dwMaxClusters - 2;

        for( DWORD i = 0; i < m_dwSavedGames; ++i )
        {
            // Generate a name
            WCHAR strSave[ MAX_GAMENAME ];
            WCHAR strNum[32];
            lstrcpyW( strSave, strTEST_SAVE_CONTAINER );
            lstrcatW( strSave, _itow( i, strNum, 10 ) );

            // Create the container (2 clusters)
            CHAR strDir[ MAX_PATH ];
            DWORD dwSuccess = XCreateSaveGame( strPath, strSave, OPEN_ALWAYS,
                                               XSAVEGAME_NOCOPY, strDir,
                                               MAX_PATH );
            assert( dwSuccess == ERROR_SUCCESS );
            if( dwSuccess != ERROR_SUCCESS )
                return FALSE;

            // Fill the container (dir) with some files
            DWORD dwNumFiles = rand() % ( dwGameClusters + 1 );
            DWORD dwClusters = ( dwNumFiles == 0 ) ? 0 : dwGameClusters / dwNumFiles;
            for( DWORD j = 0; j < dwNumFiles; ++j )
            {
                // Generate a name
                CHAR strFile[ MAX_PATH ];
                CHAR strNum2[32];
                lstrcpyA( strFile, strDir );
                lstrcatA( strFile, strTEST_SAVE_GAME );
                lstrcatA( strFile, itoa( j, strNum2, 10 ) );

                // Create the file
                File file;
                BOOL bSuccess = file.Create( strFile, GENERIC_WRITE );
                assert( bSuccess );
                if( !bSuccess )
                    return FALSE;

                // Size the file
                DWORD dwBytes = ( dwClusters - 1 ) * dwClusterSize;
                dwBytes += rand() % dwClusterSize;
                file.SetPos( dwBytes );
                bSuccess = file.SetEOF();
                if( !bSuccess )
                    return FALSE;
            }

            // Create a subfolder -- sometimes
            if( dwNumFiles * dwClusters < dwGameClusters && 
                ( rand() % SAVE_SUBFOLDER_RATIO ) == 0 )
            {
                // Generate a name
                CHAR strPath[ MAX_PATH ];
                lstrcpyA( strPath, strDir );
                lstrcatA( strPath, strTEST_SAVE_GAME );
                lstrcatA( strPath, "Subdir" );

                BOOL bSuccess = CreateDirectory( strPath, NULL );
                assert( bSuccess );
                if( !bSuccess )
                    return FALSE;

                // Fill the subdir with files
                for( DWORD j = 1; j < dwGameClusters / dwClusters - 
                                      dwNumFiles; ++j )
                {
                    // Generate a name
                    CHAR strFile[ MAX_PATH ];
                    CHAR strNum2[32];
                    lstrcpyA( strFile, strPath );
                    lstrcatA( strFile, "\\" );
                    lstrcatA( strFile, strTEST_SAVE_GAME );
                    lstrcatA( strFile, itoa( j, strNum2, 10 ) );

                    // Create the file
                    File file;
                    BOOL bSuccess = file.Create( strFile, GENERIC_WRITE );
                    assert( bSuccess );
                    if( !bSuccess )
                        return FALSE;

                    // Size the file
                    DWORD dwBytes = ( dwClusters - 1 ) * dwClusterSize;
                    dwBytes += rand() % dwClusterSize;
                    file.SetPos( dwBytes );
                    bSuccess = file.SetEOF();
                    if( !bSuccess )
                        return FALSE;
                }
            }
        }
        return TRUE;
    }

    VOID Destroy()
    {
        CHAR strPath[4] = "x:\\";
        strPath[0] = m_chDrive;

        for( DWORD i = 0; i < m_dwSavedGames; ++i )
        {
            // Generate the name
            WCHAR strSave[ MAX_GAMENAME ];
            WCHAR strNum[32];
            lstrcpyW( strSave, strTEST_SAVE_CONTAINER );
            lstrcatW( strSave, _itow( i, strNum, 10 ) );

            // Delete the container
            VERIFY( XDeleteSaveGame( strPath, strSave ) == ERROR_SUCCESS );
        }
    }

private:

    // Disabled
    SavedGames();
    SavedGames( const SavedGames& );
    SavedGames& operator=( const SavedGames& );

};




//-----------------------------------------------------------------------------
// Name: FileList
// Desc: List of file names
//-----------------------------------------------------------------------------
class FileList : public std::list< CHAR* >
{

public:

    ~FileList()
    {
        for( const_iterator i = begin(); i != end(); ++i )
            delete *i;
    }

};




//-----------------------------------------------------------------------------
// Name: OpenFileList
// Desc: List of open file handles
//-----------------------------------------------------------------------------
class OpenFileList : public std::list< HANDLE >
{

public:

    ~OpenFileList()
    {
        for( const_iterator i = begin(); i != end(); ++i )
        {
            VERIFY( CloseHandle( *i ) );
        }
    }

};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program. Initializes everything, then begins the
//       profiling phase.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    BenchStorage xbApp;

    if( FAILED( xbApp.Create() ) )
        return;

    // Run the tests
    xbApp.RunBenchSuite();

    // Show completion status
    xbApp.ShowComplete();

    Sleep( INFINITE );
}




//-----------------------------------------------------------------------------
// Name: BenchStorage()
// Desc: Constructor
//-----------------------------------------------------------------------------
BenchStorage::BenchStorage()
:
    m_xprResource(),
    m_Font(),
    m_strStatusHdr(),
    m_strStatus(),
    m_StatusTimer( FALSE ),
    m_MemUnit(),
    m_Overlapped(),
    m_dwBytesTransferred( 0 ),
    m_dwHdBufferSize( DEFAULT_BUFFER_SIZE ),
    m_dwDefaultCacheSize( XGetFileCacheSize() ),
    m_dwSeed( GetTickCount() ),
    m_chHardDrive( '\0' ),
    m_chMemUnit( '\0' ),
    m_chDvd( '\0' )
{
    *m_strStatusHdr = 0;
    *m_strStatus = 0;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: One time initialization
//-----------------------------------------------------------------------------
HRESULT BenchStorage::Initialize()
{
    // Create the resources
    // if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
    //                                   resource_NUM_RESOURCES ) ) )
    //     return XBAPPERR_MEDIANOTFOUND;

    // Load the font resources
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font16.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    m_chHardDrive = 'T';            // persistent data region on HD
    m_chDvd = 'D';                  // DVD drive
    m_chMemUnit = FindBestMu();     // first available MU

    // If no MUs found initially, wait a few moments and try again.
    // The peripheral drivers are initialized late, and may take a
    // few moments to enum all MUs.
    CXBStopWatch stopWatch( TRUE );
    while( m_chMemUnit == '\0' )
    {
        Sleep( 200 );
        m_chMemUnit = FindBestMu();
        if( stopWatch.GetElapsedSeconds() > 5.0f )
            break;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RunBenchSuite()
// Desc: Run benchmark suites
//-----------------------------------------------------------------------------
VOID BenchStorage::RunBenchSuite()
{
    m_StatusTimer.StartZero();

    RunGetDeviceChangesSuite();
    RunFileBuffersSuite();
    RunAsyncSuite();
    RunApiComparisonSuite();
    RunRandomSeekSuite();
    RunSequentialSeekSuite();
    RunRandomAccessSuite();
    RunSequentialAccessSuite();
    RunCacheSuite();
    RunRegionSuite();
    RunEnumerationSuite();
    RunScatterGatherSuite();
    RunInterleavedSuite();
}




//-----------------------------------------------------------------------------
// Name: ShowComplete()
// Desc: Display completion status
//-----------------------------------------------------------------------------
VOID BenchStorage::ShowComplete()
{
    lstrcpyW( m_strStatusHdr, L"Benchmarks complete" );
    lstrcpyW( m_strStatus, L"Output available on U:\n"
                           L"xbcp xU:\\*.csv" );
    ShowStatus( TRUE );
}




//-----------------------------------------------------------------------------
// Name: FindBestMu()
// Desc: Find and mount the MU with the largest amount of free space
//-----------------------------------------------------------------------------
CHAR BenchStorage::FindBestMu()
{
    // Locate the MU with the largest amount of free space
    ULONGLONG qwMaxFreeBlocks = ULONGLONG( 0 );
    DWORD dwBestMu = DWORD(-1);
    DWORD dwSnapshot = CXBMemUnit::GetMemUnitSnapshot();
    for( DWORD i = 0; i < MAX_MEMORY_UNITS; ++i )
    {
        if( dwSnapshot & CXBMemUnit::GetMemUnitMask( i ) )
        {
            // Temporarily mount MU
            DWORD dwPort = CXBMemUnit::GetMemUnitPort( i );
            DWORD dwSlot = CXBMemUnit::GetMemUnitSlot( i );
            DWORD dwError;
            CXBMemUnit MemUnit( dwPort, dwSlot, dwError );
            CXBStorageDevice MuDevice( MemUnit.GetDrive() );

            // See how much room on MU
            ULONGLONG qwTotalBlocks;
            ULONGLONG qwUsedBlocks;
            ULONGLONG qwFreeBlocks;
            if( MuDevice.GetSize( qwTotalBlocks, qwUsedBlocks, qwFreeBlocks ) )
            {
                if( qwFreeBlocks > qwMaxFreeBlocks )
                {
                    qwMaxFreeBlocks = qwFreeBlocks;
                    dwBestMu = i;
                }
            }
        }
    }
    if( dwBestMu != DWORD(-1) )
    {
        // Mount MU for duration of app
        DWORD dwPort = CXBMemUnit::GetMemUnitPort( dwBestMu );
        DWORD dwSlot = CXBMemUnit::GetMemUnitSlot( dwBestMu );
        DWORD dwError;
        if( m_MemUnit.Mount( dwPort, dwSlot, dwError ) )
            return m_MemUnit.GetDrive();
    }

    // No MUs available
    return '\0';
}




//-----------------------------------------------------------------------------
// Name: RunGetDeviceChangesSuite()
// Desc: Evaluate the speed of XGetDevices() and XGetDeviceChanges()
//-----------------------------------------------------------------------------
VOID BenchStorage::RunGetDeviceChangesSuite()
{
    DeviceType devType = Device_HD;
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strDEVICE_CHANGES, 
                  "XGetDevices",
                  L"Compare XGetDevices and XGetDeviceChanges", "Calls",
                  "XGetDevices (seconds)", "XGetDeviceChanges (seconds)" ) )
        return;

    CXBStopWatch timer( FALSE );
    for( DWORD i = 0; i < MAX_DEVICE_POLLS; ++i )
    {
        timer.Start();
        XGetDevices( XDEVICE_TYPE_MEMORY_UNIT );
        timer.Stop();
        ShowStatus( FLOAT(i) / FLOAT(MAX_DEVICE_POLLS*2) );
    }

    FLOAT fGetDevices = timer.GetElapsedSeconds();
    timer.Reset();

    DWORD dwInsert;
    DWORD dwRemove;
    for( DWORD i = 0; i < MAX_DEVICE_POLLS; ++i )
    {
        timer.Start();
        XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, &dwInsert, &dwRemove );
        timer.Stop();
        ShowStatus( FLOAT(MAX_DEVICE_POLLS+i) / FLOAT(MAX_DEVICE_POLLS*2) );
    }

    FLOAT fGetDeviceChanges = timer.GetElapsedSeconds();
    fileResults.Write( MAX_DEVICE_POLLS, fGetDevices, fGetDeviceChanges );
}




//-----------------------------------------------------------------------------
// Name: RunFileBuffersSuite()
// Desc: Evaluate the best size for file buffers
//-----------------------------------------------------------------------------
VOID BenchStorage::RunFileBuffersSuite()
{
    FileBuffersRead( Device_MU );
    FileBuffersRead( Device_HD );
    FileBuffersRead( Device_DVD );

    FileBuffersWrite( Device_MU );
    FileBuffersWrite( Device_HD );
}




//-----------------------------------------------------------------------------
// Name: RunAsyncSuite()
// Desc: Evaluate asynchronous file I/O
//-----------------------------------------------------------------------------
VOID BenchStorage::RunAsyncSuite()
{
    AsyncRead( Device_MU );
    AsyncRead( Device_HD );
    AsyncRead( Device_DVD );

    AsyncWrite( Device_MU );
    AsyncWrite( Device_HD );
}




//-----------------------------------------------------------------------------
// Name: RunApiComparisonSuite()
// Desc: Evaluate the performance differences between Xbox API, C API and C++
//       API
//-----------------------------------------------------------------------------
VOID BenchStorage::RunApiComparisonSuite()
{
    ApiCompare( Device_MU );
    ApiCompare( Device_HD );
    ApiCompare( Device_DVD );
}




//-----------------------------------------------------------------------------
// Name: RunRandomSeekSuite()
// Desc: Evaluate random access time
//-----------------------------------------------------------------------------
VOID BenchStorage::RunRandomSeekSuite()
{
    RandomSeek( Device_MU );
    RandomSeek( Device_HD );
    RandomSeek( Device_DVD );
}




//-----------------------------------------------------------------------------
// Name: RunSequentialSeekSuite()
// Desc: Evaluate sequential access time
//-----------------------------------------------------------------------------
VOID BenchStorage::RunSequentialSeekSuite()
{
    SeqSeek( Device_MU );
    SeqSeek( Device_HD );
    SeqSeek( Device_DVD );
}




//-----------------------------------------------------------------------------
// Name: RunRandomAccessSuite()
// Desc: Evaluate random access file I/O
//-----------------------------------------------------------------------------
VOID BenchStorage::RunRandomAccessSuite()
{
    RandomRead( Device_MU );
    RandomRead( Device_HD );
    RandomRead( Device_DVD );

    RandomWrite( Device_MU );
    RandomWrite( Device_HD );
}




//-----------------------------------------------------------------------------
// Name: RunSequentialAccessSuite()
// Desc: Evaluate sequential access file I/O
//-----------------------------------------------------------------------------
VOID BenchStorage::RunSequentialAccessSuite()
{
    SeqRead( Device_MU );
    SeqRead( Device_HD );
    SeqRead( Device_DVD );

    SeqWrite( Device_MU );
    SeqWrite( Device_HD );
}




//-----------------------------------------------------------------------------
// Name: RunCacheSuite()
// Desc: Evaluate file caching issues
//-----------------------------------------------------------------------------
VOID BenchStorage::RunCacheSuite()
{
    CacheRead( Device_MU );
    CacheRead( Device_HD );
    CacheRead( Device_DVD );

    CacheWrite( Device_MU );
    CacheWrite( Device_HD );
}




//-----------------------------------------------------------------------------
// Name: RunRegionSuite()
// Desc: Evaluate speeds of various hard drive regions
//-----------------------------------------------------------------------------
VOID BenchStorage::RunRegionSuite()
{
    RegionRead();
    RegionWrite();
}




//-----------------------------------------------------------------------------
// Name: RunEnumerationSuite()
// Desc: Enumerate storage items like saved games, directories, soundtracks,
//       nicknames
//-----------------------------------------------------------------------------
VOID BenchStorage::RunEnumerationSuite()
{
    EnumSaves( Device_MU );
    EnumSaves( Device_HD_User );

    EnumDirs( Device_MU );
    EnumDirs( Device_HD_User );

    EnumSoundtracks();
    EnumNicknames();

    EnumOpen( Device_MU );
    EnumOpen( Device_HD_User );
}




//-----------------------------------------------------------------------------
// Name: RunScatterGatherSuite()
// Desc: Evaluate scatter and gather routines
//-----------------------------------------------------------------------------
VOID BenchStorage::RunScatterGatherSuite()
{
    Scatter( Device_MU );
    Scatter( Device_HD );
    Scatter( Device_DVD );

    Gather( Device_MU );
    Gather( Device_HD );
}




//-----------------------------------------------------------------------------
// Name: RunInterleavedSuite()
// Desc: Evaluate speed of interleaved I/O between hard drive and DVD
//-----------------------------------------------------------------------------
VOID BenchStorage::RunInterleavedSuite()
{
    InterleavedRead();
    InterleavedWrite();
}




//-----------------------------------------------------------------------------
// Name: FileBuffersRead()
// Desc: Evaluate file buffer size for reading
//-----------------------------------------------------------------------------
VOID BenchStorage::FileBuffersRead( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strBUFFERS_READ_FILE, 
                  "FileBuffersRead", L"File Buffer Size for Read",
                  "Buffer Size (bytes)", "Read Time (seconds)" ) )
        return;

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( TEST_FILE_SIZE );

    // Read the file using various buffer sizes
    FLOAT fBest = 0.0f;
    for( DWORD dwBufferSize = MIN_BUFFER_SIZE; dwBufferSize <= MAX_BUFFER_SIZE; 
         dwBufferSize *= 2 )
    {
        // Open the test file
        File fileRead;
        VERIFY( fileRead.Open( fileTest.GetFileName(), GENERIC_READ ) );

        // Read the entire file
        DWORD dwTotalRead = 0;
        FLOAT fTotal = 0.0f;
        for( ;; )
        {
            DWORD dwRead;
            FLOAT fElapsed;
            VERIFY( fileRead.Read( pBuffer, dwBufferSize, dwRead, fElapsed ) );
            fTotal += fElapsed;
            assert( TestFile::IsValid( pBuffer, dwRead ) );
            dwTotalRead += dwRead;
            if( dwTotalRead >= TEST_FILE_SIZE )
                break;

            ShowStatus( dwBufferSize, FLOAT(dwTotalRead) / FLOAT(TEST_FILE_SIZE) );
        }

        fileResults.Write( dwBufferSize, fTotal );

        // Save the best hard drive buffer size for later
        if( devType == Device_HD )
        {
            if( fBest == 0.0f || fTotal < fBest )
            {
                fBest = fTotal;
                m_dwHdBufferSize = dwBufferSize;
            }
        }
    }
}




//-----------------------------------------------------------------------------
// Name: FileBuffersWrite()
// Desc: Evaluate file buffer size for writing
//-----------------------------------------------------------------------------
VOID BenchStorage::FileBuffersWrite( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strBUFFERS_WRITE_FILE, 
                  "FileBuffersWrite", L"File Buffer Size for Write",
                  "Buffer Size (bytes)", "Write Time (seconds)" ) )
        return;

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( TEST_FILE_SIZE );

    // Write to the file using various buffer sizes
    for( DWORD dwBufferSize = MIN_BUFFER_SIZE; dwBufferSize <= MAX_BUFFER_SIZE; 
         dwBufferSize *= 2 )
    {
        // Open the test file
        File fileWrite;
        VERIFY( fileWrite.Open( fileTest.GetFileName(), GENERIC_WRITE ) );

        // Write the entire file
        FLOAT fTotal = 0.0f;
        DWORD dwTotalWritten = 0;
        for( ;; )
        {
            DWORD dwWritten;
            FLOAT fElapsed;
            VERIFY( fileWrite.Write( pBuffer, dwBufferSize, dwWritten, fElapsed ) );
            fTotal += fElapsed;
            dwTotalWritten += dwWritten;
            if( dwTotalWritten >= TEST_FILE_SIZE )
                break;

            ShowStatus( dwBufferSize, FLOAT(dwTotalWritten) / FLOAT(TEST_FILE_SIZE) );
        }

        fileResults.Write( dwBufferSize, fTotal );
    }
}




//-----------------------------------------------------------------------------
// Name: AsyncRead()
// Desc: Evaluate speed of asynchronous read
//-----------------------------------------------------------------------------
VOID BenchStorage::AsyncRead( DeviceType devType )
{
    AsyncReadNotify( devType );
    AsyncReadCallback( devType );
}




//-----------------------------------------------------------------------------
// Name: AsyncReadNotify()
// Desc: Evaluate speed of asynchronous read w/ notification
//-----------------------------------------------------------------------------
VOID BenchStorage::AsyncReadNotify( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strASYNC_READ_NOTIFY_FILE, 
                  "AsyncReadNotify", L"Asynchronous Read with Notify",
                  "Buffer Size (bytes)", "Read Time (seconds)" ) )
        return;

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( MAX_BUFFER_SIZE );

    // Buffer address must be DWORD aligned
    assert( (DWORD)pBuffer % sizeof(DWORD) == 0 );

    // Prepare the notification event
    m_Overlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    AutoHandle hEvent( m_Overlapped.hEvent );

    // Read the file using various buffer sizes.
    // Size must be integer multiple of device sector size
    CXBStorageDevice dev( GetDrive( devType ) );
    for( DWORD dwBufferSize = dev.GetSectorSize(); dwBufferSize <= MAX_BUFFER_SIZE; 
         dwBufferSize *= 2 )
    {
        // Open the test file in async mode
        File fileRead;
        VERIFY( fileRead.Open( fileTest.GetFileName(), GENERIC_READ,
                                       FILE_FLAG_OVERLAPPED | 
                                       FILE_FLAG_NO_BUFFERING ) );

        // Start reading from beginning
        m_Overlapped.Offset = 0;
        m_Overlapped.OffsetHigh = 0;

        DWORD dwTotalRead = 0;
        FLOAT fTotal = 0.0f;
        for( ;; )
        {
            // Offset must be integer multiple of sector size
            assert( m_Overlapped.Offset % dev.GetSectorSize() == 0 );

            // Initiate the read
            FLOAT fElapsed;
            VERIFY( !fileRead.ReadAsyncNotify( pBuffer, dwBufferSize, 
                                               fElapsed, m_Overlapped ) );
            assert( GetLastError() == ERROR_IO_PENDING );
            fTotal += fElapsed;

            // Other work would normally occur here, but we're benchmarking, so
            // don't want to influence results

            // Wait for the read to complete
            DWORD dwRead;
            VERIFY( fileRead.GetOverlappedResult( m_Overlapped, dwRead, 
                                                  fElapsed ) );
            assert( TestFile::IsValid( pBuffer, dwRead ) );
            fTotal += fElapsed;
            dwTotalRead += dwRead;
            if( dwTotalRead >= TEST_FILE_SIZE )
                break;

            // Read from the next section of the file
            m_Overlapped.Offset += dwBufferSize;

            ShowStatus( dwBufferSize, FLOAT(dwTotalRead) / FLOAT(TEST_FILE_SIZE) );
        }
        fileResults.Write( dwBufferSize, fTotal );
    }
}




//-----------------------------------------------------------------------------
// Name: AsyncReadCallback()
// Desc: Evaluate speed of asynchronous read w/ callback
//-----------------------------------------------------------------------------
VOID BenchStorage::AsyncReadCallback( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strASYNC_READ_CALLBACK_FILE, 
                  "AsyncReadCallback", L"Asynchronous Read with Callback",
                  "Buffer Size (bytes)", "Read Time (seconds)" ) )
        return;

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( MAX_BUFFER_SIZE );

    // Buffer address must be DWORD aligned
    assert( (DWORD)pBuffer % sizeof(DWORD) == 0 );

    // Create a pseudo kernel event that we can wait on
    AutoHandle hTransferComplete( CreateEvent( NULL, FALSE, FALSE, NULL ) );

    // Read the file using various buffer sizes.
    // Size must be integer multiple of device sector size
    CXBStorageDevice dev( GetDrive( devType ) );
    for( DWORD dwBufferSize = dev.GetSectorSize(); dwBufferSize <= MAX_BUFFER_SIZE; 
         dwBufferSize *= 2 )
    {
        // Open the test file in async mode
        File fileRead;
        VERIFY( fileRead.Open( fileTest.GetFileName(), GENERIC_READ,
                                       FILE_FLAG_OVERLAPPED | 
                                       FILE_FLAG_NO_BUFFERING ) );

        // Prepare the overlapped struct
        m_Overlapped.Offset = 0;
        m_Overlapped.OffsetHigh = 0;
        m_Overlapped.hEvent = this;

        DWORD dwTotalRead = 0;
        FLOAT fTotal = 0.0f;
        for( ;; )
        {
            // Offset must be integer multiple of sector size
            assert( m_Overlapped.Offset % dev.GetSectorSize() == 0 );

            // Initiate the read
            FLOAT fElapsed;
            VERIFY( fileRead.ReadAsyncCallback( pBuffer, dwBufferSize, 
                                                fElapsed, m_Overlapped, 
                                                BytesTransferred ) );
            assert( GetLastError() == ERROR_SUCCESS );
            fTotal += fElapsed;

            // Other work would normally occur here, but we're benchmarking, so
            // don't want to influence results

            // Wait for the read to complete. Note that hTransferComplete is never
            // signalled -- we're really waiting on the completion of the callback
            CXBStopWatch timer( TRUE );
            VERIFY( WaitForSingleObjectEx( hTransferComplete, INFINITE, TRUE )
                    == WAIT_IO_COMPLETION );

            assert( TestFile::IsValid( pBuffer, m_dwBytesTransferred ) );
            fTotal += timer.GetElapsedSeconds();
            dwTotalRead += m_dwBytesTransferred;
            if( dwTotalRead >= TEST_FILE_SIZE )
                break;

            // Read from the next section of the file
            m_Overlapped.Offset += dwBufferSize;

            ShowStatus( dwBufferSize, FLOAT(dwTotalRead) / FLOAT(TEST_FILE_SIZE) );
        }
        fileResults.Write( dwBufferSize, fTotal );
    }
}




//-----------------------------------------------------------------------------
// Name: AsyncWrite()
// Desc: Evaluate speed of asynchronous writes
//-----------------------------------------------------------------------------
VOID BenchStorage::AsyncWrite( DeviceType devType )
{
    AsyncWriteNotify( devType );
    AsyncWriteCallback( devType );
}




//-----------------------------------------------------------------------------
// Name: AsyncWriteNotify()
// Desc: Evaluate speed of asynchronous write w/ notification
//-----------------------------------------------------------------------------
VOID BenchStorage::AsyncWriteNotify( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strASYNC_WRITE_NOTIFY_FILE, 
                  "AsyncWriteNotify", L"Asynchronous Write with Notify",
                  "Buffer Size (bytes)", "Write Time (seconds)" ) )
        return;

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( MAX_BUFFER_SIZE );

    // Buffer address must be DWORD aligned
    assert( (DWORD)pBuffer % sizeof(DWORD) == 0 );

    // Prepare the notification event
    m_Overlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    AutoHandle hEvent( m_Overlapped.hEvent );

    // Write to the file using various buffer sizes.
    // Size must be integer multiple of device sector size
    CXBStorageDevice dev( GetDrive( devType ) );
    for( DWORD dwBufferSize = dev.GetSectorSize(); dwBufferSize <= MAX_BUFFER_SIZE; 
         dwBufferSize *= 2 )
    {
        // Open the test file in async mode
        File fileWrite;
        VERIFY( fileWrite.Open( fileTest.GetFileName(), GENERIC_WRITE,
                                FILE_FLAG_OVERLAPPED | 
                                FILE_FLAG_NO_BUFFERING ) );

        // Start writing from beginning
        m_Overlapped.Offset = 0;
        m_Overlapped.OffsetHigh = 0;

        DWORD dwTotalWritten = 0;
        FLOAT fTotal = 0.0f;
        for( ;; )
        {
            // Offset must be integer multiple of sector size
            assert( m_Overlapped.Offset % dev.GetSectorSize() == 0 );

            // Initiate the write
            FLOAT fElapsed;
            VERIFY( !fileWrite.WriteAsyncNotify( pBuffer, dwBufferSize, 
                                                 fElapsed, m_Overlapped ) );
            assert( GetLastError() == ERROR_IO_PENDING );
            fTotal += fElapsed;

            // Other work would normally occur here, but we're benchmarking, so
            // don't want to influence results

            // Wait for the write to complete
            DWORD dwWritten;
            VERIFY( fileWrite.GetOverlappedResult( m_Overlapped, dwWritten, 
                                                   fElapsed ) );
            fTotal += fElapsed;
            dwTotalWritten += dwWritten;
            if( dwTotalWritten >= TEST_FILE_SIZE )
                break;

            // Write to the next section of the file
            m_Overlapped.Offset += dwBufferSize;

            ShowStatus( dwBufferSize, FLOAT(dwTotalWritten) / FLOAT(TEST_FILE_SIZE) );
        }
        fileResults.Write( dwBufferSize, fTotal );
    }
}




//-----------------------------------------------------------------------------
// Name: AsyncWriteCallback()
// Desc: Evaluate speed of asynchronous write w/ callback
//-----------------------------------------------------------------------------
VOID BenchStorage::AsyncWriteCallback( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strASYNC_WRITE_CALLBACK_FILE, 
                  "AsyncWriteCallback", L"Asynchronous Write with Callback",
                  "Buffer Size (bytes)", "Write Time (seconds)" ) )
        return;

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( MAX_BUFFER_SIZE );

    // Buffer address must be DWORD aligned
    assert( (DWORD)pBuffer % sizeof(DWORD) == 0 );

    // Create a pseudo kernel event that we can wait on
    AutoHandle hTransferComplete( CreateEvent( NULL, FALSE, FALSE, NULL ) );

    // Write to the file using various buffer sizes.
    // Size must be integer multiple of device sector size
    CXBStorageDevice dev( GetDrive( devType ) );
    for( DWORD dwBufferSize = dev.GetSectorSize(); dwBufferSize <= MAX_BUFFER_SIZE; 
         dwBufferSize *= 2 )
    {
        // Open the test file in async mode
        File fileWrite;
        VERIFY( fileWrite.Open( fileTest.GetFileName(), GENERIC_WRITE,
                                FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING ) );

        // Prepare the overlapped struct
        m_Overlapped.Offset = 0;
        m_Overlapped.OffsetHigh = 0;
        m_Overlapped.hEvent = this;

        DWORD dwTotalWritten = 0;
        FLOAT fTotal = 0.0f;
        for( ;; )
        {
            // Offset must be integer multiple of sector size
            assert( m_Overlapped.Offset % dev.GetSectorSize() == 0 );

            // Initiate the write
            FLOAT fElapsed;
            VERIFY( fileWrite.WriteAsyncCallback( pBuffer, dwBufferSize, 
                                                  fElapsed, m_Overlapped, 
                                                  BytesTransferred ) );
            assert( GetLastError() == ERROR_SUCCESS );
            fTotal += fElapsed;

            // Other work would normally occur here, but we're benchmarking, so
            // don't want to influence results

            // Wait for the write to complete. Note that hTransferComplete is never
            // signalled -- we're really waiting on the completion of the callback
            CXBStopWatch timer( TRUE );
            VERIFY( WaitForSingleObjectEx( hTransferComplete, INFINITE, TRUE )
                    == WAIT_IO_COMPLETION );

            fTotal += timer.GetElapsedSeconds();
            dwTotalWritten += m_dwBytesTransferred;
            if( dwTotalWritten >= TEST_FILE_SIZE )
                break;

            // Write to the next section of the file
            m_Overlapped.Offset += dwBufferSize;

            ShowStatus( dwBufferSize, FLOAT(dwTotalWritten) / FLOAT(TEST_FILE_SIZE) );
        }
        fileResults.Write( dwBufferSize, fTotal );
    }
}




//-----------------------------------------------------------------------------
// Name: ApiCompare()
// Desc: Compare Xbox vs C vs C++ API overhead
//-----------------------------------------------------------------------------
VOID BenchStorage::ApiCompare( DeviceType devType )
{
    ApiOpen( devType );
    ApiClose( devType );
    ApiRead( devType );
    ApiWrite( devType );
    ApiSeek( devType );
}




//-----------------------------------------------------------------------------
// Name: ApiOpen()
// Desc: Compare open routines
//-----------------------------------------------------------------------------
VOID BenchStorage::ApiOpen( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strAPI_OPEN_FILE, 
                  "ApiOpen", L"Compare open APIs",
                  "Number of attempts", "Native CreateFile (seconds)",
                  "C fopen (seconds)", "C++ filebuf open (seconds)" ) )
        return;

    CXBStopWatch NativeTimer( FALSE );
    CXBStopWatch CTimer( FALSE );
    CXBStopWatch CppTimer( FALSE );
    for( DWORD i = 0; i < API_PASSES; ++i )
    {
        // Xbox API
        NativeTimer.Start();
        HANDLE hFile = CreateFile( fileTest.GetFileName(), 
                                   GENERIC_READ | GENERIC_WRITE, 0,
                                   NULL, OPEN_ALWAYS, 0, NULL );
        NativeTimer.Stop();
        assert( hFile != INVALID_HANDLE_VALUE );
        CloseHandle( hFile );

        // Standard C
        CTimer.Start();
        FILE* pFile = fopen( fileTest.GetFileName(), "rb+" );
        CTimer.Stop();
        assert( pFile != NULL );
        fclose( pFile );

        // Standard C++
        using namespace std;
        basic_fstream<BYTE> file;
        CppTimer.Start();
        file.open( fileTest.GetFileName(), ios::in | ios::out | ios::binary );
        CppTimer.Stop();
        assert( file.is_open() );
        file.close();

        ShowStatus( FLOAT( i ) / FLOAT( API_PASSES ) );
    }
    fileResults.Write( API_PASSES, NativeTimer.GetElapsedSeconds(),
                       CTimer.GetElapsedSeconds(), CppTimer.GetElapsedSeconds() );
}




//-----------------------------------------------------------------------------
// Name: ApiClose()
// Desc: Compare close routines
//-----------------------------------------------------------------------------
VOID BenchStorage::ApiClose( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strAPI_CLOSE_FILE, 
                  "ApiClose", L"Compare close APIs", "Number of attempts",
                  "Native CloseHandle (seconds)", "C fclose (seconds)", 
                  "C++ filebuf close (seconds)" ) )
        return;

    CXBStopWatch NativeTimer( FALSE );
    CXBStopWatch CTimer( FALSE );
    CXBStopWatch CppTimer( FALSE );
    for( DWORD i = 0; i < API_PASSES; ++i )
    {
        // Xbox API
        HANDLE hFile = CreateFile( fileTest.GetFileName(), 
                                   GENERIC_READ | GENERIC_WRITE, 0,
                                   NULL, OPEN_ALWAYS, 0, NULL );
        assert( hFile != INVALID_HANDLE_VALUE );
        NativeTimer.Start();
        CloseHandle( hFile );
        NativeTimer.Stop();

        // Standard C
        FILE* pFile = fopen( fileTest.GetFileName(), "rb+" );
        assert( pFile != NULL );
        CTimer.Start();
        fclose( pFile );
        CTimer.Stop();

        // Standard C++
        using namespace std;
        basic_fstream<BYTE> file;
        file.open( fileTest.GetFileName(), ios::in | ios::out | ios::binary );
        assert( file.is_open() );
        CppTimer.Start();
        file.close();
        CppTimer.Stop();

        ShowStatus( FLOAT( i ) / FLOAT( API_PASSES ) );
    }
    fileResults.Write( API_PASSES, NativeTimer.GetElapsedSeconds(),
                       CTimer.GetElapsedSeconds(), CppTimer.GetElapsedSeconds() );
}




//-----------------------------------------------------------------------------
// Name: ApiRead()
// Desc: Compare read routines
//-----------------------------------------------------------------------------
VOID BenchStorage::ApiRead( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strAPI_READ_FILE, 
                  "ApiRead", L"Compare read APIs", "Buffer Size (bytes)",
                  "Native ReadFile (seconds)", "C fread (seconds)", 
                  "C++ fstream read (seconds)" ) )
        return;

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( TEST_FILE_SIZE );

    for( DWORD dwBufferSize = MIN_BUFFER_SIZE_API; dwBufferSize <= MAX_BUFFER_SIZE; 
         dwBufferSize *= 2 )
    {
        DWORD dwTotalRead = 0;

        // Xbox method
        DWORD dwXboxTotalRead = 0;
        FLOAT fTotal = 0.0f;
        File fileRead;
        VERIFY( fileRead.Open( fileTest.GetFileName(), GENERIC_READ ) );
        for( ;; )
        {
            // ReadFile API
            DWORD dwBytesRead;
            FLOAT fElapsed;
            VERIFY( fileRead.Read( pBuffer, dwBufferSize, 
                                   dwBytesRead, fElapsed ) );
            assert( dwBytesRead == dwBufferSize );
            assert( TestFile::IsValid( pBuffer, dwBytesRead ) );
            fTotal += fElapsed;
            dwXboxTotalRead += dwBytesRead;
            if( dwXboxTotalRead >= TEST_FILE_SIZE )
                break;

            dwTotalRead += dwBytesRead;
            ShowStatus( dwBufferSize, FLOAT(dwTotalRead) / FLOAT(TEST_FILE_SIZE*3) );
        }
        fileRead.Close();

        // Standard C
        DWORD dwCTotalRead = 0;
        CXBStopWatch CTimer( FALSE );
        FILE* pFile = fopen( fileTest.GetFileName(), "rb+" );
        assert( pFile != NULL );
        for( ;; )
        {
            // fread API
            CTimer.Start();
            size_t nBytes = fread( pBuffer, 1, dwBufferSize, pFile );
            CTimer.Stop();
            assert( nBytes == dwBufferSize );
            assert( TestFile::IsValid( pBuffer, nBytes ) );
            dwCTotalRead += nBytes;
            if( dwCTotalRead >= TEST_FILE_SIZE )
                break;

            dwTotalRead += nBytes;
            ShowStatus( dwBufferSize, FLOAT(dwTotalRead) / FLOAT(TEST_FILE_SIZE*3) );
        }
        fclose( pFile );

        // Standard C++
        DWORD dwCppTotalRead = 0;
        CXBStopWatch CppTimer( FALSE );
        using namespace std;
        basic_fstream<BYTE> file;
        file.open( fileTest.GetFileName(), ios::in | ios::out | ios::binary );
        assert( file.is_open() );
        for( ;; )
        {
            // read API
            CppTimer.Start();
            file.read( pBuffer, dwBufferSize );
            CppTimer.Stop();
            assert( file.good() );
            assert( TestFile::IsValid( pBuffer, dwBufferSize ) );
            dwCppTotalRead += dwBufferSize;
            if( dwCppTotalRead >= TEST_FILE_SIZE )
                break;

            dwTotalRead += dwBufferSize;
            ShowStatus( dwBufferSize, FLOAT(dwTotalRead) / FLOAT(TEST_FILE_SIZE*3) );
        }
        file.close();

        fileResults.Write( dwBufferSize, fTotal, CTimer.GetElapsedSeconds(), 
                           CppTimer.GetElapsedSeconds() );
    }
}




//-----------------------------------------------------------------------------
// Name: ApiWrite()
// Desc: Compare write routines
//-----------------------------------------------------------------------------
VOID BenchStorage::ApiWrite( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strAPI_WRITE_FILE, 
                  "ApiWrite", L"Compare write APIs", "Buffer Size (bytes)",
                  "Native WriteFile (seconds)", "C fwrite (seconds)", 
                  "C++ fstream write (seconds)" ) )
        return;

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( TEST_FILE_SIZE );

    for( DWORD dwBufferSize = MIN_BUFFER_SIZE_API; dwBufferSize <= MAX_BUFFER_SIZE; 
         dwBufferSize *= 2 )
    {
        DWORD dwTotalWritten = 0;

        // Xbox method
        DWORD dwXboxTotalWritten = 0;
        FLOAT fTotal = 0.0f;
        File fileWrite;
        VERIFY( fileWrite.Open( fileTest.GetFileName(), GENERIC_WRITE ) );
        for( ;; )
        {
            // WriteFile API
            DWORD dwBytesWritten;
            FLOAT fElapsed;
            VERIFY( fileWrite.Write( pBuffer, dwBufferSize, 
                                     dwBytesWritten, fElapsed ) );
            assert( dwBytesWritten == dwBufferSize );
            fTotal += fElapsed;
            dwXboxTotalWritten += dwBytesWritten;
            if( dwXboxTotalWritten >= TEST_FILE_SIZE )
                break;

            dwTotalWritten += dwBytesWritten;
            ShowStatus( dwBufferSize, FLOAT(dwTotalWritten) / FLOAT(TEST_FILE_SIZE*3) );
        }
        fileWrite.Close();

        // Standard C
        DWORD dwCTotalWritten = 0;
        CXBStopWatch CTimer( FALSE );
        FILE* pFile = fopen( fileTest.GetFileName(), "rb+" );
        assert( pFile != NULL );
        for( ;; )
        {
            // fwrite API
            CTimer.Start();
            size_t nBytes = fwrite( pBuffer, 1, dwBufferSize, pFile );
            CTimer.Stop();
            assert( nBytes == dwBufferSize );
            dwCTotalWritten += nBytes;
            if( dwCTotalWritten >= TEST_FILE_SIZE )
                break;

            dwTotalWritten += nBytes;
            ShowStatus( dwBufferSize, FLOAT(dwTotalWritten) / FLOAT(TEST_FILE_SIZE*3) );
        }
        fclose( pFile );

        // Standard C++
        DWORD dwCppTotalWritten = 0;
        CXBStopWatch CppTimer( FALSE );
        using namespace std;
        basic_fstream<BYTE> file;
        file.open( fileTest.GetFileName(), ios::in | ios::out | ios::binary );
        assert( file.is_open() );
        for( ;; )
        {
            // write API
            CppTimer.Start();
            file.write( pBuffer, dwBufferSize );
            CppTimer.Stop();
            assert( file.good() );
            dwCppTotalWritten += dwBufferSize;
            if( dwCppTotalWritten >= TEST_FILE_SIZE )
                break;

            dwTotalWritten += dwBufferSize;
            ShowStatus( dwBufferSize, FLOAT(dwTotalWritten) / FLOAT(TEST_FILE_SIZE*3) );
        }
        file.close();

        fileResults.Write( dwBufferSize, fTotal, CTimer.GetElapsedSeconds(), 
                           CppTimer.GetElapsedSeconds() );
    }
}




//-----------------------------------------------------------------------------
// Name: ApiSeek()
// Desc: Compare seek routines
//-----------------------------------------------------------------------------
VOID BenchStorage::ApiSeek( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strAPI_SEEK_FILE, 
                  "ApiSeek", L"Compare seek APIs", "Number of attempts",
                  "Native SetFilePos (seconds)", "C fseek (seconds)", 
                  "C++ fstream seek (seconds)" ) )
        return;

    // Construct a list of random seek offsets to be used for all test passes
    std::vector<DWORD> SeekList;
    SeekList.reserve( API_PASSES );
    srand( m_dwSeed );
    DWORD dwLastOffset = 0;
    for( DWORD i = 0; i < API_PASSES; ++i )
    {
        // Generate a random new seek position
        INT iPos = rand() * rand();

        // Determine if we should seek forward or backward.
        // Tend to progress forward more often than backward
        BOOL bForward = ( rand() % 3 ) > 0;

        // Wrap to beginning if go past the end of the file
        DWORD dwOffset = dwLastOffset + ( bForward ? iPos : -iPos );
        if( dwOffset > TEST_FILE_SIZE )
            dwOffset = 0;
        dwLastOffset = dwOffset;

        SeekList.push_back( dwOffset );
    }

    // Create a small buffer
    BYTE pBuffer[4];

    DWORD dwPasses = 0;

    // Xbox method
    CXBStopWatch XboxTimer( FALSE );
    File fileRead;
    VERIFY( fileRead.Open( fileTest.GetFileName(), GENERIC_READ ) );
    for( DWORD i = 0; i < API_PASSES; ++i, ++dwPasses )
    {
        DWORD dwOffset = SeekList[i];

        // SetFilePos
        XboxTimer.Start();
        fileRead.SetPos( dwOffset );
        XboxTimer.Stop();

        // Do a read to force the seek to occur
        DWORD dwBytesRead;
        FLOAT fElapsed;
        VERIFY( fileRead.Read( pBuffer, 4, dwBytesRead, fElapsed ) );
        ShowStatus( FLOAT(dwPasses) / FLOAT(API_PASSES*3) );
    }
    fileRead.Close();

    // Standard C
    CXBStopWatch CTimer( FALSE );
    FILE* pFile = fopen( fileTest.GetFileName(), "rb+" );
    assert( pFile != NULL );
    for( DWORD i = 0; i < API_PASSES; ++i, ++dwPasses )
    {
        DWORD dwOffset = SeekList[i];

        // fseek API
        CTimer.Start();
        VERIFY( fseek( pFile, dwOffset, SEEK_SET ) == 0 );
        CTimer.Stop();

        // Do a read to force the seek to occur
        VERIFY( fread( pBuffer, 1, 4, pFile ) == 4 );
        ShowStatus( FLOAT(dwPasses) / FLOAT(API_PASSES*3) );
    }
    fclose( pFile );

    // Standard C++
    CXBStopWatch CppTimer( FALSE );
    using namespace std;
    basic_fstream<BYTE> file;
    file.open( fileTest.GetFileName(), ios::in | ios::out | ios::binary );
    assert( file.is_open() );
    for( DWORD i = 0; i < API_PASSES; ++i, ++dwPasses )
    {
        DWORD dwOffset = SeekList[i];

        // seek API
        CppTimer.Start();
        file.seekg( dwOffset );
        CppTimer.Stop();
        assert( file.good() );

        // Do a read to force the seek to occur
        file.read( pBuffer, 4 );
        assert( file.good() );
        ShowStatus( FLOAT(dwPasses) / FLOAT(API_PASSES*3) );
    }
    file.close();

    fileResults.Write( API_PASSES, XboxTimer.GetElapsedSeconds(), 
                       CTimer.GetElapsedSeconds(),
                       CppTimer.GetElapsedSeconds() );
}




//-----------------------------------------------------------------------------
// Name: RandomSeek()
// Desc: Evaluate random seek time within a file
//-----------------------------------------------------------------------------
VOID BenchStorage::RandomSeek( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strRANDOM_SEEK_FILE, 
                  "RandomSeek", L"Random Seek within a File", 
                  "Seeks", "Total Seek Time (seconds)" ) )
        return;

    File fileRead;
    VERIFY( fileRead.Open( fileTest.GetFileName(), GENERIC_READ ) );

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( MAX_BUFFER_SIZE );

    DWORD dwSeeks = ( devType == Device_MU ) ? MAX_RANDOM_SEEKS / 10 :
                                               MAX_RANDOM_SEEKS;

    // Seek to various positions
    srand( m_dwSeed );
    FLOAT fTotal = 0.0f;
    for( DWORD i = 0; i < dwSeeks; ++i )
    {
        // Get random position
        INT iPos = rand() * rand();

        // Restrict offset to file size
        DWORD dwOffset = iPos % TEST_FILE_SIZE;

        // Time the seek
        CXBStopWatch timer( TRUE );
        fileRead.SetPos( dwOffset );
        fTotal += timer.GetElapsedSeconds();

        // Do a read to "force" the seek
        DWORD dwBytesRead;
        FLOAT fElapsed;
        VERIFY( fileRead.Read( pBuffer, 1, dwBytesRead, fElapsed ) );

        swprintf( m_strStatus, L"Progress: %.0f %%",
                  100.0f * FLOAT(i) / FLOAT(dwSeeks) );
        ShowStatus();
    }

    fileResults.Write( dwSeeks, fTotal );
}




//-----------------------------------------------------------------------------
// Name: SeqSeek()
// Desc: Evaluate random seek time within a file
//-----------------------------------------------------------------------------
VOID BenchStorage::SeqSeek( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strSEQ_SEEK_FILE, 
                  "SeqSeek", L"Sequential Seek within a File", 
                  "Seeks", "Total Seek Time (seconds)" ) )
        return;

    File fileRead;
    VERIFY( fileRead.Open( fileTest.GetFileName(), GENERIC_READ ) );

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( MAX_BUFFER_SIZE );

    DWORD dwSeeks = ( devType == Device_MU ) ? MAX_SEQUENTIAL_SEEKS / 10 :
                                               MAX_SEQUENTIAL_SEEKS;

    // Seek to various positions
    srand( m_dwSeed );
    FLOAT fTotal = 0.0f;
    DWORD dwLastOffset = 0;
    for( DWORD i = 0; i < dwSeeks; ++i )
    {
        // Generate a random new sequential position.
        // Wrap to beginning if go past the end of the file.
        INT iPos = rand();
        DWORD dwOffset = dwLastOffset + iPos;
        if( dwOffset > TEST_FILE_SIZE )
            dwOffset = 0;
        dwLastOffset = dwOffset;

        // Time the seek
        CXBStopWatch timer( TRUE );
        fileRead.SetPos( dwOffset );
        fTotal += timer.GetElapsedSeconds();

        // Do a read to "force" the seek
        DWORD dwBytesRead;
        FLOAT fElapsed;
        VERIFY( fileRead.Read( pBuffer, 1, dwBytesRead, fElapsed ) );

        swprintf( m_strStatus, L"Progress: %.0f %%",
                  100.0f * FLOAT(i) / FLOAT(dwSeeks) );
        ShowStatus();
    }

    fileResults.Write( dwSeeks, fTotal );
}




//-----------------------------------------------------------------------------
// Name: RandomRead()
// Desc: Evaluate random seeks/reads within a file with and without the
//       FILE_FLAG_RANDOM_ACCESS setting
//-----------------------------------------------------------------------------
VOID BenchStorage::RandomRead( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strRANDOM_READ_FILE, 
                  "RandomRead", L"Random Read within a File",
                  "Read Time w/ no flags (seconds)", 
                  "Read Time w/ FILE_FLAG_SEQUENTIAL_SCAN (seconds)", 
                  "Read Time w/ FILE_FLAG_RANDOM_ACCESS (seconds)" ) )
        return;

    DWORD dwReads = ( devType == Device_MU ) ? MAX_RANDOM_READS / 10 :
                                               MAX_RANDOM_READS;

    FLOAT fTotalNone = RandomRead( fileTest, dwReads, 0, L"<empty>" );
    FLOAT fTotalSeq  = RandomRead( fileTest, dwReads, FILE_FLAG_SEQUENTIAL_SCAN, 
                                   L"FILE_FLAG_SEQUENTIAL_SCAN" );
    FLOAT fTotalRand = RandomRead( fileTest, dwReads, FILE_FLAG_RANDOM_ACCESS, 
                                   L"FILE_FLAG_RANDOM_ACCESS" );

    fileResults.Write( fTotalNone, fTotalSeq, fTotalRand );
}




//-----------------------------------------------------------------------------
// Name: RandomRead()
// Desc: Perform random seeks and reads within a file and return the
//       elapsed time
//-----------------------------------------------------------------------------
FLOAT BenchStorage::RandomRead( const TestFile& fileTest, DWORD dwReads,
                                DWORD dwFlags, const WCHAR* strFlag, 
                                DWORD dwCache )
{
    File fileRead;
    VERIFY( fileRead.Open( fileTest.GetFileName(), GENERIC_READ, dwFlags ) );

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( MAX_BUFFER_SIZE );

    // Read various positions and buffer sizes
    srand( m_dwSeed );
    FLOAT fTotal = 0.0f;
    for( DWORD i = 0; i < dwReads; ++i )
    {
        // Get random position and size
        INT iPos = rand() * rand();
        INT iSize = rand() * rand();

        // Restrict offset and size
        DWORD dwOffset = iPos % TEST_FILE_SIZE;
        DWORD dwLength = iSize % MAX_BUFFER_SIZE;

        // Time the seek
        CXBStopWatch timer( TRUE );
        fileRead.SetPos( dwOffset );
        fTotal += timer.GetElapsedSeconds();

        // Time the read
        DWORD dwBytesRead;
        FLOAT fElapsed;
        VERIFY( fileRead.Read( pBuffer, dwLength, dwBytesRead, fElapsed ) );
        fTotal += fElapsed;

        if( dwCache )
        {
            swprintf( m_strStatus, L"Cache size: %lu\nProgress: %.0f %%",
                      dwCache, 100.0f * FLOAT(i) / FLOAT(dwReads) );
        }
        else
        {
            swprintf( m_strStatus, L"Flag: %s\nProgress: %.0f %%",
                      strFlag, 100.0f * FLOAT(i) / FLOAT(dwReads) );
        }
        ShowStatus();
    }
    return fTotal;
}




//-----------------------------------------------------------------------------
// Name: RandomWrite()
// Desc: Evaluate random seeks/writes within a file with and without the
//       FILE_FLAG_RANDOM_ACCESS setting
//-----------------------------------------------------------------------------
VOID BenchStorage::RandomWrite( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strRANDOM_WRITE_FILE, 
                  "RandomWrite", L"Random Write within a File",
                  "Write Time w/ no flags (seconds)", 
                  "Write Time w/ FILE_FLAG_SEQUENTIAL_SCAN (seconds)", 
                  "Write Time w/ FILE_FLAG_RANDOM_ACCESS (seconds)" ) )
        return;

    DWORD dwWrites = ( devType == Device_MU ) ? MAX_RANDOM_WRITES / 10 :
                                                MAX_RANDOM_WRITES;

    FLOAT fTotalNone = RandomWrite( fileTest, dwWrites, 0, L"<empty>" );
    FLOAT fTotalSeq  = RandomWrite( fileTest, dwWrites, FILE_FLAG_SEQUENTIAL_SCAN, 
                                    L"FILE_FLAG_SEQUENTIAL_SCAN" );
    FLOAT fTotalRand = RandomWrite( fileTest, dwWrites, FILE_FLAG_RANDOM_ACCESS, 
                                    L"FILE_FLAG_RANDOM_ACCESS" );

    fileResults.Write( fTotalNone, fTotalSeq, fTotalRand );
}




//-----------------------------------------------------------------------------
// Name: RandomWrite()
// Desc: Perform random seeks and writes within a file and return the
//       elapsed time
//-----------------------------------------------------------------------------
FLOAT BenchStorage::RandomWrite( const TestFile& fileTest, DWORD dwWrites,
                                 DWORD dwFlags, const WCHAR* strFlag, 
                                 DWORD dwCache )
{
    File fileWrite;
    VERIFY( fileWrite.Open( fileTest.GetFileName(), GENERIC_WRITE, dwFlags ) );

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( MAX_BUFFER_SIZE );

    // Write various positions and buffer sizes
    srand( m_dwSeed );
    FLOAT fTotal = 0.0f;
    for( DWORD i = 0; i < dwWrites; ++i )
    {
        // Get random position and size
        INT iPos = rand() * rand();
        INT iSize = rand() * rand();

        // Restrict offset and size
        DWORD dwOffset = iPos % TEST_FILE_SIZE;
        DWORD dwLength = iSize % MAX_BUFFER_SIZE;

        // Time the seek
        CXBStopWatch timer( TRUE );
        fileWrite.SetPos( dwOffset );
        fTotal += timer.GetElapsedSeconds();

        // Time the write
        DWORD dwBytesWritten;
        FLOAT fElapsed;
        VERIFY( fileWrite.Write( pBuffer, dwLength, dwBytesWritten, fElapsed ) );
        fTotal += fElapsed;

        if( dwCache )
        {
            swprintf( m_strStatus, L"Cache size: %lu\nProgress: %.0f %%",
                      dwCache, 100.0f * FLOAT(i) / FLOAT(dwWrites) );
        }
        else
        {
            swprintf( m_strStatus, L"Flag: %s\nProgress: %.0f %%",
                      strFlag, 100.0f * FLOAT(i) / FLOAT(dwWrites) );
        }
        ShowStatus();
    }
    return fTotal;
}




//-----------------------------------------------------------------------------
// Name: SeqRead()
// Desc: Evaluate sequential reads within a file with and without the
//       FILE_FLAG_SEQUENTIAL_SCAN setting
//-----------------------------------------------------------------------------
VOID BenchStorage::SeqRead( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strSEQ_READ_FILE, 
                  "SequentialRead", L"Sequential Read within a File",
                  "Read Time w/ no flags (seconds)", 
                  "Read Time w/ FILE_FLAG_SEQUENTIAL_SCAN (seconds)", 
                  "Read Time w/ FILE_FLAG_RANDOM_ACCESS (seconds)" ) )
        return;

    DWORD dwReads = ( devType == Device_MU ) ? MAX_SEQUENTIAL_READS / 10 :
                                               MAX_SEQUENTIAL_READS;

    FLOAT fTotalNone = SeqRead( fileTest, dwReads, 0, L"<empty>" );
    FLOAT fTotalSeq  = SeqRead( fileTest, dwReads, FILE_FLAG_SEQUENTIAL_SCAN, 
                                L"FILE_FLAG_SEQUENTIAL_SCAN" );
    FLOAT fTotalRand = SeqRead( fileTest, dwReads, FILE_FLAG_RANDOM_ACCESS, 
                                L"FILE_FLAG_RANDOM_ACCESS" );

    fileResults.Write( fTotalNone, fTotalSeq, fTotalRand );
}




//-----------------------------------------------------------------------------
// Name: SeqRead()
// Desc: Perform sequential seeks and reads within a file and return the
//       elapsed time
//-----------------------------------------------------------------------------
FLOAT BenchStorage::SeqRead( const TestFile& fileTest, DWORD dwReads,
                             DWORD dwFlags, const WCHAR* strFlag, DWORD dwCache )
{
    File fileRead;
    VERIFY( fileRead.Open( fileTest.GetFileName(), GENERIC_READ, dwFlags ) );

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( MAX_BUFFER_SIZE );

    // Read various sequential positions and buffer sizes
    srand( m_dwSeed );
    FLOAT fTotal = 0.0f;
    DWORD dwLastOffset = 0;
    for( DWORD i = 0; i < dwReads; ++i )
    {
        // Generate a random new sequential position.
        // Wrap to beginning if go past the end of the file.
        INT iPos = rand();
        DWORD dwOffset = dwLastOffset + iPos;
        if( dwOffset > TEST_FILE_SIZE )
            dwOffset = 0;
        dwLastOffset = dwOffset;

        // Get random size
        INT iSize = rand();
        DWORD dwLength = iSize % MAX_BUFFER_SIZE;

        // Time the seek
        CXBStopWatch timer( TRUE );
        fileRead.SetPos( dwOffset );
        fTotal += timer.GetElapsedSeconds();

        // Time the read
        DWORD dwBytesRead;
        FLOAT fElapsed;
        VERIFY( fileRead.Read( pBuffer, dwLength, dwBytesRead, fElapsed ) );
        fTotal += fElapsed;

        if( dwCache )
        {
            swprintf( m_strStatus, L"Cache size: %lu\nProgress: %.0f %%",
                      dwCache, 100.0f * FLOAT(i) / FLOAT(dwReads) );
        }
        else
        {
            swprintf( m_strStatus, L"Flag: %s\nProgress: %.0f %%",
                      strFlag, 100.0f * FLOAT(i) / FLOAT(dwReads) );
        }
        ShowStatus();
    }
    return fTotal;
}




//-----------------------------------------------------------------------------
// Name: SeqWrite()
// Desc: Evaluate sequential writes within a file with and without the
//       FILE_FLAG_SEQUENTIAL_SCAN setting
//-----------------------------------------------------------------------------
VOID BenchStorage::SeqWrite( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strSEQ_WRITE_FILE, 
                  "SequentialWrite", L"Sequential Write within a File",
                  "Write Time w/ no flags (seconds)", 
                  "Write Time w/ FILE_FLAG_SEQUENTIAL_SCAN (seconds)", 
                  "Write Time w/ FILE_FLAG_RANDOM_ACCESS (seconds)" ) )
        return;

    DWORD dwWrites = ( devType == Device_MU ) ? MAX_SEQUENTIAL_WRITES / 10 :
                                                MAX_SEQUENTIAL_WRITES;

    FLOAT fTotalNone = SeqWrite( fileTest, dwWrites, 0, L"<empty>" );
    FLOAT fTotalSeq  = SeqWrite( fileTest, dwWrites, FILE_FLAG_SEQUENTIAL_SCAN, 
                                 L"FILE_FLAG_SEQUENTIAL_SCAN" );
    FLOAT fTotalRand = SeqWrite( fileTest, dwWrites, FILE_FLAG_RANDOM_ACCESS, 
                                 L"FILE_FLAG_RANDOM_ACCESS" );

    fileResults.Write( fTotalNone, fTotalSeq, fTotalRand );
}




//-----------------------------------------------------------------------------
// Name: SeqWrite()
// Desc: Perform sequential seeks and writeswithin a file and return the
//       elapsed time
//-----------------------------------------------------------------------------
FLOAT BenchStorage::SeqWrite( const TestFile& fileTest, DWORD dwWrites,
                              DWORD dwFlags, const WCHAR* strFlag, DWORD dwCache )
{
    File fileWrite;
    VERIFY( fileWrite.Open( fileTest.GetFileName(), GENERIC_WRITE, dwFlags ) );

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( MAX_BUFFER_SIZE );

    // Write various sequential positions and buffer sizes
    srand( m_dwSeed );
    FLOAT fTotal = 0.0f;
    DWORD dwLastOffset = 0;
    for( DWORD i = 0; i < dwWrites; ++i )
    {
        // Generate a random new sequential position.
        // Wrap to beginning if go past the end of the file.
        INT iPos = rand();
        DWORD dwOffset = dwLastOffset + iPos;
        if( dwOffset > TEST_FILE_SIZE )
            dwOffset = 0;
        dwLastOffset = dwOffset;

        // Get random size
        INT iSize = rand();
        DWORD dwLength = iSize % MAX_BUFFER_SIZE;

        // Time the seek
        CXBStopWatch timer( TRUE );
        fileWrite.SetPos( dwOffset );
        fTotal += timer.GetElapsedSeconds();

        // Time the write
        DWORD dwBytesWritten;
        FLOAT fElapsed;
        VERIFY( fileWrite.Write( pBuffer, dwLength, dwBytesWritten, fElapsed ) );
        fTotal += fElapsed;

        if( dwCache )
        {
            swprintf( m_strStatus, L"Cache size: %lu\nProgress: %.0f %%",
                      dwCache, 100.0f * FLOAT(i) / FLOAT(dwWrites) );
        }
        else
        {
            swprintf( m_strStatus, L"Flag: %s\nProgress: %.0f %%",
                      strFlag, 100.0f * FLOAT(i) / FLOAT(dwWrites) );
        }
        ShowStatus();
    }
    return fTotal;
}




//-----------------------------------------------------------------------------
// Name: CacheRead()
// Desc: Evaluate cache performance for various read situations
//-----------------------------------------------------------------------------
VOID BenchStorage::CacheRead( DeviceType devType )
{
    CacheRandomRead( devType );
    CacheSeqRead( devType );
}




//-----------------------------------------------------------------------------
// Name: CacheRandomRead()
// Desc: Evaluate cache performance for random reads
//-----------------------------------------------------------------------------
VOID BenchStorage::CacheRandomRead( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strCACHE_RANDOM_READ_FILE, 
                  "CacheRandomRead", L"Cache Random Read within a File",
                  "Read Time w/ FILE_FLAG_RANDOM_ACCESS (seconds)" ) )
        return;

    DWORD dwReads = ( devType == Device_MU ) ? MAX_RANDOM_READS / 10 :
                                               MAX_RANDOM_READS;

    // Use default size first
    VERIFY( XSetFileCacheSize( m_dwDefaultCacheSize ) );
    FLOAT fElapsed = RandomRead( fileTest, dwReads, FILE_FLAG_RANDOM_ACCESS, L"",
                                 m_dwDefaultCacheSize );
    fileResults.Write( m_dwDefaultCacheSize, fElapsed );

    // Try some different sizes    
    for( DWORD i = MIN_CACHE_SIZE; i <= MAX_CACHE_SIZE; i *= 2 )
    {
        VERIFY( XSetFileCacheSize( i ) );
        fElapsed = RandomRead( fileTest, dwReads, FILE_FLAG_RANDOM_ACCESS, 
                               L"", i );
        fileResults.Write( i, fElapsed );
    }

    // Restore default
    VERIFY( XSetFileCacheSize( m_dwDefaultCacheSize ) );
}




//-----------------------------------------------------------------------------
// Name: CacheSeqRead()
// Desc: Evaluate cache performance for sequential reads
//-----------------------------------------------------------------------------
VOID BenchStorage::CacheSeqRead( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strCACHE_SEQ_READ_FILE, 
                  "CacheSeqRead", L"Cache Sequential Read within a File",
                  "Read Time w/ FILE_FLAG_SEQUENTIAL_SCAN (seconds)" ) )
        return;

    DWORD dwReads = ( devType == Device_MU ) ? MAX_SEQUENTIAL_READS / 10 :
                                               MAX_SEQUENTIAL_READS;

    // Use default size first
    VERIFY( XSetFileCacheSize( m_dwDefaultCacheSize ) );
    FLOAT fElapsed = SeqRead( fileTest, dwReads, FILE_FLAG_SEQUENTIAL_SCAN, L"",
                              m_dwDefaultCacheSize );
    fileResults.Write( m_dwDefaultCacheSize, fElapsed );

    // Try some different sizes    
    for( DWORD i = MIN_CACHE_SIZE; i <= MAX_CACHE_SIZE; i *= 2 )
    {
        VERIFY( XSetFileCacheSize( i ) );
        fElapsed = SeqRead( fileTest, dwReads, FILE_FLAG_SEQUENTIAL_SCAN, L"", i );
        fileResults.Write( i, fElapsed );
    }

    // Restore default
    VERIFY( XSetFileCacheSize( m_dwDefaultCacheSize ) );
}




//-----------------------------------------------------------------------------
// Name: CacheWrite()
// Desc: Evaluate cache performance for various write situations
//-----------------------------------------------------------------------------
VOID BenchStorage::CacheWrite( DeviceType devType )
{
    CacheRandomWrite( devType );
    CacheSeqWrite( devType );
}




//-----------------------------------------------------------------------------
// Name: CacheRandomWrite()
// Desc: Evaluate cache performance for random writes
//-----------------------------------------------------------------------------
VOID BenchStorage::CacheRandomWrite( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strCACHE_RANDOM_WRITE_FILE, 
                  "CacheRandomWrite", L"Cache Random Write within a File",
                  "Write Time w/ FILE_FLAG_RANDOM_ACCESS (seconds)" ) )
        return;

    DWORD dwWrites = ( devType == Device_MU ) ? MAX_RANDOM_WRITES / 10 :
                                                MAX_RANDOM_WRITES;

    // Use default size first
    VERIFY( XSetFileCacheSize( m_dwDefaultCacheSize ) );
    FLOAT fElapsed = RandomWrite( fileTest, dwWrites, FILE_FLAG_RANDOM_ACCESS, 
                                  L"", m_dwDefaultCacheSize );
    fileResults.Write( m_dwDefaultCacheSize, fElapsed );

    // Try some different sizes    
    for( DWORD i = MIN_CACHE_SIZE; i <= MAX_CACHE_SIZE; i *= 2 )
    {
        VERIFY( XSetFileCacheSize( i ) );
        fElapsed = RandomWrite( fileTest, dwWrites, FILE_FLAG_RANDOM_ACCESS, 
                                L"", i );
        fileResults.Write( i, fElapsed );
    }

    // Restore default
    VERIFY( XSetFileCacheSize( m_dwDefaultCacheSize ) );
}




//-----------------------------------------------------------------------------
// Name: CacheSeqWrite()
// Desc: Evaluate cache performance for sequential writes
//-----------------------------------------------------------------------------
VOID BenchStorage::CacheSeqWrite( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strCACHE_SEQ_WRITE_FILE, 
                  "CacheSeqWrite", L"Cache Sequential Write within a File",
                  "Write Time w/ FILE_FLAG_SEQUENTIAL_SCAN (seconds)" ) )
        return;

    DWORD dwWrites = ( devType == Device_MU ) ? MAX_SEQUENTIAL_WRITES / 10 :
                                                MAX_SEQUENTIAL_WRITES;

    // Use default size first
    VERIFY( XSetFileCacheSize( m_dwDefaultCacheSize ) );
    FLOAT fElapsed = SeqWrite( fileTest, dwWrites, FILE_FLAG_SEQUENTIAL_SCAN, L"",
                               m_dwDefaultCacheSize );
    fileResults.Write( m_dwDefaultCacheSize, fElapsed );

    // Try some different sizes    
    for( DWORD i = MIN_CACHE_SIZE; i <= MAX_CACHE_SIZE; i *= 2 )
    {
        VERIFY( XSetFileCacheSize( i ) );
        fElapsed = SeqWrite( fileTest, dwWrites, FILE_FLAG_SEQUENTIAL_SCAN, 
                             L"", i );
        fileResults.Write( i, fElapsed );
    }

    // Restore default
    VERIFY( XSetFileCacheSize( m_dwDefaultCacheSize ) );
}




//-----------------------------------------------------------------------------
// Name: RegionRead()
// Desc: Evaluate the read speed of the different regions on the hard drive
//-----------------------------------------------------------------------------
VOID BenchStorage::RegionRead()
{
    RegionRead( Device_HD_User );
    RegionRead( Device_HD_Persist );
    RegionRead( Device_HD_Utility );
}




//-----------------------------------------------------------------------------
// Name: RegionRead()
// Desc: Evaluate the read speed of the given drive (region)
//-----------------------------------------------------------------------------
VOID BenchStorage::RegionRead( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strREGION_READ_FILE, 
                  "RegionRead", L"Region Read Speed",
                  "Buffer Size (bytes)", "Read Time (seconds)" ) )
        return;

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( m_dwHdBufferSize );

    FLOAT fTotal = 0.0f;
    for( DWORD i = 0; i < REGION_PASSES; ++i )
    {
        // Open the test file
        File fileRead;
        VERIFY( fileRead.Open( fileTest.GetFileName(), GENERIC_READ ) );

        // Read the entire file sequentially
        DWORD dwTotalRead = 0;
        for( ;; )
        {
            DWORD dwRead;
            FLOAT fElapsed;
            VERIFY( fileRead.Read( pBuffer, m_dwHdBufferSize, dwRead, 
                                   fElapsed ) );
            fTotal += fElapsed;
            assert( TestFile::IsValid( pBuffer, dwRead ) );
            dwTotalRead += dwRead;
            if( dwTotalRead >= TEST_FILE_SIZE )
                break;
        }
        ShowStatus( FLOAT(i) / FLOAT(REGION_PASSES) );
    }

    fileResults.Write( m_dwHdBufferSize, fTotal );
}




//-----------------------------------------------------------------------------
// Name: RegionWrite()
// Desc: Evaluate the write speed of the different regions on the hard drive
//-----------------------------------------------------------------------------
VOID BenchStorage::RegionWrite()
{
    RegionWrite( Device_HD_User );
    RegionWrite( Device_HD_Persist );
    RegionWrite( Device_HD_Utility );
}




//-----------------------------------------------------------------------------
// Name: RegionWrite()
// Desc: Evaluate the write speed of the given drive (region)
//-----------------------------------------------------------------------------
VOID BenchStorage::RegionWrite( DeviceType devType )
{
    TestFile fileTest( GetDrive( devType ) );
    ResultsFile fileResults;
    if( !Prepare( devType, fileTest, fileResults, strREGION_WRITE_FILE, 
                  "RegionWrite", L"Region Write Speed",
                  "Buffer Size (bytes)", "Write Time (seconds)" ) )
        return;

    // Create a large buffer
    AutoHeapPtr Buffer;
    BYTE* pBuffer = Buffer.Alloc( m_dwHdBufferSize );

    FLOAT fTotal = 0.0f;
    for( DWORD i = 0; i < REGION_PASSES; ++i )
    {
        // Open the test file
        File fileWrite;
        VERIFY( fileWrite.Open( fileTest.GetFileName(), GENERIC_WRITE ) );

        // Write the entire file sequentially
        DWORD dwTotalWritten = 0;
        for( ;; )
        {
            DWORD dwWritten;
            FLOAT fElapsed;
            VERIFY( fileWrite.Write( pBuffer, m_dwHdBufferSize, 
                                     dwWritten, fElapsed ) );
            fTotal += fElapsed;
            dwTotalWritten += dwWritten;
            if( dwTotalWritten >= TEST_FILE_SIZE )
                break;
        }
        ShowStatus( FLOAT(i) / FLOAT(REGION_PASSES) );
    }

    fileResults.Write( m_dwHdBufferSize, fTotal );
}




//-----------------------------------------------------------------------------
// Name: EnumSaves()
// Desc: Evaluate the speed of enumerating saved games
//-----------------------------------------------------------------------------
VOID BenchStorage::EnumSaves( DeviceType devType )
{
    assert( devType != Device_DVD );
    CHAR chDrive = GetDrive( devType );
    ResultsFile fileResults;
    VERIFY( fileResults.Create( strENUM_SAVES_FILE, chDrive, "Saved games", 
                                "Enum speed (seconds)" ) );

    lstrcpyW( m_strStatusHdr, L"Saved Game Enumeration Speed" );
    swprintf( m_strStatus, L"Drive %c", chDrive );
    ShowStatus( TRUE );

    // Generate some fake saved games
    SavedGames savedGames( m_dwSeed, ENUM_SAVE_GAMES, chDrive );
    VERIFY( savedGames.Create() );

    // Enumerate the saves
    CHAR strDrive[4] = "x:\\";
    strDrive[0] = chDrive;

    DWORD dwSaves = 0;
    XGAME_FIND_DATA SaveGameData;
    CXBStopWatch timer( TRUE );
    HANDLE hSaveGame = XFindFirstSaveGame( strDrive, &SaveGameData );
    assert( hSaveGame != INVALID_HANDLE_VALUE );
    if( hSaveGame != INVALID_HANDLE_VALUE )
    {
        do
        {
            ++dwSaves;
            // Would normally do something with SaveGameData here,
            // but we're just evaluating the overhead of enumeration
        } while( XFindNextSaveGame( hSaveGame, &SaveGameData ) );
        VERIFY( XFindClose( hSaveGame ) );
    }
    FLOAT fElapsed = timer.GetElapsedSeconds();
    fileResults.Write( ENUM_SAVE_GAMES, fElapsed );
    assert( dwSaves == ENUM_SAVE_GAMES );
}




//-----------------------------------------------------------------------------
// Name: EnumDirs()
// Desc: Evaluate the speed of enumerating directory trees
//-----------------------------------------------------------------------------
VOID BenchStorage::EnumDirs( DeviceType devType )
{
    assert( devType != Device_DVD );
    CHAR chDrive = GetDrive( devType );
    ResultsFile fileResults;
    VERIFY( fileResults.Create( strENUM_DIRS_FILE, chDrive, 
                                "Files/Directories", 
                                "Enum speed (seconds)" ) );

    lstrcpyW( m_strStatusHdr, L"Directory Enumeration Speed" );
    swprintf( m_strStatus, L"Drive %c", chDrive );
    ShowStatus( TRUE );

    // Generate a directory tree
    SavedGames dirTree( m_dwSeed, ENUM_DIR_TREE, chDrive );
    VERIFY( dirTree.Create() );

    // Enumerate the directories
    CHAR strDrive[4] = "x:\\";
    strDrive[0] = chDrive;

    CXBStopWatch timer( TRUE );
    DWORD dwCount = EnumFiles( strDrive );
    FLOAT fElapsed = timer.GetElapsedSeconds();

    // Record the results
    fileResults.Write( dwCount, fElapsed );
}




//-----------------------------------------------------------------------------
// Name: EnumFiles()
// Desc: Recursive enumeration function; returns number of files
//-----------------------------------------------------------------------------
DWORD BenchStorage::EnumFiles( const CHAR* strDir ) // static
{
    assert( strDir != NULL );

    // Append wildcard specifier to folder
    WIN32_FIND_DATA FileData;
    lstrcpyA( FileData.cFileName, strDir );
    lstrcatA( FileData.cFileName, "*" );

    // Count all files in strDir and all subdirectories
    DWORD dwCount = 0;

    HANDLE hFile = FindFirstFile( FileData.cFileName, &FileData );
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do
        {
            // Recurse into subdirectories
            if( FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
                // subdir = strDir + FileData.cFileName + '\'
                CHAR strSubDir[ MAX_PATH ];
                lstrcpyA( strSubDir, strDir );
                lstrcatA( strSubDir, FileData.cFileName );
                lstrcatA( strSubDir, "\\" );
                dwCount += EnumFiles( strSubDir );
            }
            else
            {
                // We have a file
                ++dwCount;
            }

        } while( FindNextFile( hFile, &FileData ) );
        VERIFY( FindClose( hFile ) );
    }
    return dwCount;
}




//-----------------------------------------------------------------------------
// Name: EnumSoundtracks()
// Desc: Evaluate the speed of enumerating soundtracks
//-----------------------------------------------------------------------------
VOID BenchStorage::EnumSoundtracks()
{
    CHAR chDrive = GetDrive( Device_HD );
    ResultsFile fileResults;
    VERIFY( fileResults.Create( strENUM_SOUNDTRACKS_FILE, chDrive, 
                                "Songs", 
                                "Enum speed (seconds)" ) );

    lstrcpyW( m_strStatusHdr, L"Soundtrack Enumeration Speed" );
    swprintf( m_strStatus, L"Drive %c", chDrive );
    ShowStatus( TRUE );
    DWORD dwCount = 0;

    // Enumerate the soundtracks
    XSOUNDTRACK_DATA SoundtrackData;
    CXBStopWatch timer( TRUE );
    HANDLE hSoundtrack = XFindFirstSoundtrack( &SoundtrackData );
    if( hSoundtrack != INVALID_HANDLE_VALUE )
    {
        do
        {
            for( DWORD i = 0; i < SoundtrackData.uSongCount; ++i, ++dwCount )
            {
                DWORD dwSongId;
                DWORD dwSongLength;
                WCHAR strSongName[ MAX_SOUNDTRACK_NAME ];
                VERIFY( XGetSoundtrackSongInfo( 
                                    SoundtrackData.uSoundtrackId, i, 
                                    &dwSongId, &dwSongLength,
                                    strSongName, MAX_SOUNDTRACK_NAME ) );
            }        
        } while( XFindNextSoundtrack( hSoundtrack, &SoundtrackData ) );
        VERIFY( XFindClose( hSoundtrack ) );
    }

    // Record the results
    FLOAT fElapsed = timer.GetElapsedSeconds();
    fileResults.Write( dwCount, fElapsed );
}




//-----------------------------------------------------------------------------
// Name: EnumNicknames()
// Desc: Evaluate the speed of enumerating nicknames
//-----------------------------------------------------------------------------
VOID BenchStorage::EnumNicknames()
{
    CHAR chDrive = GetDrive( Device_HD );
    ResultsFile fileResults;
    VERIFY( fileResults.Create( strENUM_NICKNAMES_FILE, chDrive, 
                                "Nicknames", 
                                "Enum speed (seconds)" ) );

    lstrcpyW( m_strStatusHdr, L"Nickname Enumeration Speed" );
    swprintf( m_strStatus, L"Drive %c", chDrive );
    ShowStatus( TRUE );

    WCHAR strNickname[ MAX_NICKNAME ];

    // Generate some fake nicknames
    for( DWORD i = 0; i < MAX_NICKNAMES; ++i )
    {
        WCHAR strNum[32];
        lstrcpyW( strNickname, strNICKNAME_PREFIX );
        lstrcatW( strNickname, _itow( i, strNum, 10 ) );
        VERIFY( XSetNickname( strNickname, FALSE ) );
    }

    // Enumerate the nicknames
    DWORD dwCount = 0;
    CXBStopWatch timer( TRUE );
    HANDLE hNickname = XFindFirstNickname( FALSE, strNickname, MAX_NICKNAME );
    if( hNickname != INVALID_HANDLE_VALUE )
    {
        do
        {
            ++dwCount;
        } while ( XFindNextNickname( hNickname, strNickname, MAX_NICKNAME ) );
        VERIFY( XFindClose( hNickname ) );
    }

    // Record the results
    FLOAT fElapsed = timer.GetElapsedSeconds();
    fileResults.Write( dwCount, fElapsed );
}




//-----------------------------------------------------------------------------
// Name: EnumOpen()
// Desc: Evaluate the speed of enumerating and opening files
//-----------------------------------------------------------------------------
VOID BenchStorage::EnumOpen( DeviceType devType )
{
    assert( devType != Device_DVD );
    CHAR chDrive = GetDrive( devType );
    ResultsFile fileResults;
    VERIFY( fileResults.Create( strENUM_OPEN_FILE, chDrive, 
                                "Files/Directories", 
                                "Enum/open speed (seconds)",
                                "Enum/open optimized speed (seconds)" ) );

    lstrcpyW( m_strStatusHdr, L"File Enum/Open Speed" );
    swprintf( m_strStatus, L"Drive %c", chDrive );
    ShowStatus( TRUE );

    // Generate a directory tree
    SavedGames dirTree( m_dwSeed, ENUM_OPEN_TREE, chDrive );
    VERIFY( dirTree.Create() );

    CHAR strDrive[4] = "x:\\";
    strDrive[0] = chDrive;

    // Get a list of all the files
    FileList fileList;
    FLOAT fElapsed = EnumFiles( strDrive, fileList );

    // Open each file
    CXBStopWatch FileOpenTimer( FALSE );
    for( FileList::const_iterator i = fileList.begin(); i != fileList.end(); ++i )
    {
        FileOpenTimer.Start();
        HANDLE hFile = CreateFile( *i, GENERIC_READ, FILE_SHARE_READ, NULL, 
                                   OPEN_ALWAYS, 0, NULL );
        FileOpenTimer.Stop();

        assert( hFile != INVALID_HANDLE_VALUE );
        BOOL bSuccess = CloseHandle( hFile );
        (VOID)bSuccess;
    }

    // Record the time to enum and open the files
    fElapsed += FileOpenTimer.GetElapsedSeconds();

    // Enumerate and open all the files in one pass
    OpenFileList openFileList;
    FLOAT fOptimized = EnumFiles( strDrive, openFileList );

    // Record the results
    fileResults.Write( DWORD( fileList.size() ), fElapsed, fOptimized );
}




//-----------------------------------------------------------------------------
// Name: EnumFiles()
// Desc: Recursive enumeration function; returns enumeration time and
//       list of files
//-----------------------------------------------------------------------------
FLOAT BenchStorage::EnumFiles( const CHAR* strDir, FileList& fileList ) // static
{
    FLOAT fElapsed = 0.0f;
    CXBStopWatch timer( FALSE );

    assert( strDir != NULL );

    timer.Start();

    // Append wildcard specifier to folder
    WIN32_FIND_DATA FileData;
    lstrcpyA( FileData.cFileName, strDir );
    lstrcatA( FileData.cFileName, "*" );

    // Enum all files in strDir and all subdirectories
    HANDLE hFile = FindFirstFile( FileData.cFileName, &FileData );
    timer.Stop();
    if( hFile != INVALID_HANDLE_VALUE )
    {
        BOOL bHaveFile = FALSE;
        do
        {
            // Ignore fileResults
            if( strstr( FileData.cFileName, strENUM_OPEN_FILE ) == NULL )
            {
                // name = strDir + FileData.cFileName
                timer.Start();
                CHAR strName[ MAX_PATH ];
                lstrcpyA( strName, strDir );
                lstrcatA( strName, FileData.cFileName );
                timer.Stop();

                // Recurse into subdirectories
                if( FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                    lstrcatA( strName, "\\" );
                    fElapsed += EnumFiles( strName, fileList );
                }
                else
                {
                    CHAR* pName = new CHAR [ lstrlenA( strName ) + 1 ];
                    lstrcpyA( pName, strName );
                    fileList.push_back( pName );
                }
            }

            timer.Start();
            bHaveFile = FindNextFile( hFile, &FileData );
            timer.Stop();

        } while( bHaveFile );

        timer.Start();
        VERIFY( FindClose( hFile ) );
        timer.Stop();
    }

    fElapsed += timer.GetElapsedSeconds();
    return fElapsed;
}




//-----------------------------------------------------------------------------
// Name: EnumFiles()
// Desc: Recursive enumeration function; returns enumeration time and
//       list of opened files
//-----------------------------------------------------------------------------
FLOAT BenchStorage::EnumFiles( const CHAR* strDir, OpenFileList& openFileList ) // static
{
    FLOAT fElapsed = 0.0f;
    CXBStopWatch timer( FALSE );

    assert( strDir != NULL );

    timer.Start();

    // Append wildcard specifier to folder
    WIN32_FIND_DATA FileData;
    lstrcpyA( FileData.cFileName, strDir );
    lstrcatA( FileData.cFileName, "*" );

    // Enum all files in strDir and all subdirectories
    HANDLE hFile = FindFirstFile( FileData.cFileName, &FileData );
    timer.Stop();
    if( hFile != INVALID_HANDLE_VALUE )
    {
        BOOL bHaveFile = FALSE;
        do
        {
            // Ignore fileResults
            if( strstr( FileData.cFileName, strENUM_OPEN_FILE ) == NULL )
            {
                // name = strDir + FileData.cFileName
                timer.Start();
                CHAR strName[ MAX_PATH ];
                lstrcpyA( strName, strDir );
                lstrcatA( strName, FileData.cFileName );
                timer.Stop();

                // Recurse into subdirectories
                if( FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                    lstrcatA( strName, "\\" );
                    fElapsed += EnumFiles( strName, openFileList );
                }
                else
                {
                    timer.Start();
                    HANDLE hFile = CreateFile( strName, GENERIC_READ, 
                                               FILE_SHARE_READ, NULL,
                                               OPEN_ALWAYS, 0, NULL );
                    timer.Stop();
                    assert( hFile != INVALID_HANDLE_VALUE );
                    openFileList.push_back( hFile );
                }
            }

            timer.Start();
            bHaveFile = FindNextFile( hFile, &FileData );
            timer.Stop();

        } while( bHaveFile );

        timer.Start();
        VERIFY( FindClose( hFile ) );
        timer.Stop();
    }

    fElapsed += timer.GetElapsedSeconds();
    return fElapsed;
}




//-----------------------------------------------------------------------------
// Name: Scatter()
// Desc: Evaluate the speed of 
//-----------------------------------------------------------------------------
VOID BenchStorage::Scatter( DeviceType devType )
{
    // !!!
    (VOID)devType;
}




//-----------------------------------------------------------------------------
// Name: Gather()
// Desc: Evaluate the speed of
//-----------------------------------------------------------------------------
VOID BenchStorage::Gather( DeviceType devType )
{
    // !!!
    (VOID)devType;
}




//-----------------------------------------------------------------------------
// Name: InterleavedRead()
// Desc: Evaluate the performance of interleaving reads from HD and DVD
//-----------------------------------------------------------------------------
VOID BenchStorage::InterleavedRead()
{
    // !!!
}




//-----------------------------------------------------------------------------
// Name: InterleavedWrite()
// Desc: Evaluate the performance of interleaving writes to HD and DVD
//-----------------------------------------------------------------------------
VOID BenchStorage::InterleavedWrite()
{
    // !!!
}




//-----------------------------------------------------------------------------
// Name: BytesTransferred()
// Desc: Standard Read/WriteFileEx callback function
//-----------------------------------------------------------------------------
VOID CALLBACK BenchStorage::BytesTransferred( DWORD dwErrorCode,
                                              DWORD dwBytesTransferred,
                                              OVERLAPPED* pOverlapped ) // static
{
    assert( dwErrorCode == 0 );
    assert( pOverlapped != NULL );
    (VOID)dwErrorCode;

    // Extract the "this" pointer that we tucked away in hEvent
    BenchStorage* pThis = (BenchStorage*)( pOverlapped->hEvent );
    assert( pThis != NULL );
    pThis->OnBytesTransferred( dwBytesTransferred );
}




//-----------------------------------------------------------------------------
// Name: OnBytesTransferred()
// Desc: Custom callback for this class
//-----------------------------------------------------------------------------
VOID BenchStorage::OnBytesTransferred( DWORD dwBytesTransferred )
{
    // Store the number of bytes read/written and trigger the event
    m_dwBytesTransferred = dwBytesTransferred;
}




//-----------------------------------------------------------------------------
// Name: IsValidDevice()
// Desc: TRUE if the given device is available for benchmarking
//-----------------------------------------------------------------------------
BOOL BenchStorage::IsValidDevice( DeviceType devType ) const
{
    switch( devType )
    {
        case Device_MU:          return TEST_MU  && m_chMemUnit   != '\0';
        case Device_DVD:         return TEST_DVD && m_chDvd       != '\0';
        case Device_HD:          return TEST_HD  && m_chHardDrive != '\0';
        case Device_HD_User:     return TEST_HD;
        case Device_HD_Persist:  return TEST_HD;
        case Device_HD_Utility:  return TEST_HD;
        default:assert( FALSE ); return FALSE;
    }
}




//-----------------------------------------------------------------------------
// Name: GetDrive()
// Desc: Get the logical drive matching the device
//-----------------------------------------------------------------------------
CHAR BenchStorage::GetDrive( DeviceType devType ) const
{
    switch( devType )
    {
        case Device_MU:          return m_chMemUnit;
        case Device_DVD:         return m_chDvd;
        case Device_HD:          return m_chHardDrive;
        case Device_HD_User:     return 'U';
        case Device_HD_Persist:  return 'T';
        case Device_HD_Utility:  return 'Z';
        default:assert( FALSE ); return '\0';
    }
}




//-----------------------------------------------------------------------------
// Name: ShowStatus()
// Desc: Show the current benchmarking status percent
//-----------------------------------------------------------------------------
VOID BenchStorage::ShowStatus( FLOAT fPercent )
{
    swprintf( m_strStatus, L"Progress: %.0f %%", 100.0f * fPercent );
    ShowStatus();
}




//-----------------------------------------------------------------------------
// Name: ShowStatus()
// Desc: Show the current benchmarking status percent
//-----------------------------------------------------------------------------
VOID BenchStorage::ShowStatus( DWORD dwBufferSize, FLOAT fPercent )
{
    swprintf( m_strStatus, L"Buffer size: %lu\nProgress: %.0f %%",
              dwBufferSize, 100.0f * fPercent );
    ShowStatus();
}




//-----------------------------------------------------------------------------
// Name: ShowStatus()
// Desc: Show the current benchmarking status
//-----------------------------------------------------------------------------
VOID BenchStorage::ShowStatus( BOOL bForceRefresh )
{
    if( !bForceRefresh )
    {
        // Only update status screen ten times per second, because this
        // function has considerable overhead
        if( m_StatusTimer.GetElapsedSeconds() < 0.1f )
            return;
    }

    m_StatusTimer.StartZero();
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x000A0A6A, 1.0f, 0L );

    m_Font.DrawText( 320, 140, 0xFFFFFFFF, m_strStatusHdr, XBFONT_CENTER_X );
    m_Font.DrawText( 320, 240, 0xFFFFFFFF, m_strStatus, XBFONT_CENTER_X );

    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: Prepare()
// Desc: Prepare for profiling
//-----------------------------------------------------------------------------
BOOL BenchStorage::Prepare( DeviceType devType, TestFile& fileTest, 
                            ResultsFile& fileResults, const CHAR* strResultsFile,
                            const CHAR* strFuncName, const WCHAR* strDesc, 
                            const CHAR* strResultsHdr1, const CHAR* strResultsHdr2,
                            const CHAR* strResultsHdr3, const CHAR* strResultsHdr4 )
{
    CHAR chDrive = GetDrive( devType );
    if( !IsValidDevice( devType ) )
        return FALSE;

    lstrcpyW( m_strStatusHdr, L"Preparing next benchmark" );
    swprintf( m_strStatus, L"Writing %lu MB test file to %c", 
              TEST_FILE_SIZE / 1024 / 1024, GetDrive( devType ) );
    ShowStatus( TRUE );

    // Create a large file for reading
    if( !fileTest.Create() )
    {
        CHAR strDebug[1024];
        wsprintfA( strDebug, "Test file create failure in %s Drive %c\n",
                   strFuncName, chDrive );
        OUTPUT_DEBUG_STRING( strDebug );
        return FALSE;
    }

    // Create results file
    if( !fileResults.Create( strResultsFile, chDrive, strResultsHdr1, 
                             strResultsHdr2, strResultsHdr3, strResultsHdr4 ) )
    {
        CHAR strDebug[1024];
        wsprintfA( strDebug, "Results file create failure in %s Drive %c\n",
                   strFuncName, chDrive );
        return FALSE;
    }

    // Flush the file cache
    VERIFY( XSetFileCacheSize( 0 ) );
    VERIFY( XSetFileCacheSize( m_dwDefaultCacheSize ) );

    wsprintfW( m_strStatusHdr, L"%s, Drive %c", strDesc, chDrive );
    return TRUE;
}
