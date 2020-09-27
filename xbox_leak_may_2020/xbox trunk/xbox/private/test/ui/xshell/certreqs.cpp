/*****************************************************
*** certreqs.cpp
***
*** CPP file for our Certification Requirements 
*** class.  This class will configure the Xbox in
*** various ways to satisfy certain certification
*** requirements
***
*** by James N. Helm
*** May 26th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "certreqs.h"

extern CHardDrive   g_XboxHardDrive;    // Used to configure the Xbox Hard Drive
extern DWORD        g_dwInsertedMUs;    // Tracks which MUs are inserted or removed
extern CMemoryUnit  g_MemoryUnits[8];   // Memory Units that can be inserted, or removed

// Constructors
CCertReqs::CCertReqs()
: m_bUtilityMounted( FALSE )
{
}

// Destructor
CCertReqs::~CCertReqs()
{
}

// Configures the Xbox for the cert requirment
// Removes all System Files: Soundtracks and Nicknames
HRESULT CCertReqs::Req10_1_11()
{
    HRESULT hr = S_OK;

    // Remove Nicknames
    if( FAILED( g_XboxHardDrive.RemoveAllNicknames() ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_1_11():Failed to delete the Nicknames!!" );

        hr = E_FAIL;
    }

    // Remove Soundtracks
    if( FAILED( g_XboxHardDrive.RemoveAllSoundtracks() ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_1_11():Failed to delete the Soundtracks!!" );

        hr = E_FAIL;
    }

    return hr;
}


// Configures the Xbox for the cert requirment
// List all global settings
HRESULT CCertReqs::Req10_1_12()
{
    return S_OK;
}


// Configures the Xbox for the cert requirment
// Fragments tdata and udata
// #define _PERF
#define PERF_READFILE_NUM_TIMES     100
#define PERF_NUM_FILES_TO_USE       10
#define PERF_FILE_SIZE              5120000 // 10240000
#define PERF_BUFFER_SIZE            PERF_FILE_SIZE // 4096
#define PERF_FILE_NUM_READ_LOOPS    ( PERF_FILE_SIZE / PERF_BUFFER_SIZE )
HRESULT CCertReqs::Req10_2_01()
{
#ifdef _PERF
    // Clear out the partition
    g_XboxHardDrive.UnMapDrive( FILE_DATA_UDATA_DRIVE_LETTER_A );
    g_XboxHardDrive.DeleteDirectory( "c:\\udata" );
    CreateDirectory( "c:\\udata", NULL );
    g_XboxHardDrive.MapDrive( FILE_DATA_UDATA_DRIVE_LETTER_A, FILE_DATA_UDATA_PATH_A );

    // Write files
    char pszFileName[MAX_PATH];
    ZeroMemory( pszFileName, MAX_PATH );
    LARGE_INTEGER liFileSize;
    liFileSize.QuadPart = PERF_FILE_SIZE;
    for( unsigned int x = 0; x < PERF_NUM_FILES_TO_USE; x++ )
    {
        sprintf( pszFileName, "c:\\udata\\file_%d", x );
        g_XboxHardDrive.CreateFileOfSize( pszFileName, liFileSize );
    }

    // Read the files and time it
    DWORD dwTimeBeforeFrag = GetTickCount();

    BYTE* buffer = new BYTE[PERF_BUFFER_SIZE];
    if( NULL == buffer )
    {
        XDBGWRN( APP_TITLE_NAME_A, "Failed to allocate buffer memory!!" );

        return E_FAIL;
    }

    for( unsigned int xx = 0; xx < PERF_READFILE_NUM_TIMES; xx++ )
    {
        //XDBGWRN( APP_TITLE_NAME_A, "xx - '%d'", xx );
        for( unsigned int x = 0; x < PERF_NUM_FILES_TO_USE; x++ )
        {
            //XDBGWRN( APP_TITLE_NAME_A, "x - '%d'", xx );
            sprintf( pszFileName, "c:\\udata\\file_%d", x );

            // Dump the file info to our destination file
            HANDLE hInFile = CreateFile( pszFileName,
                                         GENERIC_READ,
                                         0,
                                         NULL,
                                         OPEN_EXISTING,
                                         FILE_ATTRIBUTE_NORMAL,
                                         NULL );

            if( INVALID_HANDLE_VALUE == hInFile )
            {
                XDBGWRN( APP_TITLE_NAME_A, "Failed to read the file - '%s'", pszFileName );

                continue;
            }

            DWORD dwNumRead;
            for( unsigned int y = 0; y < PERF_FILE_NUM_READ_LOOPS; y++ )
            {
                if( !ReadFile( hInFile,
                               buffer,
                               PERF_BUFFER_SIZE,
                               &dwNumRead,
                               NULL ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "Failed to read the file block from file!! - '%s'", pszFileName );
                }
            }

            CloseHandle( hInFile );
            hInFile = NULL;
        }
    }

    dwTimeBeforeFrag = GetTickCount() - dwTimeBeforeFrag;

    // Fragment the partition
    g_XboxHardDrive.CreateFragTempFiles();
    g_XboxHardDrive.FragmentTree( "c:\\udata" );
    g_XboxHardDrive.DeleteDirectory( MEMORY_AREA_FRAG_FILE_DIR );

    // Read the files and time it
    DWORD dwTimeAfterFrag = GetTickCount();

    for( unsigned int xx = 0; xx < PERF_READFILE_NUM_TIMES; xx++ )
    {
        for( unsigned int x = 0; x < PERF_NUM_FILES_TO_USE; x++ )
        {
            sprintf( pszFileName, "c:\\udata\\file_%d", x );

            // Dump the file info to our destination file
            HANDLE hInFile = CreateFile( pszFileName,
                                         GENERIC_READ,
                                         0,
                                         NULL,
                                         OPEN_EXISTING,
                                         FILE_ATTRIBUTE_NORMAL,
                                         NULL );

            if( INVALID_HANDLE_VALUE == hInFile )
            {
                XDBGWRN( APP_TITLE_NAME_A, "Failed to read the file - '%s'", pszFileName );

                continue;
            }

            DWORD dwNumRead;
            for( unsigned int y = 0; y < PERF_FILE_NUM_READ_LOOPS; y++ )
            {
                if( !ReadFile( hInFile,
                               buffer,
                               PERF_BUFFER_SIZE,
                               &dwNumRead,
                               NULL ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "Failed to read the file block from file!! - '%s'", pszFileName );
                }
            }

            CloseHandle( hInFile );
            hInFile = NULL;
        }
    }

    dwTimeAfterFrag = GetTickCount() - dwTimeAfterFrag;

    XDBGWRN( APP_TITLE_NAME_A, "Time First  - '%d'", dwTimeBeforeFrag );
    XDBGWRN( APP_TITLE_NAME_A, "Time Second - '%d'", dwTimeAfterFrag );

    // Free our memory
    if( buffer )
    {
        delete[] buffer;
        buffer = NULL;
    }

    g_XboxHardDrive.UnMapDrive( FILE_DATA_UDATA_DRIVE_LETTER_A );
    g_XboxHardDrive.DeleteDirectory( "c:\\udata" );
    CreateDirectory( "c:\\udata", NULL );
    g_XboxHardDrive.MapDrive( FILE_DATA_UDATA_DRIVE_LETTER_A, FILE_DATA_UDATA_PATH_A );
#else // _PERF
    // Mount our utility drive if it hasn't been mounted already
    if( !m_bUtilityMounted )
    {
        m_bUtilityMounted = XMountUtilityDrive( TRUE );
    }

    if( !m_bUtilityMounted )
    {
        XDBGWRN( APP_TITLE_NAME_A, "Req10_2_01():Failed to mount the utility drive!! Error - '0x%0.8X (%d)'", GetLastError(), GetLastError() );

        return E_FAIL;
    }

    // Create our temporary files used in fragmentation
    g_XboxHardDrive.CreateFragTempFiles();

    // Process the TDATA directory
    g_XboxHardDrive.FragmentTree( "c:\\tdata" );

    // Process the UDATA directory
    g_XboxHardDrive.FragmentTree( "c:\\udata" );

    // Clean up our directories
    g_XboxHardDrive.DeleteDirectory( MEMORY_AREA_FRAG_FILE_DIR );
#endif // _PERF

    return S_OK;
}


// Configures the Xbox for the cert requirment
// Removes all Soundtracks
HRESULT CCertReqs::Req10_2_02()
{
    return g_XboxHardDrive.RemoveAllSoundtracks();
}


// Configures the Xbox for the cert requirment
// Fills udata\<titleid> with 4096 entries
HRESULT CCertReqs::Req10_2_07( char pszTitleID[9] )
{
    if( !pszTitleID )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_2_07():Invalid paramter passed in!!" );
        
        return E_INVALIDARG;
    }

    char pszFullPath[MAX_PATH];
    ZeroMemory( pszFullPath, MAX_PATH );

    _snprintf( pszFullPath, MAX_PATH - 1, "%c:\\%s", FILE_DATA_UDATA_DRIVE_LETTER_A, pszTitleID );

    // Create the TITLE ID Directory if it doesn't exist
    CreateDirectory( pszFullPath, NULL );

    // Fill the TITLE ID Directory with 4096 entries
    for( unsigned int x = 0; ( x < 4096 ) && ( g_XboxHardDrive.GetAbortProcess() != TRUE ); x++ )
    {
        _snprintf( pszFullPath, MAX_PATH - 1, "%c:\\%s\\%s%d.xdk", FILE_DATA_UDATA_DRIVE_LETTER_A, pszTitleID, FILE_DATA_DIR_ENTRY_PATTERN, x );

        DWORD dwReturn = g_XboxHardDrive.CreateZeroByteFile( pszFullPath );

        if( ERROR_DISK_FULL == dwReturn )
        {
            break;
        }
    }

    return S_OK;
}


// Configures the Xbox for the cert requirment
// Clears Utility partition for <titleid>
HRESULT CCertReqs::Req10_2_10( DWORD dwTitleID )
{
    XapiDeleteCachePartition( dwTitleID );

    return S_OK;
}


// Configures the Xbox for the cert requirment
// Draws outline of non-HDTV safe area
HRESULT CCertReqs::Req10_2_18()
{
    return S_OK;
}


// Configures the Xbox for the cert requirment
// Draws outline of HDTV safe area
HRESULT CCertReqs::Req10_2_19()
{
    return S_OK;
}

/*
// Configures the Xbox for the cert requirment
// Fills udata area, but with less than 4096 entries
HRESULT CCertReqs::Req10_3_02()
{
    DWORD dwNumBlocksFree = g_XboxHardDrive.GetFreeBlocks();

    if( 0 == dwNumBlocksFree )
    {
        // Bail early, since we have no work to do

        return S_OK;
    }

    // Setup the proper path
    char pszFullPath[4];
    ZeroMemory( pszFullPath, 4 );

    sprintf( pszFullPath, "%c:\\", FILE_DATA_UDATA_DRIVE_LETTER_A );

    // Fill the UDATA area
    g_XboxHardDrive.FillArea( pszFullPath, dwNumBlocksFree );

    return S_OK;
}


// Configures the Xbox for the cert requirment
// Fills tdata area, but with less than 4096 entries
HRESULT CCertReqs::Req10_3_03()
{
    DWORD dwNumBlocksFree = g_XboxHardDrive.GetFreeBlocks();

    if( 0 == dwNumBlocksFree )
    {
        // Bail early, since we have no work to do

        return S_OK;
    }

    // Setup the proper path
    char pszFullPath[4];
    ZeroMemory( pszFullPath, 4 );

    sprintf( pszFullPath, "%c:\\", FILE_DATA_TDATA_DRIVE_LETTER_A );

    // Fill the TDATA area
    g_XboxHardDrive.FillArea( pszFullPath, dwNumBlocksFree );

    return S_OK;
}
*/

