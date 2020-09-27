/*****************************************************
*** harddrive.cpp
***
*** CPP file for our Hard Drive class.
*** This class will contain functions and information
*** pertaining to the Xbox Hard Drive
***
*** by James N. Helm
*** April 2nd, 2001
***
*****************************************************/

#include "memarpch.h"
#include "harddrive.h"

// Contructors
CHardDrive::CHardDrive()
: CMemoryArea()
{
    SetTDataDrive( HARDDRIVE_TDATA_DRIVE_LETTER_A );
    SetUDataDrive( HARDDRIVE_UDATA_DRIVE_LETTER_A );

    MapDrive( GetTDataDrive(), HARDDRIVE_TDATA_PATH_A );
    MapDrive( GetUDataDrive(), HARDDRIVE_UDATA_PATH_A );

    SetDefaultName( HARDDRIVE_NAME_PATTERN );
}


// Destructor
CHardDrive::~CHardDrive()
{
    UnMapDrive( GetTDataDrive() );
    UnMapDrive( GetUDataDrive() );
}


// Map the TDATA drive to it's orignal location
HRESULT CHardDrive::MapTDataToOrigLoc()
{
    UnMapDrive( GetTDataDrive() );
    MapDrive( GetTDataDrive(), HARDDRIVE_TDATA_PATH_A );

    return S_OK;
}


// Map the TDATA drive letter to the specified Title ID
HRESULT CHardDrive::MapTDataToTitleID( char* pszTitleID )
{
    if( NULL == pszTitleID )
    {
        return E_INVALIDARG;
    }

    // Make sure the Title Directory has been created
    char pszBuffer[MAX_PATH+1];
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%c:\\%hs", GetTDataDrive(), pszTitleID );

    // We don't care if this fails
    CreateDirectory( pszBuffer, NULL );

    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%hs\\%hs", HARDDRIVE_TDATA_PATH_A, pszTitleID );

    // Map the TDATA Drive to the new Title ID Directory
    UnMapDrive( 'T' );

    HRESULT hr = S_OK;
    if( !NT_SUCCESS( MapDrive( 'T', pszBuffer ) ) )
    {
        hr = E_FAIL;
    }

    return hr;
}


// Map the TDATA drive letter to the specified Title ID
HRESULT CHardDrive::MapTDataToTitleID( WCHAR* pwszTitleID )
{
    if( NULL == pwszTitleID )
    {
        return E_INVALIDARG;
    }

    // Make sure the Title Directory has been created
    char pszBuffer[MAX_PATH+1];
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%c:\\%ls", GetTDataDrive(), pwszTitleID );

    // We don't care if this fails
    CreateDirectory( pszBuffer, NULL );

    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%hs\\%ls", HARDDRIVE_TDATA_PATH_A, pwszTitleID );

    // Map the TDATA Drive to the new Title ID Directory
    UnMapDrive( 'T' );

    HRESULT hr = S_OK;
    if( !NT_SUCCESS( MapDrive( 'T', pszBuffer ) ) )
    {
        hr = E_FAIL;
    }

    return hr;
}


// Map the TDATA drive letter to the specified Title ID
HRESULT CHardDrive::MapTDataToTitleID( DWORD dwTitleID )
{
    // Make sure the Title Directory has been created
    char pszBuffer[MAX_PATH+1];
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%c:\\%.8X", GetTDataDrive(), dwTitleID );

    // We don't care if this fails
    CreateDirectory( pszBuffer, NULL );

    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%hs\\%.8X", HARDDRIVE_TDATA_PATH_A, dwTitleID );

    // Map the TDATA Drive to the new Title ID Directory
    UnMapDrive( 'T' );

    HRESULT hr = S_OK;
    if( !NT_SUCCESS( MapDrive( 'T', pszBuffer ) ) )
    {
        hr = E_FAIL;
    }

    return hr;
}


