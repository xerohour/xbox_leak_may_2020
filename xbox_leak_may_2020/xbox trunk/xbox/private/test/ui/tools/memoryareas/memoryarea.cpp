/*****************************************************
*** memoryarea.cpp
***
*** CPP file for our Memory Area class.
*** This class will allow you to perform certain
*** actions that pertain to a memory area on the
*** Xbox, such as erase, create game titles, etc.
***
*** by James N. Helm
*** March 30th, 2001
***
*****************************************************/

#include "memarpch.h"
#include "memoryarea.h"

// Constructor
CMemoryArea::CMemoryArea()
: m_pwszDefaultName( NULL ),
m_bAbortProcess( FALSE )
{
    SetUDataDrive( '\0' );

    // Initialize our Critical Section
    InitializeCriticalSection( &m_ProcessCritSec );
}

// Destructor
CMemoryArea::~CMemoryArea()
{
    if( m_pwszDefaultName )
    {
        delete[] m_pwszDefaultName;
        m_pwszDefaultName = NULL;
    }

    // Cleanup our Critical Section
    DeleteCriticalSection( &m_ProcessCritSec );
}


// Clear the Memory Area of all data
void CMemoryArea::Erase( char cDriveLetter/*='\0'*/ )
{
    char pszDir[4];
    ZeroMemory( pszDir, 4 );

    if( '\0' == cDriveLetter )
    //
    // The user did not specify a drive letter, assume the local UDATA drive letter
    //
    {
        sprintf( pszDir, "%c:", GetUDataDrive() );
    }
    else
    //
    // The user specified a drive letter, use that one instead
    //
    {
        sprintf( pszDir, "%c:", cDriveLetter );
    }

    if( !DeleteDirectory( pszDir ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::Erase():Failed to delete the directory - '%s'!!", pszDir );
    }
};


// Create a Title Meta XBX file for the specified game title
// pwszGameName and pwszLangCode can be NULL
HRESULT CMemoryArea::CreateTitleMetaXbx( char* pszGamePath, WCHAR* pwszGameName, WCHAR* pwszLangCode, WCHAR* pwszMissingLangCode, BOOL bPrependLangCodeToName/*=TRUE*/ )
{
    if( NULL == pszGamePath )
    {
        return E_INVALIDARG;
    }

    char pszFilePath[MAX_PATH+1];
    ZeroMemory( pszFilePath, MAX_PATH+1 );

    _snprintf( pszFilePath, MAX_PATH, "%s\\%s", pszGamePath, MEMORY_AREA_TITLE_META_FILENAME );

    HANDLE hMetaXbxFile = CreateFile( pszFilePath,
                                      GENERIC_WRITE,
                                      0,
                                      NULL,
                                      CREATE_ALWAYS,
                                      FILE_ATTRIBUTE_NORMAL,
                                      NULL );
    if( INVALID_HANDLE_VALUE == hMetaXbxFile )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CreateTitleMetaXbx():Failed to create the file - '%hs', Error - '0x%0.8X (%d)'", pszFilePath, GetLastError(), GetLastError() );

        return E_FAIL;
    }

    DWORD dwBytesWritten;
    WCHAR wpszGenBuffer[MAX_PATH+1];
    ZeroMemory( wpszGenBuffer, sizeof( WCHAR ) * ( MAX_PATH + 1 ) );

    //
    // Write the UNICODE signature to the file
    //
    HRESULT hr = S_OK;
    int iNumBytesInBuffer = _snwprintf( wpszGenBuffer, MAX_PATH, L"%c", MEMORY_AREA_UNICODE_SIGNATURE );

    if( !WriteFile( hMetaXbxFile,
                    wpszGenBuffer,
                    iNumBytesInBuffer * sizeof( WCHAR ),
                    &dwBytesWritten,
                    NULL ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CreateTitleMetaXbx():Failed to write to the file - '%ls', Error - '0x%0.8X (%d)'!!", wpszGenBuffer, GetLastError(), GetLastError() );

        hr = E_FAIL;
        goto cleanup;
    }

    if( NULL == pwszLangCode )
    {
        //
        // Write out our data to the file, for all languages
        //
        for( unsigned int x = 0; x < ENUM_MEMAREA_LANGUAGECODE_NUM_ITEMS; x++ )
        {
            if( ( NULL == pwszMissingLangCode ) || ( 0 != wcscmp( pwszMissingLangCode, g_wpszMemAreaLanguageCodes[x] ) ) )
            {
                if( NULL == pwszGameName )
                {
                    if( bPrependLangCodeToName )
                    {
                        iNumBytesInBuffer = _snwprintf( wpszGenBuffer, MAX_PATH, L"%ls\r\n%ls%ls:Test Game %c%c\r\n", g_wpszMemAreaLanguageCodes[x], MEMORY_AREA_TITLE_META_TITLENAME_TOKEN, g_wpszMemAreaLanguageCodes[x], ( rand() % 74 ) + 48, ( rand() % 74 ) + 48 );
                    }
                    else
                    {
                        iNumBytesInBuffer = _snwprintf( wpszGenBuffer, MAX_PATH, L"%ls\r\n%lsTest Game %c%c\r\n", g_wpszMemAreaLanguageCodes[x], MEMORY_AREA_TITLE_META_TITLENAME_TOKEN, ( rand() % 74 ) + 48, ( rand() % 74 ) + 48 );
                    }
                }
                else
                {
                    if( bPrependLangCodeToName )
                    {
                        iNumBytesInBuffer = _snwprintf( wpszGenBuffer, MAX_PATH, L"%ls\r\n%ls%ls:%ls\r\n", g_wpszMemAreaLanguageCodes[x], MEMORY_AREA_TITLE_META_TITLENAME_TOKEN, g_wpszMemAreaLanguageCodes[x], pwszGameName );
                    }
                    else
                    {
                        iNumBytesInBuffer = _snwprintf( wpszGenBuffer, MAX_PATH, L"%ls\r\n%ls%ls\r\n", g_wpszMemAreaLanguageCodes[x], MEMORY_AREA_TITLE_META_TITLENAME_TOKEN, pwszGameName );
                    }
                }

                if( !WriteFile( hMetaXbxFile,
                                wpszGenBuffer,
                                iNumBytesInBuffer * sizeof( WCHAR ),
                                &dwBytesWritten,
                                NULL ) )
                {
                    XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CreateTitleMetaXbx():Failed to write to the file - '%ls', Error - '0x%0.8X (%d)'!!", wpszGenBuffer, GetLastError(), GetLastError() );

                    hr = E_FAIL;
                    goto cleanup;
                }
            }
        }
    }
    else
    {
        //
        // Write out our data for the specified lang code, ONLY
        //
        if( NULL == pwszGameName )
        {
            if( bPrependLangCodeToName )
            {
                iNumBytesInBuffer = _snwprintf( wpszGenBuffer, MAX_PATH, L"%ls\r\n%ls%ls:Test Game %c%c\r\n", pwszLangCode, MEMORY_AREA_TITLE_META_TITLENAME_TOKEN, pwszLangCode, ( rand() % 74 ) + 48, ( rand() % 74 ) + 48 );
            }
            else
            {
                iNumBytesInBuffer = _snwprintf( wpszGenBuffer, MAX_PATH, L"%ls\r\n%lsTest Game %c%c\r\n", pwszLangCode, MEMORY_AREA_TITLE_META_TITLENAME_TOKEN, ( rand() % 74 ) + 48, ( rand() % 74 ) + 48 );
            }
        }
        else
        {
            if( bPrependLangCodeToName )
            {
                iNumBytesInBuffer = _snwprintf( wpszGenBuffer, MAX_PATH, L"%ls\r\n%ls%ls:%ls\r\n", pwszLangCode, MEMORY_AREA_TITLE_META_TITLENAME_TOKEN, pwszLangCode, pwszGameName );
            }
            else
            {
                iNumBytesInBuffer = _snwprintf( wpszGenBuffer, MAX_PATH, L"%ls\r\n%ls%ls\r\n", pwszLangCode, MEMORY_AREA_TITLE_META_TITLENAME_TOKEN, pwszGameName );
            }
        }

        if( !WriteFile( hMetaXbxFile,
                        wpszGenBuffer,
                        iNumBytesInBuffer * sizeof( WCHAR ),
                        &dwBytesWritten,
                        NULL ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CreateTitleMetaXbx():Failed to write to the file - '%ls', Error - '0x%0.8X (%d)'!!", wpszGenBuffer, GetLastError(), GetLastError() );

            hr = E_FAIL;
            goto cleanup;
        }
    }

cleanup:

    CloseHandle( hMetaXbxFile );
    hMetaXbxFile = INVALID_HANDLE_VALUE;

    return hr;
}


// Create a Game Title in the specified directory
HRESULT CMemoryArea::CreateGameTitle( char cDriveLetter, char* pszTitleDir, WCHAR* pwszGameName/*=NULL*/, BOOL bPrependLangCodeToName/*=TRUE*/ )
{
    // Check to make sure we were passed a file TitleDir
    if( !pszTitleDir )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateGameTitle():Invalid parameter!!" );

        return E_INVALIDARG;
    }

    char pszTitlePath[MAX_PATH];
    ZeroMemory( pszTitlePath, MAX_PATH );

    _snprintf( pszTitlePath, MAX_PATH, "%c:\\%s", cDriveLetter, pszTitleDir );

    // Create the directory for the Game
    if( !CreateDirectory( pszTitlePath, NULL ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateGameTitle():Failed to create the directory - '%s', Error - '0x%.8X (%d)'", pszTitlePath, GetLastError(), GetLastError() );

        return E_FAIL;
    }

    HRESULT hr = CreateTitleMetaXbx( pszTitlePath, pwszGameName, NULL, NULL, bPrependLangCodeToName );
    if( FAILED( hr ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateGameTitle():Failed to create the titlemeta.xbx file!! TitlePath - '%hs', Error - '0x%0.8X (%d)'!!", pszTitlePath, hr, hr );
    }

    hr = CopyGameImages( pszTitlePath );
    if( FAILED( hr ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateGameTitle():Failed to copy the saved game images!! TitlePath - '%hs', Error - '0x%0.8X (%d)'!!", pszTitlePath, hr, hr );
    }

    return S_OK;
}


// Copy the Game images in to the title directory
HRESULT CMemoryArea::CopyGameImages( char* pszTitleDir, BOOL bCopyTitleImage/*=TRUE*/, BOOL bCopyDefSaveImage/*=TRUE*/ )
{
    char pszDestFilePath[MAX_PATH+1];

    if( bCopyTitleImage )
    {
        ZeroMemory( pszDestFilePath, MAX_PATH+1 );
        _snprintf( pszDestFilePath, MAX_PATH, "%s\\%s", pszTitleDir, MEMORY_AREA_TITLE_IMAGE_FILENAME );

        // Copy the Game Title image to the correct location
        if( !CopyFile( MEMORY_AREA_TITLE_IMAGE_PATHINFO, pszDestFilePath, FALSE ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CopyGameImages():Failed to copy the file!! Source - '%hs', Dest - '%hs', Error - '0x%0.8X (%d)'!!", MEMORY_AREA_TITLE_IMAGE_PATHINFO, pszDestFilePath, GetLastError(), GetLastError() );
        }
        else
        {
            SetFileAttributes( pszDestFilePath, FILE_ATTRIBUTE_NORMAL );
        }
    }

    if( bCopyDefSaveImage )
    {
        ZeroMemory( pszDestFilePath, MAX_PATH+1 );
        _snprintf( pszDestFilePath, MAX_PATH, "%s\\%s", pszTitleDir, MEMORY_AREA_SAVEGAME_IMAGE_FILENAME );

        // Copy the Default Save Image to the correct location
        if( !CopyFile( MEMORY_AREA_DEFSAVE_IMAGE_PATHINFO, pszDestFilePath, FALSE ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CopyGameImages():Failed to copy the file!! Source - '%hs', Dest - '%hs', Error - '0x%0.8X (%d)'!!", MEMORY_AREA_DEFSAVE_IMAGE_PATHINFO, pszDestFilePath, GetLastError(), GetLastError() );
        }
        else
        {
            SetFileAttributes( pszDestFilePath, FILE_ATTRIBUTE_NORMAL );
        }
    }

    return S_OK;
}


// Create a Saved Game in the specified directory
HRESULT CMemoryArea::CreateSavedGame( IN WCHAR* pwszSavedGameName,
                                      OUT char* pszGamePath,
                                      IN unsigned int cbBuffSize,
                                      IN SYSTEMTIME* pSysTimeGameDate,
                                      IN char* pszSaveImage,
                                      IN BOOL bNoCopyGame )
{
    // Check to make sure we were passed a file TitleDir
    if( NULL == pwszSavedGameName )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateSavedGame():Invalid parameter!!" );

        return E_INVALIDARG;
    }

    char pszDriveInfo[4];
    pszDriveInfo[3] = '\0';
    // sprintf( pszDriveInfo, "%c:\\", cDriveLetter );
    sprintf( pszDriveInfo, "%c:\\", GetUDataDrive() );

    unsigned int uiBufSize = 0;
    char* pszGameBuffer;
    if( NULL == pszGamePath )
    {
        pszGameBuffer = new char[MAX_PATH+1];
        if( NULL == pszGameBuffer )
        {
            // TODO: Warn about failure and clean up!!
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateSavedGame():Failed to allocate memory!!" );
        }

        ZeroMemory( pszGameBuffer, MAX_PATH+1 );
        uiBufSize = MAX_PATH;
    }
    else
    {
        pszGameBuffer = pszGamePath;
        uiBufSize = cbBuffSize;
    }

    DWORD dwCreateFlags = 0;
    if( bNoCopyGame )
    {
        dwCreateFlags = XSAVEGAME_NOCOPY;
    }

    DWORD dwReturn = XCreateSaveGame( pszDriveInfo,
                                      pwszSavedGameName,
                                      OPEN_ALWAYS,
                                      dwCreateFlags,
                                      pszGameBuffer,
                                      uiBufSize );

    if( ERROR_SUCCESS != dwReturn )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateSavedGame():Failed to Create the Saved Game!! Error - '0x%.8X (%d)'", dwReturn, dwReturn );
    }
    else
    {
        if( NULL != pszSaveImage )
        {
            // Copy the Save Game image in to the directory
            char pszDestFilePath[MAX_PATH];

            ZeroMemory( pszDestFilePath, MAX_PATH );
            _snprintf( pszDestFilePath, MAX_PATH, "%s%s", pszGameBuffer, MEMORY_AREA_SAVEGAME_IMAGE_FILENAME );

            if( 0 == strcmp( "", pszSaveImage ) )
            //
            // Use the default image info
            //
            {
                // Copy the Game Title image to the correct location
                if( !CopyFile( MEMORY_AREA_SAVEGAME_IMAGE_PATHINFO, pszDestFilePath, FALSE ) )
                {
                    XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateSavedGame():Failed to copy the file!! Source - '%hs', Dest - '%hs', Error - '0x%0.8X (%d)'!!", MEMORY_AREA_SAVEGAME_IMAGE_PATHINFO, pszDestFilePath, GetLastError(), GetLastError() );
                }
                else
                {
                    SetFileAttributes( pszDestFilePath, FILE_ATTRIBUTE_NORMAL );
                }
            }
            else
            //
            // Use the passed in image info
            //
            {
                // Copy the Game Title image to the correct location
                if( !CopyFile( pszSaveImage, pszDestFilePath, FALSE ) )
                {
                    XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateSavedGame():Failed to copy the file!! Source - '%hs', Dest - '%hs', Error - '0x%0.8X (%d)'!!", pszSaveImage, pszDestFilePath, GetLastError(), GetLastError() );
                }
                else
                {
                    SetFileAttributes( pszDestFilePath, FILE_ATTRIBUTE_NORMAL );
                }
            }
        }

        // Set the Date of the Saved Game if necessary
        if( NULL != pSysTimeGameDate )
        {
            pszGameBuffer[strlen(pszGameBuffer) - 1] = '\0';

            // Set the Date / Time of the directory
            HANDLE hDir = CreateFile( pszGameBuffer, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );

            if( INVALID_HANDLE_VALUE == hDir )
            {
                XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateSavedGame():Failed to open the Save Game Directory - '%s', Error - '0x%0.8X (%d)'", pszGameBuffer, GetLastError(), GetLastError() );
            }
            else
            //
            // The handle is valid, Set the file date and time
            //
            {
                FILETIME fileTime;

                if( !SystemTimeToFileTime( pSysTimeGameDate, &fileTime ) )
                {
                    XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateSavedGame():SystemTimeToFileTime Failed!! Error - '0x%0.8X (%d)'", GetLastError(), GetLastError() );
                }

                if( !SetFileTime( hDir, &fileTime, &fileTime, &fileTime ) )
                {
                    XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateSavedGame():Failed to Set Filetime for '%hs', Error - '0x%0.8X (%d)'", pszGameBuffer, GetLastError(), GetLastError() );
                }

                CloseHandle( hDir );
            }
        }
    }

    return S_OK;
}


// Corrupt the specified partition by blanking out the initial page
BOOL CMemoryArea::CorruptPartition( POBJECT_STRING VolumePath )
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    IO_STATUS_BLOCK IoStatusBlock;
    HANDLE VolumeHandle;
    DWORD dwNumBytesWritten;
    BYTE pbData[PAGE_SIZE];
    BOOL bReturn = TRUE;

    ZeroMemory( pbData, PAGE_SIZE );

    //
    // Open the volume for read/write access.
    //

    InitializeObjectAttributes(&ObjectAttributes, VolumePath,
        OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = NtOpenFile(&VolumeHandle, SYNCHRONIZE | FILE_READ_DATA |
        FILE_WRITE_DATA, &ObjectAttributes, &IoStatusBlock, 0,
        FILE_SYNCHRONOUS_IO_ALERT | FILE_NO_INTERMEDIATE_BUFFERING);

    if (!NT_SUCCESS(status)) {
        SetLastError(RtlNtStatusToDosError(status));
        
        return FALSE;
    }

    //
    // Write 0's to the volume
    //
    if( !WriteFile( VolumeHandle,
                    &pbData,
                    PAGE_SIZE,
                    &dwNumBytesWritten,
                    NULL ) )
    {
        bReturn = FALSE;
    }

    NtClose(VolumeHandle);

    return bReturn;
}


// Get the number of blocks that are free on the Memory Area
DWORD CMemoryArea::GetFreeBlocks()
{
    DWORD dwTotalBlocks, dwFreeBlocks;

    if( FAILED( CalcSpace( &dwTotalBlocks, &dwFreeBlocks ) ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::GetFreeBlocks():CalcSpace failed!!" );
    }

    return dwFreeBlocks;
}


// Get the total number of blocks on the Memory Area
DWORD CMemoryArea::GetTotalBlocks()
{
    DWORD dwTotalBlocks, dwFreeBlocks;

    if( FAILED( CalcSpace( &dwTotalBlocks, &dwFreeBlocks ) ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::GetTotalBlocks():CalcSpace failed!!" );
    }

    return dwTotalBlocks;
}


// Fill the Memory Area with a specified Number of Blocks worth of data
#define MEMORY_AREA_FILL_FILE_PATTERN  "xdk_data_"
void CMemoryArea::FillArea( char* pszDirectory, unsigned int uiNumBlocks )
{
    // Check to make sure we are going to fill the proper directory
    if( !pszDirectory )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FillArea():Invalid Directory passed in!!" );

        return;
    }

    // Check to make sure we can create the files
    if( uiNumBlocks > GetTotalBlocks() )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FillArea():Too many blocks specified!  Specified - '%d', Max - '%d'", uiNumBlocks, GetFreeBlocks() );

        return;
    }

    unsigned int uiCounter = 0;
    LARGE_INTEGER liBlockInformation;       // Convert the user passed in blocks to bytes
    liBlockInformation.QuadPart = 0;

    if( MEMORY_AREA_BLOCK_SIZE_MAX_NUM_LOW_PART < uiNumBlocks )
    {
        uiCounter = uiNumBlocks / MEMORY_AREA_BLOCK_SIZE_MAX_NUM_LOW_PART;
        uiNumBlocks = uiNumBlocks - ( uiCounter * MEMORY_AREA_BLOCK_SIZE_MAX_NUM_LOW_PART );
        liBlockInformation.LowPart = (DWORD)MEMORY_AREA_BLOCK_SIZE_MAX_NUM_LOW_PART * (DWORD)MEMORY_AREA_BLOCK_SIZE_IN_BYTES;
    }

    char pszFilePath[MAX_PATH+1];
    ZeroMemory( pszFilePath, MAX_PATH+1 );

    // For each "high part" of the QuadWord, we should create a huge file
    unsigned int uiModifier = 0;
    for( unsigned int x = 0; x < uiCounter; ++x )
    {
        _snprintf( pszFilePath, MAX_PATH, "%s%s%d.xdk", pszDirectory, MEMORY_AREA_FILL_FILE_PATTERN, x + uiModifier );
        while( FileExists( pszFilePath ) )
        {
            _snprintf( pszFilePath, MAX_PATH, "%s%s%d.xdk", pszDirectory, MEMORY_AREA_FILL_FILE_PATTERN, x + (++uiModifier) );

            if( 4096 == uiModifier )
            {
                XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FillArea():Unable to create a unique (large) file!!" );

                return;
            }
        }

        HRESULT hr = CreateFileOfSize( pszFilePath, liBlockInformation );
        if( FAILED( hr ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FillArea():CreateFileOfSize Failed!! - '0x%0.8X (%d)'", hr, hr );
        }
    }

    //
    // Create a file for the smaller block size
    //

    liBlockInformation.LowPart = uiNumBlocks * MEMORY_AREA_BLOCK_SIZE_IN_BYTES;

    _snprintf( pszFilePath, MAX_PATH, "%s%s%d.xdk", pszDirectory, MEMORY_AREA_FILL_FILE_PATTERN, x + (++uiModifier) );
    while( FileExists( pszFilePath ) )
    {
        _snprintf( pszFilePath, MAX_PATH, "%s%s%d.xdk", pszDirectory, MEMORY_AREA_FILL_FILE_PATTERN, x + (++uiModifier) );

        if( 4096 == uiModifier )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FillArea():Unable to create a unique (small) file!!" );

            return;
        }
    }

    HRESULT hr = CreateFileOfSize( pszFilePath, liBlockInformation );
    if( FAILED( hr ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FillArea():CreateFileOfSize Failed!! - '0x%0.8X (%d)'", hr, hr );
    }
}


// Creates a file of a specified size
HRESULT CMemoryArea::CreateFileOfSize( char* pszFileName, LARGE_INTEGER liNumBytes )
{
    if( NULL == pszFileName )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateFileOfSize():Invalid parameter passed in!! ");

        return E_INVALIDARG;
    }

    HANDLE hFile = CreateFile( pszFileName,
                               GENERIC_WRITE,
                               0,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL );

    if( INVALID_HANDLE_VALUE == hFile )
    //
    // We did not create the file, inform the user and do not proceed
    //
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateFileOfSize():Could not create the file - %s, Error - '0x%.8X (%d)'!!", pszFileName, GetLastError(), GetLastError() );

        return E_FAIL;
    }

    // Set the file pointer to be as large as the free space on the disk
    if( !SetFilePointerEx( hFile, liNumBytes, NULL, FILE_CURRENT ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateFileOfSize():Failed to set the File Pointer - '0x%.8X (%d)'", GetLastError(), GetLastError() );
    }

    // Set the END OF FILE to the new location
    if( !SetEndOfFile( hFile ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateFileOfSize():Failed to Set EOF - '0x%.8X (%d)'", GetLastError(), GetLastError() );
    }

    // Close the file
    CloseHandle( hFile );
    hFile = NULL;

    return S_OK;
}


// Adjusts the file size by the specified amount
HRESULT CMemoryArea::AdjustFileSize( char* pszFileName, LARGE_INTEGER liNumBytes )
{
    if( NULL == pszFileName )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::AdjustFileSize():Invalid parameter passed in!! ");

        return E_INVALIDARG;
    }

    HANDLE hFile = CreateFile( pszFileName,
                               GENERIC_WRITE,
                               0,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL );

    if( INVALID_HANDLE_VALUE == hFile )
    //
    // We did not create the file, inform the user and do not proceed
    //
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::AdjustFileSize():Could not create the file - %s, Error - '0x%.8X (%d)'!!", pszFileName, GetLastError(), GetLastError() );

        return E_FAIL;
    }

    // Figure out how to adjust our file
    LARGE_INTEGER liFileSize;
    ZeroMemory( &liFileSize, sizeof( liFileSize ) );

    if( !GetFileSizeEx( hFile, &liFileSize ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::AdjustFileSize():Failed to get the File Size - '%s', '0x%.8X (%d)'", pszFileName, GetLastError(), GetLastError() );
    }

    LARGE_INTEGER liAdjustFileSize;
    liAdjustFileSize.QuadPart = liFileSize.QuadPart + liNumBytes.QuadPart;

    HRESULT hr = S_OK;
    // Set the file pointer to be as large as the free space on the disk
    if( !SetFilePointerEx( hFile, liAdjustFileSize, NULL, FILE_BEGIN ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::AdjustFileSize():Failed to set the File Pointer - '0x%.8X (%d)'", GetLastError(), GetLastError() );

        hr = E_FAIL;
    }

    // Set the END OF FILE to the new location
    if( !SetEndOfFile( hFile ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::AdjustFileSize():Failed to Set EOF - '0x%.8X (%d)'", GetLastError(), GetLastError() );

        hr = E_FAIL;
    }

    // Close the file
    CloseHandle( hFile );
    hFile = NULL;

    return hr;
}


// Retreives block information for the current MU
HRESULT CMemoryArea::CalcSpace( DWORD* pdwTotalBlocks, DWORD* pdwFreeBlocks )
{
    *pdwTotalBlocks = 0;
    *pdwFreeBlocks = 0;

    ULARGE_INTEGER uliFreeBytesAvailable, uliTotalNumBytes, uliTotalNumFreeBytes;

    char pszDirectory[4];   // Used to hold the directory, which will always be the root
    ZeroMemory( pszDirectory, 4 );
    sprintf( pszDirectory, "%c:\\", GetUDataDrive());

    // Get the disk space information for the MU -- If there was a problem, return 0
    if( !GetDiskFreeSpaceEx( pszDirectory, &uliFreeBytesAvailable, &uliTotalNumBytes, &uliTotalNumFreeBytes ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CalcSpace():Failed to 'GetDiskFreeSpaceEx'!! - '%s', '0x%.8X (%d)'", pszDirectory, GetLastError(), GetLastError() );

        return E_FAIL;
    }

    // Calculate the block sizes, and store the values in our parameters
    *pdwTotalBlocks = (DWORD)( uliTotalNumBytes.QuadPart / MEMORY_AREA_BLOCK_SIZE_IN_BYTES );
    *pdwFreeBlocks  = (DWORD)( uliTotalNumFreeBytes.QuadPart / MEMORY_AREA_BLOCK_SIZE_IN_BYTES );

    return S_OK;
}


// Copy files from one directory to another
void CMemoryArea::CopyDirs( char* pszSource, char* pszDest )
{
    if( NULL == pszSource || NULL == pszDest )
    {
        return;
    }

    char pszBuf[MAX_PATH+1];
    char pszNewDest[MAX_PATH+1];

    ZeroMemory( pszBuf, MAX_PATH+1 );
    ZeroMemory( pszNewDest, MAX_PATH+1 );

    sprintf( pszBuf, "%s\\*.*", pszSource );

    WIN32_FIND_DATA fd;
    HANDLE h = FindFirstFile( pszBuf, &fd );
    if ( h != INVALID_HANDLE_VALUE )
    //
    // Valid handle value -- There must be files or sub-directories
    //
    {
        // Create the destination directory
        if( !CreateDirectory( pszDest, NULL ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CopyDirs():Failed to Create Destination Dir - '%hs', Error - '0x%0.8X (%d)'", pszDest, GetLastError(), GetLastError() );
        }

        do
        {
            // Skip over "." and ".." directories
            if( fd.cFileName[0] == '.' )
            {
                continue;
            }

            _snprintf( pszBuf, MAX_PATH, "%s\\%s", pszSource, fd.cFileName );
            _snprintf( pszNewDest, MAX_PATH, "%s\\%s", pszDest, fd.cFileName );

            // If it's a directory, copy that entire directory
            if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
            {
                CopyDirs( pszBuf, pszNewDest );
            }
            else
            {
                // Copy the file
                if( !CopyFile( pszBuf, pszNewDest, FALSE ) )
                {
                    XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CopyDirs():Failed to Copy the File.  Source - '%hs', Dest - '%hs', Error - '0x%0.8X (%d)'", pszBuf, pszNewDest, GetLastError(), GetLastError() );
                }
                else
                {
                    SetFileAttributes( pszNewDest, FILE_ATTRIBUTE_NORMAL );
                }
            }
        }
        while ( FindNextFile( h, &fd ) );

        FindClose( h );
        h = INVALID_HANDLE_VALUE;
    }
    else
    //
    // Invalid handle value -- FindFirstFile failed!
    //
    {
        // If we didn't find a file, that's OK
        if( ERROR_FILE_NOT_FOUND != GetLastError() )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CopyDirs():FindFirstFile Failed - '%s', Error - '0x%0.8X (%d)'", pszBuf, GetLastError(), GetLastError() );
        }
    }
}


// Delete a directory and all files within
BOOL CMemoryArea::DeleteDirectory( char* pszPath )
{
    // DO NOT use a trailing slash

    char pszBuf[MAX_PATH+1];
    ZeroMemory( pszBuf, MAX_PATH+1 );

    sprintf( pszBuf, "%s\\*.*", pszPath );

    WIN32_FIND_DATA fd;
    HANDLE h = FindFirstFile( pszBuf, &fd );
    if ( h != INVALID_HANDLE_VALUE )
    //
    // Valid handle value -- There must be files or sub-directories
    //
    {
        do
        {
            // Skip over "." and ".." directories
            // BUGBUG: Skip over Dash directories on the TDATA drive
            // BUGBUG: We should NOT skip these while we are in the launcher -- We can allow ALL data to be nuked
            // if ( ( fd.cFileName[0] == '.' ) ||
            //      ( ( strcmp( fd.cFileName, FILE_DATA_XDASH_TITLEID  ) == 0 ) && ( FILE_DATA_TDATA_DRIVE_LETTER_A == pszPath[0] ) ) )

            if( fd.cFileName[0] == '.' )
            {
                continue;
            }


            _snprintf( pszBuf, MAX_PATH, "%s\\%s", pszPath, fd.cFileName );

            if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
            {
                if ( !DeleteDirectory( pszBuf ) )
                {
                    return FALSE;
                }
            }
            else
            {
                if ( !DeleteFile( pszBuf ) )
                {
                    return FALSE;
                }
            }
        }
        while ( FindNextFile( h, &fd ) );

        FindClose( h );
        h = NULL;
    }
    else
    //
    // Invalid handle value -- FindFirstFile failed!
    //
    {
        // If we didn't find a file, the directory is already deleted!
        if( ERROR_FILE_NOT_FOUND != GetLastError() )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::DeleteDirectory():FindFirstFile Failed - '%s', Error - '0x%.8X (%d)'", pszBuf, GetLastError(), GetLastError() );
        }
    }

    BOOL fReturn;

    // Check to see if we are dealing with a rool level directory
    if( ( strlen( pszPath ) == 2 ) && ( pszPath[1] == ':' ) )
    //
    // Yes we are dealing with a root level directory, keep the directory
    //
    {
        fReturn = TRUE;
    }
    else
    //
    // No we are not dealing with a root level directory, remove the directory
    //
    {
        fReturn = RemoveDirectory( pszPath ) != FALSE;
    }

    return fReturn;
}


// Will fragment the specified directory, and all directories within
HRESULT CMemoryArea::FragmentTree( char* pszStartDirectory )
{
    if( NULL == pszStartDirectory )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentTree():Inavlid parameter passed in!!" );

        return E_INVALIDARG;
    }

    if( GetAbortProcess() == TRUE )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentTree():Aborting process..." );

        return S_OK;
    }

    // Fragment the current directory
    if( FAILED( FragmentDirectory( pszStartDirectory ) ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentTree():Failed to fragment the directory!! - '%s'", pszStartDirectory );
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentTree():Make sure it is a valid directory!!" );
    }

    char pszSearchPath[MAX_PATH];
    WIN32_FIND_DATA findData;

    ZeroMemory( pszSearchPath, MAX_PATH );
    ZeroMemory( &findData, sizeof( findData ) );

    // If the user passed a trailing '\', we should not append another one
    if( '\\' == pszStartDirectory[strlen(pszStartDirectory) - 1] )
    {
        _snprintf( pszSearchPath, MAX_PATH - 1, "%s*", pszStartDirectory );
    }
    else
    {
        _snprintf( pszSearchPath, MAX_PATH - 1, "%s\\*", pszStartDirectory );
    }

    HANDLE hFile = FindFirstFile( pszSearchPath, &findData );
    if( INVALID_HANDLE_VALUE == hFile )
    {
        if( ERROR_FILE_NOT_FOUND == GetLastError() )
        {
            // This is ok, as there are no files in the tree

            return S_OK;
        }
        else
        {
            // We should always find at least one file on the root.  Nothing should really
            // erase the TDATA and UDATA directories, etc
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentTree():Invalid Handle Value returned!! - '%s', Error - '0x%.8X (%d)'", pszSearchPath, GetLastError(), GetLastError() );

            return E_FAIL;
        }
    }

    do
    {
        if( FILE_ATTRIBUTE_DIRECTORY & findData.dwFileAttributes )
        {
            char pszFullDirPath[MAX_PATH];
            ZeroMemory( pszFullDirPath, MAX_PATH );

            // If the user passed a trailing '\', we should not append another one
            if( '\\' == pszStartDirectory[strlen(pszStartDirectory) - 1] )
            {
                _snprintf( pszFullDirPath, MAX_PATH - 1, "%s%s", pszStartDirectory, findData.cFileName );
            }
            else
            {
                _snprintf( pszFullDirPath, MAX_PATH - 1, "%s\\%s", pszStartDirectory, findData.cFileName );
            }

            if( FAILED( FragmentTree( pszFullDirPath ) ) )
            {
                XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentTree():Failed to fragment the directory!! - '%s'", pszFullDirPath );
            }
        }
    } while( FindNextFile( hFile, &findData ) && ( GetAbortProcess() == FALSE ) );

    // Close our file handle
    FindClose( hFile );
    hFile = NULL;

    return S_OK;
}


// Fragments all files in a given directory
HRESULT CMemoryArea::FragmentDirectory( char* pszDirPath )
{
    if( NULL == pszDirPath )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentDirectory():Invalid argument passed in!!" );

        return E_INVALIDARG;
    }

    char pszSearchPath[MAX_PATH];
    WIN32_FIND_DATA findData;

    ZeroMemory( pszSearchPath, MAX_PATH );
    ZeroMemory( &findData, sizeof( findData ) );

    // If the user passed a trailing '\', we should not append another one
    if( '\\' == pszDirPath[strlen(pszDirPath) - 1] )
    {
        _snprintf( pszSearchPath, MAX_PATH - 1, "%s*", pszDirPath );
    }
    else
    {
        _snprintf( pszSearchPath, MAX_PATH - 1, "%s\\*", pszDirPath );
    }

    // Create a linked list object to hold our file names
    // and specify that the memory will need to be deleted when
    // the object is destroyed
    CLinkedList< char* > fileNames;
    fileNames.SetDeleteDataItem( TRUE );

    // Create a list of all files in the directory
    HANDLE hFile = FindFirstFile( pszSearchPath, &findData );
    if( INVALID_HANDLE_VALUE == hFile )
    {
        if( ERROR_FILE_NOT_FOUND == GetLastError() )
        {
            // This is ok, as there are no files in this directory

            return S_OK;
        }
        else
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentDirectory():Invalid Handle Value returned!! - '%s', '0x%.8X (%d)'", pszSearchPath, GetLastError(), GetLastError() );

            return E_FAIL;
        }
    }

    do
    {
        // Skip the entry if it is a directory
        if( FILE_ATTRIBUTE_DIRECTORY & findData.dwFileAttributes )
        {
            continue;
        }

        // Add the filename to our list
        int iFileNameLength = strlen( findData.cFileName );
        char* pszFName = new char[iFileNameLength + 1]; // We don't need to free this memory, as the linked list class will take care of it for us
        if( NULL == pszFName )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentDirectory():Failed to allocate memory!!" );

            // BUGBUG: Should probably exit the function at this point, as I doubt
            // any other allocations will succeed
            continue;
        }

        strcpy( pszFName, findData.cFileName );
        fileNames.AddNode( pszFName );
    } while( ( FindNextFile( hFile, &findData ) ) && ( GetAbortProcess() == FALSE ) );

    // Close our file handle
    FindClose( hFile );
    hFile = NULL;

    if( ( fileNames.GetNumItems() > 0 ) && ( GetAbortProcess() == FALSE )  )
    {
        // Make sure we are at the beginning of our list
        fileNames.MoveTo( 0 );

        // Loop on each file and Call FragmentFile
        for( unsigned int x = 0; ( x < fileNames.GetNumItems() ) && ( GetAbortProcess() == FALSE ); ++x )
        {
            HRESULT hr = FragmentFile( pszDirPath, fileNames.GetCurrentNode() );
            // Check to see if we encountered something unexpected
            // if so, try the opertation again, otherwise, check for failure
            if( E_UNEXPECTED == hr )
            {
                // TODO: Turn this in to a TRACE, instead of a WARNING
                XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentDirectory():Encountered something unexpected, trying again!!" );

                if( FAILED( FragmentFile( pszDirPath, fileNames.GetCurrentNode() ) ) )
                {
                    XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentDirectory():FragmentFile failed for - '%s', '%s'", pszDirPath, fileNames.GetCurrentNode() );
                }
            }
            else
            {
                if( FAILED( hr ) )
                {
                    XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentDirectory():FragmentFile failed for - '%s', '%s'", pszDirPath, fileNames.GetCurrentNode() );
                }
            }

            fileNames.MoveNext();
        }
    }

    return S_OK;
}


// Fragments an individual file
HRESULT CMemoryArea::FragmentFile( char* pszFilePath, char* pszFileName )
{
    if( ( NULL == pszFilePath ) || ( NULL == pszFileName ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentFile():Invalid argument passed in!!" );

        return E_INVALIDARG;
    }

    // Make sure the Z drive is clear (cache drive) before beginning
    DeleteDirectory( "z:" );

    char pszSourceFilePath[MAX_PATH];
    char pszDestFilePath[MAX_PATH];

    ZeroMemory( pszSourceFilePath, MAX_PATH );
    ZeroMemory( pszDestFilePath, MAX_PATH );

    // If the user passed a trailing '\', we should not append another one
    if( '\\' == pszFilePath[strlen(pszFilePath) - 1] )
    {
        _snprintf( pszSourceFilePath, MAX_PATH - 1, "%s%s", pszFilePath, pszFileName );
    }
    else
    {
        _snprintf( pszSourceFilePath, MAX_PATH - 1, "%s\\%s", pszFilePath, pszFileName );
    }
    _snprintf( pszDestFilePath, MAX_PATH - 1, "z:\\%s", pszFileName );

    // Move the file to the Z drive
    if( !MoveFileEx( pszSourceFilePath,
                     pszDestFilePath,
                     MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentFile():Failed to move the file!!\n  Source - '%s'\n  Dest - '%s', Error - '0x%.8X (%d)'", pszSourceFilePath, pszDestFilePath, GetLastError(), GetLastError() );

        return E_FAIL;
    }

    DWORD dwFileSize = GetMAFileSize( pszDestFilePath );
    if( 0xFFFFFFFF == dwFileSize )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentFile():Failed to obtain the size of our file!! - '%s', Error - '0x%.8X (%d)'", pszDestFilePath, GetLastError(), GetLastError() );

        return E_FAIL;
    }


    // Fill the drive's remaining space
    DWORD dwBlocksFree = GetFreeBlocks();
    LARGE_INTEGER liFileSizeToCreate;
    liFileSizeToCreate.QuadPart = dwBlocksFree * MEMORY_AREA_BLOCK_SIZE_IN_BYTES;

    CreateFileOfSize( MEMORY_AREA_FRAG_FILL_FILENAME, liFileSizeToCreate );

    //
    // Shrink our files to cause fragmentation
    //
    static DWORD dwCurrentFile = 0; // Tracks the current file we are shrinking, so we can always be rotating through our files
    DWORD dwNumFilesToAdjust = ( dwFileSize / MEMORY_AREA_BLOCK_SIZE_IN_BYTES ) + 1;
    char pszShrinkFileName[MAX_PATH];
    for( unsigned int uiBlockCount = 0; uiBlockCount < dwNumFilesToAdjust; uiBlockCount++ )
    {
        ZeroMemory( pszShrinkFileName, MAX_PATH );
        _snprintf( pszShrinkFileName, MAX_PATH - 1, "%s\\%s%d.xdk", MEMORY_AREA_FRAG_FILE_DIR, MEMORY_AREA_FRAG_FILE_PATTERN, dwCurrentFile++ );
        if( MEMORY_AREA_FRAG_NUM_FILES == dwCurrentFile )
        {
            dwCurrentFile = 0;
        }

        // Adjust the size of a file by negative 1 block
        LARGE_INTEGER liAdjustSize;
        liAdjustSize.QuadPart = MEMORY_AREA_BLOCK_SIZE_IN_BYTES * -1;
        if( FAILED( AdjustFileSize( pszShrinkFileName, liAdjustSize ) ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentFile():Failed to adjust our file, performing cleanup!! - '%s', '0x%.8X (%d)'", pszShrinkFileName, GetLastError(), GetLastError() );

            // Delete our frag file directory, and our temp filler file
            DeleteDirectory( MEMORY_AREA_FRAG_FILE_DIR );

            // Move the file back over from the cache partition
            if( !MoveFileEx( pszDestFilePath,
                             pszSourceFilePath,
                             MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH ) )
            {
                XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentFile():Failed to move the file back!!\n  Source - '%s'\n  Dest - '%s'", pszDestFilePath, pszSourceFilePath );
            }

            // Re-create our frag temp files (as if we just started the fragmentation process)
            CreateFragTempFiles();

            // Set our static file pointer to file #1
            dwCurrentFile = 0;

            return E_UNEXPECTED;
        }
    }

    // Attempt to Move the file back to the Local Drive
    if( !MoveFileEx( pszDestFilePath,
                     pszSourceFilePath,
                     MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentFile():Failed to move the file back!!\n  Source - '%s'\n  Dest - '%s'", pszDestFilePath, pszSourceFilePath );
    }

    // Remove the file we created as filler space
    if( !DeleteFile( MEMORY_AREA_FRAG_FILL_FILENAME ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FragmentFile():Failed to delete our filler file!! - '%s', Error - '0x%.8X (%d)'", MEMORY_AREA_FRAG_FILL_FILENAME, GetLastError(), GetLastError() );
    }

    return S_OK;
}


// This function will get the file size of the file, and return it to the caller
DWORD CMemoryArea::GetMAFileSize( char* pszFileName )
{
    if( NULL == pszFileName )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::GetMAFileSize():Invalid parameter passed in!!" );

        return 0xFFFFFFFF;
    }

    DWORD dwFileSize = 0;   // Return value for our function

    // Open the file to get its size
    HANDLE hFile = CreateFile( pszFileName,
                               GENERIC_READ,
                               0,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL );
    if( INVALID_HANDLE_VALUE == hFile )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::GetMAFileSize():Unable to open the file to get its size!! - '%s', Error - '0x%.8X (%d)'", pszFileName, GetLastError(), GetLastError() );

        return 0xFFFFFFFF;
    }
    else
    {
        // Get the size of the file we are dealing with
        dwFileSize = GetFileSize( hFile, NULL );

        CloseHandle( hFile );
        hFile = NULL;

        // If we failed to get the file size, we should exit the function, as we couldn't complete the
        // fragmentation
        if( 0xFFFFFFFF == dwFileSize )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::GetMAFileSize():Failed to obtain the size of our file!! - '%s', Error - '0x%.8X (%d)'", pszFileName, GetLastError(), GetLastError() );
        }
    }

    return dwFileSize;
}


// Will return TRUE if a file exists, otherise FALSE
BOOL CMemoryArea::FileExists( char* pszFileName )
{
    if( NULL == pszFileName )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FileExists():Invalid parameter passed in!!" );

        return FALSE;
    }

    BOOL bReturn = FALSE;

    // Attempt to open the file to see if the file exists
    HANDLE hFile = CreateFile( pszFileName,
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL );

    if( INVALID_HANDLE_VALUE == hFile )
    {
        if( ( ERROR_FILE_NOT_FOUND != GetLastError() ) && ( ERROR_NOT_READY != GetLastError() ) && ( ERROR_PATH_NOT_FOUND != GetLastError() ) && ( ERROR_UNRECOGNIZED_VOLUME != GetLastError() ) )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::FileExists():CreateFile failed!! - '%s', '0x%.8X (%d)'", pszFileName, GetLastError(), GetLastError() );
        }
    }
    else
    {
        bReturn = TRUE;

        CloseHandle( hFile );
        hFile = NULL;
    }

    return bReturn;
}


// Create 0 byte file
DWORD CMemoryArea::CreateZeroByteFile( char* pszFileName )
{
    if( NULL == pszFileName )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateZeroByteFile():Invalid parameter passed in!!" );

        return E_INVALIDARG;
    }

    // Create the file
    HANDLE hFile = CreateFile( pszFileName,
                               GENERIC_WRITE,
                               0,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL );

    if( INVALID_HANDLE_VALUE == hFile )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CCertReqs::Req10_2_01():Failed to create the file!! - '%s', Error - '0x%.8X (%d)'", pszFileName, GetLastError(), GetLastError() );

        return GetLastError();
    }
    else
    {
        CloseHandle( hFile );
        hFile = NULL;
    }

    return S_OK;
}