// Configures the Xbox for the cert requirment
// Fills tdata and udata area
HRESULT CCertReqs::Req10_3_04( DWORD dwFileSize )
{
    if( 0 == dwFileSize )
    {
        return S_OK;
    }

    DWORD dwNumBlocksFree = g_XboxHardDrive.GetFreeBlocks();

    XDBGTRC( APP_TITLE_NAME_A, "CCertReqs::Req10_3_04():Free - '%d', File - '%d'", dwNumBlocksFree, dwFileSize );

    if( 0 == dwNumBlocksFree )
    {
        // Bail early, since we have no work to do

        return S_OK;
    }

    if( dwFileSize > dwNumBlocksFree ) 
    {
        dwFileSize = dwNumBlocksFree;
    }

    // Setup the proper path
    unsigned int uiStrLen = strlen(FILE_EXECUTABLE_DIRECTORY_A)+1;
    char* pszFullPath = new char[uiStrLen+1];
    if( NULL == pszFullPath )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_04():Could not allocate memory!!" );

        return E_OUTOFMEMORY;
    }
    else
    {
        ZeroMemory( pszFullPath, uiStrLen + 1 );

        _snprintf( pszFullPath, uiStrLen, "%hs\\", FILE_EXECUTABLE_DIRECTORY_A );

        // Fill the UDATA are with the correct file
        g_XboxHardDrive.FillArea( pszFullPath, dwFileSize );

        delete[] pszFullPath;
        pszFullPath = NULL;
    }

    return S_OK;
}