// Map the UDATA drive letter to the specified Title ID
HRESULT CHardDrive::MapUDataToTitleID( char* pszTitleID )
{
    if( NULL == pszTitleID )
    {
        return E_INVALIDARG;
    }

    // Make sure the Title Directory has been created
    char pszBuffer[MAX_PATH+1];
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%c:\\%hs", GetUDataDrive(), pszTitleID );

    // We don't care if this fails
    CreateDirectory( pszBuffer, NULL );

    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%hs\\%hs", HARDDRIVE_UDATA_PATH_A, pszTitleID );

    // Map the UDATA Drive to the new Title ID Directory
    UnMapDrive( 'U' );

    HRESULT hr = S_OK;
    if( !NT_SUCCESS( MapDrive( 'U', pszBuffer ) ) )
    {
        hr = E_FAIL;
    }

    return hr;
}


// Map the UDATA drive letter to the specified Title ID
HRESULT CHardDrive::MapUDataToTitleID( WCHAR* pwszTitleID )
{
    if( NULL == pwszTitleID )
    {
        return E_INVALIDARG;
    }

    // Make sure the Title Directory has been created
    char pszBuffer[MAX_PATH+1];
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%c:\\%ls", GetUDataDrive(), pwszTitleID );

    // We don't care if this fails
    CreateDirectory( pszBuffer, NULL );

    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%hs\\%ls", HARDDRIVE_UDATA_PATH_A, pwszTitleID );

    // Map the UDATA Drive to the new Title ID Directory
    UnMapDrive( 'U' );

    HRESULT hr = S_OK;
    if( !NT_SUCCESS( MapDrive( 'U', pszBuffer ) ) )
    {
        hr = E_FAIL;
    }

    return hr;
}


// Create a Saved Game
HRESULT CHardDrive::CreateSavedGame( IN WCHAR* pwszSavedGameName,
                                     OUT char* pszGamePath,
                                     IN unsigned int cbBuffSize,
                                     IN SYSTEMTIME* pSysTimeGameDate,
                                     IN char* pszSaveImage,
                                     IN BOOL bNoCopyGame )
{
    char cDrive = GetUDataDrive();
    SetUDataDrive( 'U' );
    HRESULT hr = CMemoryArea::CreateSavedGame( pwszSavedGameName,
                                               pszGamePath,
                                               cbBuffSize,
                                               pSysTimeGameDate,
                                               pszSaveImage,
                                               bNoCopyGame );
    SetUDataDrive( cDrive );

    return hr;
}


// Map the UDATA drive to it's orignal location
HRESULT CHardDrive::MapUDataToOrigLoc()
{
    UnMapDrive( GetUDataDrive() );
    MapDrive( GetUDataDrive(), HARDDRIVE_UDATA_PATH_A );

    return S_OK;
}


// Map the UDATA drive letter to the specified Title ID
HRESULT CHardDrive::MapUDataToTitleID( DWORD dwTitleID )
{
    // Make sure the Title Directory has been created
    char pszBuffer[MAX_PATH+1];
    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%c:\\%.8X", GetUDataDrive(), dwTitleID );

    // We don't care if this fails
    CreateDirectory( pszBuffer, NULL );

    ZeroMemory( pszBuffer, MAX_PATH+1 );
    _snprintf( pszBuffer, MAX_PATH, "%hs\\%.8X", HARDDRIVE_UDATA_PATH_A, dwTitleID );

    // Map the UDATA Drive to the new Title ID Directory
    UnMapDrive( 'U' );

    HRESULT hr = S_OK;
    if( !NT_SUCCESS( MapDrive( 'U', pszBuffer ) ) )
    {
        hr = E_FAIL;
    }

    return hr;
}