// Map a drive to a partiticular drive letter
DWORD CMemoryArea::MapDrive( char cDriveLetter, char* pszPartition )
{
    ASSERT( pszPartition != NULL );

    char pszDrive[20];
    ZeroMemory( pszDrive, 20 );

    sprintf( pszDrive, "\\??\\%c:", cDriveLetter );

    ANSI_STRING ostDrive, ostPath;

    RtlInitObjectString( &ostDrive, pszDrive );
    RtlInitObjectString( &ostPath, pszPartition );
    NTSTATUS status = IoCreateSymbolicLink( &ostDrive, &ostPath );

    return RtlNtStatusToDosError( status );
}


// Un-map a drive from the specified drive letter
DWORD CMemoryArea::UnMapDrive( char cDriveLetter )
{
    char pszDrive[20];
    ZeroMemory( pszDrive, 20 );

    sprintf( pszDrive, "\\??\\%c:", cDriveLetter );

    ANSI_STRING ostDrive;

    RtlInitObjectString( &ostDrive, pszDrive );
    NTSTATUS status = IoDeleteSymbolicLink(&ostDrive);

    return RtlNtStatusToDosError( status );
}


// Creates temporary files to use during fragmentations
HRESULT CMemoryArea::CreateFragTempFiles()
{
    //
    // Create a directory that contains our frag files
    //

    // Make sure our directory is cleared out
    DeleteDirectory( MEMORY_AREA_FRAG_FILE_DIR );

    // Use a root-level directory so it's not picked up in our traversals
    if( !CreateDirectory( MEMORY_AREA_FRAG_FILE_DIR, NULL ) )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::CreateFragTempFiles():Failed to create the directory!! - '%hs', Error - '0x%0.8X (%d)'", MEMORY_AREA_FRAG_FILE_DIR, GetLastError(), GetLastError() );
    }

    // Create file entries in the directory
    LARGE_INTEGER liByteSizeOfFiles;

    char pszFileName[MAX_PATH];
    DWORD dwNumBlocksFree = GetFreeBlocks();
    DWORD dwBlocksPerFile = dwNumBlocksFree / MEMORY_AREA_FRAG_NUM_FILES;
    liByteSizeOfFiles.QuadPart = dwBlocksPerFile * MEMORY_AREA_BLOCK_SIZE_IN_BYTES;
    for( unsigned int x = 0; ( x < MEMORY_AREA_FRAG_NUM_FILES ) && ( GetAbortProcess() != TRUE ); x++ )
    {
        ZeroMemory( pszFileName, MAX_PATH );
        _snprintf( pszFileName, MAX_PATH - 1, "%s\\%s%d.xdk", MEMORY_AREA_FRAG_FILE_DIR, MEMORY_AREA_FRAG_FILE_PATTERN, x );

        CreateFileOfSize( pszFileName, liByteSizeOfFiles );
    }

    return S_OK;
}