// Configures the Xbox for the cert requirment
// Lists available blocks for all attached memory devices
HRESULT CCertReqs::Req10_3_11()
{
    return S_OK;
}


// Configures the Xbox for the cert requirment
// Change Language Setting
HRESULT CCertReqs::Req10_3_14()
{
    return S_OK;
}


// Configures the Xbox for the cert requirment
// Change Time Zone Setting
HRESULT CCertReqs::Req10_3_15()
{
    return S_OK;
}

// Configures the Xbox for the cert requirment
// Create Soundtrack names with localized text
HRESULT CCertReqs::Req10_3_26_1()
{
    //
    // Create Soundtrack Names
    //

    // Create soundtracks with localized Text
    if( !DashBeginUsingSoundtracks() )
    //
    // We failed to open the ST Database, so we can't create the soundtrack
    //
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_26_1():Failed to Begin Using Soundtracks!!" );

        return E_FAIL;
    }
    else
    //
    // We opened the ST Database, so we can create the soundtrack
    //
    {
        DWORD dwCurrentST = 0;

        if( !DashAddSoundtrack( STRING_DATA_LOC_CHARS, &dwCurrentST ) )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_26_1():Failed to add the Soundtrack!!" );
        }

        DashEndUsingSoundtracks();
    }

    return S_OK;
}