// Remove all of the Soundtracks that live on the hard drive
HRESULT CHardDrive::RemoveAllSoundtracks()
{
    HRESULT hr = S_OK;

    BOOL bReturn = DashBeginUsingSoundtracks();
    if( !bReturn )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CHardDrive::RemoveAllSoundtracks():Failed to open the ST DB!! Error - '0x%.8X'", GetLastError() );

        return E_FAIL;
    }

    // Get the number of soundtracks from the DB
    DWORD dwNumSTs = DashGetSoundtrackCount();
    XDBGTRC( MEMAREA_APP_TITLE_NAME_A, "CHardDrive::RemoveAllSoundtracks():Number of Soundtracks - '%d'", dwNumSTs );

    // Check to see if there are any soundtracks, if not, clean up and leave
    if( 0 == dwNumSTs )
    {
        DashEndUsingSoundtracks();

        return hr;
    }

    // Create an array of ST IDs so we can delete them
    DWORD* pdwSTIDs = new DWORD[dwNumSTs];
    if( !pdwSTIDs )
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CHardDrive::RemoveAllSoundtracks():Failed to allocate memory for our ST IDs!!" );
    }

    // Must call this before we use the XFindFirst and XFindNext Soundtrack functions
    DashEndUsingSoundtracks();

    unsigned int uiCounter = 0; // Used to track the current index of our ST IDs
    XSOUNDTRACK_DATA stData;
    ZeroMemory( &stData, sizeof( XSOUNDTRACK_DATA ) );

    HANDLE hSTHandle = XFindFirstSoundtrack( &stData );
    if( INVALID_HANDLE_VALUE != hSTHandle )
    {
        // Enumerate through the soundtracks and store the ID info
        pdwSTIDs[uiCounter++] = stData.uSoundtrackId;

        // Find the rest of the soundtracks
        while( XFindNextSoundtrack( hSTHandle, &stData ) )
        {
            pdwSTIDs[uiCounter++] = stData.uSoundtrackId;
        }

        // Close our handle to the soundtracks
        XFindClose( hSTHandle );
    }

    // Reopen the Soundtrack Database so the Dashboard can use it
    bReturn = DashBeginUsingSoundtracks();
    if( !bReturn )
    //
    // We failed to open the ST DB, do NOT try to remove any of them
    //
    {
        XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CHardDrive::RemoveAllSoundtracks():Failed to open the ST DB!! Error - '0x%.8X'", GetLastError() );

        hr = E_FAIL;
    }
    else
    //
    // We opened the ST DB successfully, let's remove the Soundtracks
    //
    {
        // Remove the soundtracks
        for( unsigned int x = 0; x < dwNumSTs; x++ )
        {
            if( !DashDeleteSoundtrack( pdwSTIDs[x] ) )
            {
                XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CHardDrive::RemoveAllSoundtracks():Failed to delete soundtrack - '%d'!!", pdwSTIDs[x] );
                XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CHardDrive::RemoveAllSoundtracks():Error - '%d, 0x%.8X'", GetLastError(), GetLastError() );
            }
        }
    }


    ///////////
    // Clean up
    ///////////
    if( pdwSTIDs )
    {
        delete[] pdwSTIDs;
        pdwSTIDs = NULL;
    }

    DashEndUsingSoundtracks();

    return hr;
}


// Remove all of the Nicknames that live on the hard drive
HRESULT CHardDrive::RemoveAllNicknames()
{
    HRESULT hr = S_OK;

    char pszFullNickPath[MAX_PATH];
    ZeroMemory( pszFullNickPath, MAX_PATH );

    sprintf( pszFullNickPath, "%c:\\%s", HARDDRIVE_UDATA_DRIVE_LETTER_A, HARDDRIVE_NICKNAME_FILENAME );

    if( !DeleteFile( pszFullNickPath ) )
    {
        // Check to make sure the file existed.  If it did not, then there really is no error
        if( ERROR_FILE_NOT_FOUND != GetLastError() )
        {
            XDBGWRN( MEMAREA_APP_TITLE_NAME_A, "CHardDrive::RemoveAllNicknames():Failed to remove the nicknames!! - '0x%.8X (%d)'!!", GetLastError(), GetLastError() );

            hr = E_FAIL;
        }
    }

    return hr;
}