// Used to set the Default name of the Memory Area
HRESULT CMemoryArea::SetDefaultName( WCHAR* pwszName )
{
    if( NULL == pwszName )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::SetDefaultName():Invalid parameter passed in!!" );

        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    if( m_pwszDefaultName )
    {
        delete[] m_pwszDefaultName;
        m_pwszDefaultName = NULL;
    }

    m_pwszDefaultName = new WCHAR[wcslen( pwszName ) + 1];
    if( NULL != m_pwszDefaultName )
    {
        wcscpy( m_pwszDefaultName, pwszName );
    }
    else
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::SetDefaultName():Failed to allocate memory!!" );

        hr = E_FAIL;
    }

    return hr;
}


// Used to get the Memory Area default name
HRESULT CMemoryArea::GetDefaultName( WCHAR* pwszBuffer, ULONG ulBufferLen )
{
    if( NULL == pwszBuffer )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::GetDefaultName():Invalid parameter passed in!!" );

        ZeroMemory( pwszBuffer, sizeof( WCHAR ) * ulBufferLen );

        return E_INVALIDARG;
    }

    if( NULL == m_pwszDefaultName )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::GetDefaultName():Default name was never set!!" );

        ZeroMemory( pwszBuffer, sizeof( WCHAR ) * ulBufferLen );

        return E_FAIL;
    }

    ASSERT( ulBufferLen >= ( wcslen( m_pwszDefaultName ) + 1 ) );

    ZeroMemory( pwszBuffer, sizeof( WCHAR ) * ulBufferLen );
    wcscpy( pwszBuffer, m_pwszDefaultName );

    return S_OK;
}


// Will search the memory area and populate the GameTitles object
void CMemoryArea::ProcessGameTitles()
{
    if( '\0' == GetUDataDrive() )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CMemoryArea::ProcessGameTitles():Can't process titles for Memory Area -- No UData Drive!!" );

        return;
    }

    // Get the Game Title Info for this MU
    char pszDir[3];
    pszDir[2] = '\0';
    sprintf( pszDir, "%c:", GetUDataDrive() );
    m_GameTitles.Init( pszDir, this );
}


// Used to set whether or not the current process should be aborted
void CMemoryArea::SetAbortProcess( BOOL bSetting )
{
    EnterCriticalSection( &m_ProcessCritSec );
    
    m_bAbortProcess = bSetting;
    
    LeaveCriticalSection( &m_ProcessCritSec );
}


// Used to get the state of our current process
BOOL CMemoryArea::GetAbortProcess()
{
    BOOL bReturnVal;
    
    EnterCriticalSection( &m_ProcessCritSec );
    
    bReturnVal = m_bAbortProcess;
    
    LeaveCriticalSection( &m_ProcessCritSec );

    return bReturnVal;
}