// Configures the Xbox for the cert requirment
// Create Nicknames with localized text
HRESULT CCertReqs::Req10_3_26_2()
{
    //
    // Create Nicknames
    //

    // Create Nicknames with localized text
    if( !XSetNickname( STRING_DATA_LOC_CHARS, TRUE ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_26_4():Failed to Set the Nickname!!" );

        return E_FAIL;
    }

    return S_OK;
}

// Configures the Xbox for the cert requirment
// Creates MU names with localized text
HRESULT CCertReqs::Req10_3_26_3()
{
    //
    // Create MU Names
    //

    // Enumerate Any Inserted MUs
    for( unsigned int x = 0; x < NUM_XBOX_MU_SLOTS; ++x )
    {
        if( g_MemoryUnits[x].IsMounted() )
        {
            // Mount any MU that are inserted, name them, and then unmount them
            g_MemoryUnits[x].Name( STRING_DATA_LOC_CHARS );
        }
    }

    return S_OK;
}


// Configures the Xbox for the cert requirment
// Creates Saved Game names with localized text
HRESULT CCertReqs::Req10_3_26_4( char pszTitleID[9] )
{
    if( !pszTitleID )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_26_4():Invalid paramter passed in!!" );
        
        return E_INVALIDARG;
    }

    //
    // Create Saved Games
    //
    
    // Create Directory for Specified Title
    char* pszDirName = new char[MAX_PATH+1];
    if( NULL == pszDirName )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_26_4():Failed to allocate memory!!" );
    }
    else
    {
        ZeroMemory( pszDirName, MAX_PATH + 1 );
        
        _snprintf( pszDirName, MAX_PATH, "%hc:\\%hs", FILE_DATA_UDATA_DRIVE_LETTER_A, pszTitleID );
        CreateDirectory( pszDirName, NULL );

        delete[] pszDirName;
        pszDirName = NULL;
    }    
    
    // Map Directory for Specified Title
    g_XboxHardDrive.MapUDataToTitleID( pszTitleID );

    // Create Saved Games with Localized Text
    char* pszSaveGamePath = new char[MAX_PATH+1];
    if( NULL == pszSaveGamePath )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_26_4():Failed to allocate memory!!" );
    }
    else
    {
        ZeroMemory( pszSaveGamePath, MAX_PATH+1 );
        DWORD dwReturn = XCreateSaveGame( "U:\\",
                                          STRING_DATA_LOC_CHARS,
                                          OPEN_ALWAYS,
                                          0,
                                          pszSaveGamePath,
                                          MAX_PATH );

        if( ERROR_SUCCESS != dwReturn )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_26_4():Failed to create the Saved Game!! Error - '0x%.8X (%d)'", dwReturn, dwReturn );
        }

        delete[] pszSaveGamePath;
        pszSaveGamePath = NULL;
    }

    // Clean up
    g_XboxHardDrive.MapUDataToOrigLoc();

    return S_OK;
}


