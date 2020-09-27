/*****************************************************
*** xbelist.cpp
***
*** CPP file for our XBE List class.
*** This class will hold a linked list of XBE's that
*** will be populated from a directory.
***
*** by James N. Helm
*** December 2nd , 2000
***
*****************************************************/

#include "stdafx.h"
#include "xbelist.h"

extern CHardDrive   g_XboxHardDrive;    // Used to configure the Xbox Hard Drive

// Constructor
CXBEList::CXBEList()
: m_bInitialized( FALSE )
{
    m_XBEItems.SetDeleteDataItem( TRUE );
}

// Destructor
CXBEList::~CXBEList()
{
}


// Will parse an XBE file and return the Title Name and Publisher Name
// If the XBE is valid, bValidXBE will be TRUE, otherwise FALSE
HRESULT CXBEList::GetInfoFromXBE( struct _XBEINFO* currentXBE, BOOL& bValidXBE, char* pszDrive )
{
    if( ( NULL == currentXBE ) || ( NULL == pszDrive ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBEList::GetInfoFromXBE():Invalid arg passed in!!" );

        return E_INVALIDARG;
    }

    bValidXBE = FALSE;                      // Set the initial return BOOLean to 'FALSE' -- only set it to TRUE if we end up with a valid XBE
    PVOID pVarImageHeader = NULL;           // Pointer to our variable image header.
    DWORD dwCertLocationOffset = 0;         // Offset in the file where the CERT information lives
    int numRead = 0;                        // Number of 'pages' read using fread
    HRESULT hr = S_OK;                      // Return Code
    FILE* pXBEFile = NULL;                  // Used to open the XBE file
    char pszfileNameBuffer[MAX_PATH + 1];   // Used to hold the file name
    XBEIMAGE_HEADER InitialXBEHeader;       // Will hold the initial header of our XBE file

    // Clear our buffers our buffers
    ZeroMemory( pszfileNameBuffer, MAX_PATH + 1 );

    _snprintf( pszfileNameBuffer, MAX_PATH, "%s%s\\%s%s", pszDrive, currentXBE->directory, currentXBE->filename, FILE_EXECUTABLE_EXENSION_A );

    pXBEFile = fopen( pszfileNameBuffer, "rbR" );
    if( NULL == pXBEFile )
    {
        // Couldn't open the XBE file, return "FALSE" as to the XBE validity
        XDBGWRN( APP_TITLE_NAME_A, "CXBEList::GetInfoFromXBE():Failed to open the file!! - '%s'", pszfileNameBuffer );

        hr = S_OK;
        goto cleanup;
    }

    numRead = fread( &InitialXBEHeader, sizeof( XBEIMAGE_HEADER ), 1, pXBEFile );
    if( 1 != numRead )
    {
        // Header wasn't big enough to be a valid XBE file, return "FALSE" as to the XBE validity
        XDBGWRN( APP_TITLE_NAME_A, "CXBEList::GetInfoFromXBE():Header wasn't big enough - expected 1, got - '%d'", numRead );

        // Clean up and return
        hr = S_OK;
        goto cleanup;
    }

    if( InitialXBEHeader.Signature != XBEIMAGE_SIGNATURE )
    {
        // Signature didn't match, return "false" as to the XBE validity
        XDBGWRN( APP_TITLE_NAME_A, "CXBEList::GetInfoFromXBE():Invalid XBE signature" );

        // Clean up and return
        hr = S_OK;
        goto cleanup;
    }

    // Calculate the offset in the file where the CERT size is located
    dwCertLocationOffset = (ULONG)InitialXBEHeader.Certificate - (ULONG)InitialXBEHeader.BaseAddress;

    // Seek to that location in the file and read in the XBOX CERT
    if( 0 != fseek( pXBEFile, dwCertLocationOffset, SEEK_SET ) )
    {
        // Return FALSE
        XDBGWRN( APP_TITLE_NAME_A, "CXBEList::GetInfoFromXBE():Could not fseek to proper file location!! Tried - '%d'", dwCertLocationOffset );

        goto cleanup;
    }

    // Read in the information in to our XCERT
    XBEIMAGE_CERTIFICATE XBECert;
    numRead = fread( &XBECert, sizeof( XBEIMAGE_CERTIFICATE ), 1, pXBEFile );
    if( 1 != numRead )
    {
        // Return FALSE
        XDBGWRN( APP_TITLE_NAME_A, "CXBEList::GetInfoFromXBE():Could not read the CERT from the file!! - expected 1, got - '%d'", numRead );

        goto cleanup;
    }

    // Now that we have the cert, let's copy the Title Information and Publisher information in to our structure
    char titleName[XBEIMAGE_TITLE_NAME_LENGTH + 1];

    _snwprintf( currentXBE->titleName, XBEIMAGE_TITLE_NAME_LENGTH, L"%s", XBECert.TitleName );

    bValidXBE = TRUE;

cleanup:

    if( NULL != pXBEFile )
    {
        fclose( pXBEFile );
    }

    return hr;
}


// Initialize our list
//
// dirname - the name of the directory to parse
// fileExt - the extension of the file to use
HRESULT CXBEList::Init( char* dirname, char* fileExt )
{
    HRESULT hr = S_OK;

    if( m_bInitialized )
    {
        // Ensure our list is empty
        hr = m_XBEItems.EmptyList();
    }

    if( SUCCEEDED( hr ) )
    {
        m_bInitialized = FALSE;
        hr = LoadXBEs( dirname, fileExt );

        // Check for the Dashboard on the Y partition
        if( g_XboxHardDrive.FileExists( FILE_DATA_DASHBOARD_LOCATION_A ) )
        {
            HRESULT hrFileStatus = ProcessFile( FILE_DATA_DASHBOARD_FILENAME_A, FILE_DATA_DASHBOARD_DRIVE_A, FILE_DATA_DASHBOARD_DIRECTORY_A, fileExt );
            if( FAILED( hrFileStatus ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CXBEList::Init():Failed to process the file!! File - '%hs', Dir - '%hs', Error - '0x%.8X (%d)'", FILE_DATA_DASHBOARD_FILENAME_A, FILE_DATA_DASHBOARD_DIRECTORY_A, hrFileStatus, hrFileStatus );
            }
        }

        if( SUCCEEDED( hr ) )
        {
            m_bInitialized = TRUE;
        }
        else // Clean up our list if we are in a bad way
        {
            m_XBEItems.EmptyList();
        }
    }

    return hr;
}


// Load the XBEs in to our Linked List
HRESULT CXBEList::LoadXBEs( char* dirname, char* fileExt )
{
    // Ensure we were passed valid parameters
    if( ( NULL == dirname ) || ( NULL == fileExt ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBEList::LoadXBEs():Invalid arguments passed in!!" );

        return E_INVALIDARG;
    }

    char searchPath[MAX_PATH];
    char nextPath[MAX_PATH];
    _snprintf( searchPath, MAX_PATH, "%s\\*.*", dirname );

    WIN32_FIND_DATA wfd;
    HANDLE hFind;

    // Find the first file in the directory
    hFind = FindFirstFile( searchPath, &wfd );

    // If FindFirstFile failed with an error, return E_FAIL
    if( INVALID_HANDLE_VALUE == hFind )
    {
        return E_FAIL;
    }

    // Parse the directories looking for the correct file types
    do
    {
        // Skip over files that we are not interested in
		if ( wfd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM )
		{
			continue;
		}

		if ( wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )
		{
			continue;
		}

		if ( wfd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY )
		{
			continue;
		}

		if ( '.' == wfd.cFileName[0] )
		{
			continue;
		}

        // If we found a directory, recursively call this function
        if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            _snprintf( nextPath, MAX_PATH, "%s\\%s", dirname, wfd.cFileName );
            LoadXBEs( nextPath, fileExt );
        }
        else // Found a file, check for XBE
        {
            HRESULT hrFileStatus = ProcessFile( wfd.cFileName, FILE_EXECUTABLE_DRIVE_A, dirname, fileExt );
            if( FAILED( hrFileStatus ) )
            {
                XDBGWRN( APP_TITLE_NAME_A, "CXBEList::LoadXBEs():Failed to process the file!! File - '%hs', Dir - '%hs', Error - '0x%.8X (%d)'", wfd.cFileName, dirname, hrFileStatus, hrFileStatus );
            }
        }
    } while( FindNextFile( hFind, &wfd ) );

// cleanup:
    if( !FindClose( hFind ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBEList::LoadXBEs():FindClose failed!! - (%d)", GetLastError() );
    }

    return S_OK;
}


// Process the file and add it to our XBE list if it's valid
HRESULT CXBEList::ProcessFile( char* pszFileName, char* pszDrive, char* pszDirectory, char* pszExtension )
{
    if( ( NULL == pszFileName ) || ( NULL == pszDirectory ) || ( NULL == pszExtension ) || ( NULL == pszDrive ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBEList::ProcessFile():Invalid parameter passed in!!" );

        return E_INVALIDARG;
    }

    // Match the extension using the upcased version of the file name

    // Convert the filename to uppercase
    _strupr( pszFileName );

    if( NULL != strstr( pszFileName, pszExtension ) )
    {
        struct _XBEINFO* pXBEInfo = new struct _XBEINFO;
        if( NULL == pXBEInfo )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CXBEList::ProcessFile():Failed to allocate memory!!" );

            return E_OUTOFMEMORY;
        }

        ZeroMemory( pXBEInfo, sizeof( struct _XBEINFO ) );

        _snprintf( pXBEInfo->directory, MAX_PATH, pszDirectory + 2 );


        unsigned int uiStrLen = strlen( pszFileName ) - strlen( pszExtension );
        strncpy( pXBEInfo->filename, pszFileName, uiStrLen );
        pXBEInfo->filename[uiStrLen] = '\0';

        XDBGTRC( APP_TITLE_NAME_A, "CXBEList::ProcessFile():Dir - '%hs', File - '%hs'", pXBEInfo->directory, pXBEInfo->filename );

        // Determine if it's a valid XBE, get the title and publisher info
        BOOL bValidXBE;
        if( FAILED( GetInfoFromXBE( pXBEInfo, bValidXBE, pszDrive ) ) )
        {
            XDBGWRN( APP_TITLE_NAME_A, "CXBEList::ProcessFile(): Get Info Failed!!" );

            delete pXBEInfo;
            pXBEInfo = NULL;
        }
        else if( !bValidXBE )
        {
            XDBGTRC( APP_TITLE_NAME_A, "CXBEList::ProcessFile():Invalid XBE!!" );

            delete pXBEInfo;
            pXBEInfo = NULL;
        }
        else
        {
            m_XBEItems.AddNode( pXBEInfo );
        }
    }

    return S_OK;
}