// Configures the Xbox for the cert requirment
// Makes list of all files on console, both hard disk and MUs
HRESULT CCertReqs::Req10_3_33()
{
    // Delete the log file
    if( !DeleteFile( FILE_DATA_FILELIST_FILENAME ) )
    {
        if( ERROR_FILE_NOT_FOUND != GetLastError() )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_33():Failed to delete the file!! - '%s', Error - '0x%.8X (%d)'", FILE_DATA_FILELIST_FILENAME, GetLastError(), GetLastError() );
        }
    }

    char pszDriveRootPath[3];
    ZeroMemory( pszDriveRootPath, 3 );

    sprintf( pszDriveRootPath, "%c:", FILE_DATA_DUMPFILE_LINK_DRIVE );

    // List all the files on each partition
    char pszFullPath[MAX_PATH];

    //
    // Process the two main partitions on the Hard Drive
    //
    for( unsigned int x = 1; x < 3; x++ )
    {
        ZeroMemory( pszFullPath, MAX_PATH );
        _snprintf( pszFullPath, MAX_PATH - 1, "%s%d", FILE_DATA_PARTITION_PATTERN, x );

        DWORD dwReturn = g_XboxHardDrive.MapDrive( FILE_DATA_DUMPFILE_LINK_DRIVE, pszFullPath );
        if( 0 != dwReturn )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_33():Failed to map drive!! - '%c', '%s', Error - '%d'", FILE_DATA_DUMPFILE_LINK_DRIVE, pszFullPath, dwReturn );
        }

        DumpFiles( pszDriveRootPath, FILE_DATA_FILELIST_FILENAME, x );

        dwReturn = g_XboxHardDrive.UnMapDrive( FILE_DATA_DUMPFILE_LINK_DRIVE );
        if( 0 != dwReturn )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_33():Failed to un-map drive!! - '%c', Error - '%d'", FILE_DATA_DUMPFILE_LINK_DRIVE, dwReturn );
        }
    }

    //
    // Process the cache partitions
    //
    for( unsigned int x = 0; x < *HalDiskCachePartitionCount; x++ )
    {
        ZeroMemory( pszFullPath, MAX_PATH );
        _snprintf( pszFullPath, MAX_PATH - 1, "%s%d", FILE_DATA_PARTITION_PATTERN, x + 3 );

        DWORD dwReturn = g_XboxHardDrive.MapDrive( FILE_DATA_DUMPFILE_LINK_DRIVE, pszFullPath );
        if( 0 != dwReturn )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_33():Failed to map drive!! - '%c', '%s', Error - '%d'", FILE_DATA_DUMPFILE_LINK_DRIVE, pszFullPath, dwReturn );
        }

        DumpFiles( pszDriveRootPath, FILE_DATA_FILELIST_FILENAME, x + 3 );

        dwReturn = g_XboxHardDrive.UnMapDrive( FILE_DATA_DUMPFILE_LINK_DRIVE );
        if( 0 != dwReturn )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_3_33():Failed to un-map drive!! - '%c', Error - '%d'", FILE_DATA_DUMPFILE_LINK_DRIVE, dwReturn );
        }
    }

    //
    // Process the MUs
    //

    // Dump the files that live on the MUs
    for( unsigned int x = 0; x < NUM_XBOX_MU_SLOTS; x++ )
    {
        if( g_MemoryUnits[x].IsMounted() )
        {
            ZeroMemory( pszFullPath, MAX_PATH);
            _snprintf( pszFullPath, MAX_PATH - 1, "%c:", g_MemoryUnits[x].GetUDataDrive() );

            DumpFiles( pszFullPath, FILE_DATA_FILELIST_FILENAME, x + 6 );
        }
    }

    return S_OK;
}

// Corrupt Saved Games for a specific Directory
HRESULT CorruptSavedGame( char* pszDirectory )
{
    char pszFindPattern[MAX_PATH+1];
    pszFindPattern[MAX_PATH] = '\0';

    _snprintf( pszFindPattern, MAX_PATH, "%hs*", pszDirectory );
    XDBGTRC( APP_TITLE_NAME_A, "CCertReqs::CorruptSavedGame():pszFindPattern - '%hs'", pszFindPattern );

    // Find each file in the directory, and corrupt it
    WIN32_FIND_DATA findData;
    ZeroMemory( &findData, sizeof( findData ) );

    HANDLE hFile = FindFirstFile( pszFindPattern, &findData );
    if( INVALID_HANDLE_VALUE != hFile )
    {
        do
        {
            // Check to see if it's a directory
            if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
                if( '.' == findData.cFileName[0] )
                {
                    continue;
                }

                char pszDir[MAX_PATH+1];
                pszDir[MAX_PATH] = '\0';

                _snprintf( pszDir, MAX_PATH, "%hs%hs\\", pszDirectory, findData.cFileName );
                
                XDBGTRC( APP_TITLE_NAME_A, "CCertReqs::CorruptSavedGame():Corrupting Directory pszDir - '%hs'", pszDir );

                CorruptSavedGame( pszDir );
            }
            else // Must be a file
            {
                // Check to see if it's a meta file.  If it is, let's skip it
                if( 0 == _stricmp( "savemeta.xbx", findData.cFileName ) ||
                    0 == _stricmp( "saveimage.xbx", findData.cFileName ) )
                {
                    continue;
                }

                // Must not be a meta file, let's corrupt it
                char pszFilePath[MAX_PATH+1];
                pszFilePath[MAX_PATH] = '\0';

                _snprintf( pszFilePath, MAX_PATH, "%hs%hs", pszDirectory, findData.cFileName );

                XDBGTRC( APP_TITLE_NAME_A, "CCertReqs::CorruptSavedGame():Corrupting File pszFilePath - '%hs'", pszFilePath );

                HANDLE hCorruptFile = CreateFile( pszFilePath,
                                                  GENERIC_WRITE | GENERIC_READ,
                                                  0, // FILE_SHARE_WRITE | FILE_SHARE_READ,
                                                  NULL,
                                                  OPEN_EXISTING,
                                                  FILE_ATTRIBUTE_NORMAL,
                                                  NULL );
                if( INVALID_HANDLE_VALUE != hCorruptFile )
                {
                    DWORD dwFileSize = GetFileSize( hCorruptFile, NULL );
                    SetFilePointer( hCorruptFile, ( dwFileSize / 2 ), NULL, FILE_BEGIN );

                    // Read the current byte of the file
                    BYTE bBuffer[1];
                    DWORD dwNumBytes;
                    if( ReadFile( hCorruptFile, bBuffer, 1, &dwNumBytes, NULL ) )
                    {
                        SetFilePointer( hCorruptFile, ( dwFileSize / 2 ), NULL, FILE_BEGIN );

                        bBuffer[0] ^= 0xFF;
                        if( !WriteFile( hCorruptFile, bBuffer, 1, &dwNumBytes, NULL ) )
                        {
                            XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CorruptSavedGame():Failed to write the byte back to the file!! - '%hs', Error - '0x%0.8X (%d)'", pszFilePath, GetLastError(), GetLastError() );
                        }
                    }
                    else
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CorruptSavedGame():Failed to read a byte from the file!! - '%hs', Error - '0x%0.8X (%d)'", pszFilePath, GetLastError(), GetLastError() );
                    }

                    CloseHandle( hCorruptFile );
                    hCorruptFile = INVALID_HANDLE_VALUE;
                }
            }
        } while( FindNextFile( hFile, &findData ) );

        FindClose( hFile );
        hFile = INVALID_HANDLE_VALUE;
    }
    else
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CorruptSavedGame():Failed to find any files for - '%hs', Error - '0x%0.8X (%d)'", pszFindPattern, GetLastError(), GetLastError() );
    }

    return S_OK;    
}

// Corrupts Saved Games for a specific Title
HRESULT CCertReqs::Req10_6_05( char pszTitleID[9] )
{
    if( !pszTitleID )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_6_05():Invalid paramter passed in!!" );
        
        return E_INVALIDARG;
    }

    // Map the UData Drive to the Game Title
    if( FAILED( g_XboxHardDrive.MapUDataToTitleID( pszTitleID ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_6_05():Problem mapping UData for - '%hs'", pszTitleID );
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_6_05():Game Title does not exist!!" );

        g_XboxHardDrive.MapUDataToOrigLoc();

        return S_OK;
    }

    // Find each Saved Game for a specific Title, and Corrupt it
    XGAME_FIND_DATA gameFindData;
    ZeroMemory( &gameFindData, sizeof( gameFindData ) );

    HANDLE hSavedGame = XFindFirstSaveGame( "u:\\", &gameFindData );
    if( INVALID_HANDLE_VALUE != hSavedGame )
    {
        do
        {
            XDBGTRC( APP_TITLE_NAME_A, "CCertReqs::Req10_6_05():Corrupting Saved Game Named - '%ls'", gameFindData.szSaveGameName );
            CorruptSavedGame( gameFindData.szSaveGameDirectory );

        } while( XFindNextSaveGame( hSavedGame, &gameFindData ) );

        XFindClose( hSavedGame );
        hSavedGame = INVALID_HANDLE_VALUE;
    }
    else // Did not find a Saved Game
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::Req10_6_05():Did not find a Saved Game for Title - '%hs', Error - '0x%0.8X (%d)'", pszTitleID, GetLastError(), GetLastError() );
    }

    g_XboxHardDrive.MapUDataToOrigLoc();

    return S_OK;
}


// Will dump out file information to the specified Destination File
void CCertReqs::DumpFiles( char* pszDirName, char* pszDestFile, unsigned int uiPartitionNumber )
{
    if( NULL == pszDirName || NULL == pszDestFile )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::DumpFiles():Invalid parameter passed in!!" );

        return;
    }

    WIN32_FIND_DATA findData;
    char pszDirPattern[MAX_PATH];

    ZeroMemory( &findData, sizeof( findData ) );
    ZeroMemory( pszDirPattern, MAX_PATH );

    _snprintf( pszDirPattern, MAX_PATH - 1, "%s\\*", pszDirName );

    HANDLE hFile = FindFirstFile( pszDirPattern, &findData );
    if( INVALID_HANDLE_VALUE == hFile )
    {
        if( ERROR_FILE_NOT_FOUND != GetLastError() )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::DumpFiles():FindFirstFile Failed!! - '%s', '0x%.8X (%d)'", pszDirPattern, GetLastError(), GetLastError() );
        }

        return;
    }

    do
    {
        // Skip the directories
        if( FILE_ATTRIBUTE_DIRECTORY & findData.dwFileAttributes )
        {
            char pszSubDirName[MAX_PATH];
            ZeroMemory( pszSubDirName, MAX_PATH );

            _snprintf( pszSubDirName, MAX_PATH - 1, "%s\\%s", pszDirName, findData.cFileName );
            DumpFiles( pszSubDirName, pszDestFile, uiPartitionNumber );

            continue;
        }

        // Dump the file info to our destination file
        HANDLE hOutputFile = CreateFile( pszDestFile,
                                         GENERIC_WRITE,
                                         0,
                                         NULL,
                                         OPEN_ALWAYS,
                                         FILE_ATTRIBUTE_NORMAL,
                                         NULL );

        if( INVALID_HANDLE_VALUE == hOutputFile )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::DumpFiles():Failed to open our log file!! - '%s', Error - '0x%.8X (%d)'", pszDestFile, GetLastError(), GetLastError() );

            continue;
        }
        else
        {
            char pszBuffer[MAX_PATH];
            SYSTEMTIME sysTime;
            DWORD dwBytesWritten = 0;

            ZeroMemory( pszBuffer, MAX_PATH );
            ZeroMemory( &sysTime, sizeof( sysTime ) );
            
            FileTimeToSystemTime( &findData.ftLastWriteTime, &sysTime );

            _snprintf( pszBuffer, MAX_PATH - 1, "Partition: %2d   %02d/%02d/%d   %02d:%02d/%02d   %10d   %s\\%s\r\n", 
                uiPartitionNumber,                                  // Partition the file lives on
                sysTime.wMonth, sysTime.wDay, sysTime.wYear,        // Date of the file
                sysTime.wHour, sysTime.wMinute, sysTime.wSecond,    // Time of the file
                findData.nFileSizeLow,                              // Size of the file
                pszDirName + 2, findData.cFileName );               // File path and name

            // Set the file pointer to the end of the file
            DWORD dwReturn = SetFilePointer( hOutputFile,
                                             0,
                                             NULL,
                                             FILE_END );

            if( 0xFFFFFFFF == dwReturn )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::DumpFiles():Failed to move our file pointer to the end of our file!! - '%s', Error - '0x%.8X (%d)'", pszDestFile, GetLastError(), GetLastError() );

                CloseHandle( hOutputFile );
                hOutputFile = NULL;

                continue;
            }

            
            // Log the file data
            if ( !WriteFile( hOutputFile,
                             pszBuffer,
                             strlen(pszBuffer),
                             &dwBytesWritten,
                             NULL ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::DumpFiles():Failed to write to the file!! - '%s', Error - '0x%.8X (%d)'", pszDestFile, GetLastError(), GetLastError() );
            }


            // Close the file
            CloseHandle( hOutputFile );
            hOutputFile = NULL;
        }

    } while( FindNextFile( hFile, &findData ) && ( g_XboxHardDrive.GetAbortProcess() != TRUE ) );

    // Close our find file
    FindClose( hFile );
    hFile = INVALID_HANDLE_VALUE;
}

// TODO: Move this to a library
HRESULT CleanTree( char* pszStartDir, char* pszFileExtension )
{
    if( ( NULL == pszStartDir ) || ( NULL == pszFileExtension ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanTree():Invalid paramter passed in!!" );

        return E_INVALIDARG;
    }

    char* pszDirPattern = new char[MAX_PATH+1];
    if( NULL == pszDirPattern )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanTree():Failed to allocate memory!!" );

        return E_FAIL;
    }

    ZeroMemory( pszDirPattern, MAX_PATH + 1 );
    _snprintf( pszDirPattern, MAX_PATH - 1, "%s\\*.*", pszStartDir );
    
    XDBGTRC( APP_TITLE_NAME_A, "CCertReqs::CleanTree():Directory Pattern - '%s'", pszDirPattern );

    HRESULT hr = S_OK;
    WIN32_FIND_DATA findData;
    ZeroMemory( &findData, sizeof( findData ) );

    HANDLE hFile = FindFirstFile( pszDirPattern, &findData );
    if( INVALID_HANDLE_VALUE == hFile )
    {
        if( ERROR_FILE_NOT_FOUND != GetLastError() )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanTree():FindFirstFile Failed!! - '%s', '0x%.8X (%d)'", pszDirPattern, GetLastError(), GetLastError() );

            hr = E_FAIL;
        }
    }
    else
    {
        do
        {
            if ( findData.cFileName[0] == '.' )
            {
			    continue;
            }

            if ( ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
            {
                char* pszNewDirPattern = new char[MAX_PATH+1];
                if( NULL == pszNewDirPattern )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanTree():Failed to allocate memory!!" );

                    continue;
                }
            
                ZeroMemory( pszNewDirPattern, MAX_PATH + 1 );
                _snprintf( pszNewDirPattern, MAX_PATH, "%s\\%s", pszStartDir, findData.cFileName );
                CleanTree( pszNewDirPattern, pszFileExtension );

                delete[] pszNewDirPattern;
                pszNewDirPattern = NULL;
            }
		    else
            //
            // It's a file
            //
		    {
                char* pszMatch = strstr( findData.cFileName, pszFileExtension );
                if( ( NULL != pszMatch ) && ( pszMatch == &findData.cFileName[strlen(findData.cFileName) - strlen(pszFileExtension)] ) )
                {
                    // Check to see if the file matches our pattern
                    char* pszFullFileName = new char[MAX_PATH+1];
                    if( NULL == pszFullFileName )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanTree():Failed to allocate memory!!" );

                        continue;
                    }

                    ZeroMemory( pszFullFileName, MAX_PATH + 1 );
                    _snprintf( pszFullFileName, MAX_PATH, "%s\\%s", pszStartDir, findData.cFileName );
			        if ( !DeleteFile( pszFullFileName ) )
                    {
                        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanTree():Failed to delete file!! - '%s'", pszFullFileName );
                    }

                    delete[] pszFullFileName;
                    pszFullFileName = NULL;
                }
		    }
        } while( FindNextFile( hFile, &findData ) && ( g_XboxHardDrive.GetAbortProcess() != TRUE ) );

        // Close our find file
        FindClose( hFile );
        hFile = INVALID_HANDLE_VALUE;
    }

    delete[] pszDirPattern;
    pszDirPattern = NULL;

    return hr;
}


HRESULT CCertReqs::CleanDir( char* pszDir, char* pszFileExtension )
{
    if( ( NULL == pszDir ) || ( NULL == pszFileExtension ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanDir():Invalid paramter passed in!!" );

        return E_INVALIDARG;
    }

    char* pszDirPattern = new char[MAX_PATH+1];
    if( NULL == pszDirPattern )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanDir():Failed to allocate memory!!" );

        return E_FAIL;
    }

    ZeroMemory( pszDirPattern, MAX_PATH + 1 );
    _snprintf( pszDirPattern, MAX_PATH - 1, "%s\\*.*", pszDir );
    
    XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanDir():Directory Pattern - '%s'", pszDirPattern );

    HRESULT hr = S_OK;
    WIN32_FIND_DATA findData;
    ZeroMemory( &findData, sizeof( findData ) );

    HANDLE hFile = FindFirstFile( pszDirPattern, &findData );
    if( INVALID_HANDLE_VALUE == hFile )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanDir():FindFirstFile Failed!! - '%s', '0x%.8X (%d)'", pszDirPattern, GetLastError(), GetLastError() );

        hr = E_FAIL;
    }
    else
    {
        do
        {
            if ( ( findData.cFileName[0] == '.' ) ||
                 ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
            {
			    continue;
            }

            //
            // It's a file
            //
            char* pszMatch = strstr( findData.cFileName, pszFileExtension );
            if( ( NULL != pszMatch ) && ( pszMatch == &findData.cFileName[strlen(findData.cFileName) - strlen(pszFileExtension)] ) )
            {
                // Check to see if the file matches our pattern
                char* pszFullFileName = new char[MAX_PATH+1];
                if( NULL == pszFullFileName )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanDir():Failed to allocate memory!!" );

                    continue;
                }

                ZeroMemory( pszFullFileName, MAX_PATH + 1 );
                _snprintf( pszFullFileName, MAX_PATH, "%s\\%s", pszDir, findData.cFileName );
			    if ( !DeleteFile( pszFullFileName ) )
                {
                    XDBGWRN( APP_TITLE_NAME_A, "CCertReqs::CleanDir():Failed to delete file!! - '%s'", pszFullFileName );
                }

                delete[] pszFullFileName;
                pszFullFileName = NULL;
            }
        } while( FindNextFile( hFile, &findData ) && ( g_XboxHardDrive.GetAbortProcess() != TRUE ) );

        // Close our find file
        FindClose( hFile );
        hFile = INVALID_HANDLE_VALUE;
    }

    delete[] pszDirPattern;
    pszDirPattern = NULL;

    return hr;
}


// Clean up any files created by the cert process
HRESULT CCertReqs::CleanUp( char* pszStartDir )
{
    if( NULL != pszStartDir )
    {
        CleanTree( pszStartDir, ".xdk" );
    }
    else
    {
        // Walk the Hard Drive and delete any XDK files
        CleanTree( "c:", ".xdk" );

        // Walk each MU and delete any XDK files
        for( unsigned int x = 0; x < NUM_XBOX_MU_SLOTS; x++ )
        {
            if( g_MemoryUnits[x].IsMounted() )
            {
                char pszDrivePath[3];
                pszDrivePath[2] = '\0';

                sprintf( pszDrivePath, "%c:", g_MemoryUnits[x].GetUDataDrive() );
                CleanTree( pszDrivePath, ".xdk" );
            }
        }
    }

    return S_OK;
}