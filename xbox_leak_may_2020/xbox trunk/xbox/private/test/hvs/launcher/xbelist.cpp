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

// Constructor
CXBEList::CXBEList( void )
: m_bInitialized( FALSE )
{
    m_XBEItems.SetDeleteDataItem( TRUE );
    m_XBEItems.SetSortedList( TRUE );
}

// Destructor
CXBEList::~CXBEList( void )
{
}


// Will parse an XBE file and return the Title Name and Publisher Name
// If the XBE is valid, bValidXBE will be TRUE, otherwise FALSE
HRESULT CXBEList::GetInfoFromXBE( CXBEInfo* currentXBE, BOOL& bValidXBE, char* pszDrive )
{
    if( ( NULL == currentXBE ) || ( NULL == pszDrive ) )
    {
        DebugPrint( "CXBEList::GetInfoFromXBE():Invalid arg passed in!!\n" );

        return E_INVALIDARG;
    }

    bValidXBE = FALSE;                      // Set the initial return BOOLean to 'FALSE' -- only set it to TRUE if we end up with a valid XBE
    DWORD dwCertLocationOffset = 0;         // Offset in the file where the CERT information lives
    int numRead = 0;                        // Number of 'pages' read using fread
    HRESULT hr = S_OK;                      // Return Code
    FILE* pXBEFile = NULL;                  // Used to open the XBE file
    char pszfileNameBuffer[MAX_PATH + 1];   // Used to hold the file name
    XBEIMAGE_HEADER InitialXBEHeader;       // Will hold the initial header of our XBE file

    // Clear our buffers
    ZeroMemory( pszfileNameBuffer, MAX_PATH + 1 );

    _snprintf( pszfileNameBuffer, MAX_PATH, "%s%s\\%s.%s", pszDrive, currentXBE->GetDirectory(), currentXBE->GetFilename(), currentXBE->GetFileExtension() );

    pXBEFile = fopen( pszfileNameBuffer, "rbR" );
    if( NULL == pXBEFile )
    {
        // Couldn't open the XBE file, return "FALSE" as to the XBE validity
        DebugPrint( "CXBEList::GetInfoFromXBE():Failed to open the file!! - '%s'\n", pszfileNameBuffer );

        hr = S_OK;
        goto cleanup;
    }

    numRead = fread( &InitialXBEHeader, sizeof( XBEIMAGE_HEADER ), 1, pXBEFile );
    if( 1 != numRead )
    {
        // Header wasn't big enough to be a valid XBE file, return "FALSE" as to the XBE validity
        DebugPrint( "CXBEList::GetInfoFromXBE():Header wasn't big enough - expected 1, got - '%d'\n", numRead );

        // Clean up and return
        hr = S_OK;
        goto cleanup;
    }

    if( InitialXBEHeader.Signature != XBEIMAGE_SIGNATURE )
    {
        // Signature didn't match, return "false" as to the XBE validity
        DebugPrint( "CXBEList::GetInfoFromXBE():Invalid XBE signature\n" );

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
        DebugPrint( "CXBEList::GetInfoFromXBE():Could not fseek to proper file location!! Tried - '%d'\n", dwCertLocationOffset );

        goto cleanup;
    }

    // Read in the information in to our XCERT
    XBEIMAGE_CERTIFICATE XBECert;
    numRead = fread( &XBECert, sizeof( XBEIMAGE_CERTIFICATE ), 1, pXBEFile );
    if( 1 != numRead )
    {
        // Return FALSE
        DebugPrint( "CXBEList::GetInfoFromXBE():Could not read the CERT from the file!! - expected 1, got - '%d'\n", numRead );

        goto cleanup;
    }

    _snwprintf( currentXBE->GetTitleName(), XBEIMAGE_TITLE_NAME_LENGTH, L"%s", XBECert.TitleName );

    bValidXBE = TRUE;

cleanup:

    if( NULL != pXBEFile )
        fclose( pXBEFile );

    return hr;
}


// Initialize our list
//
// dirname - the name of the directory to parse
// fileExt - the extension of the file to use
HRESULT CXBEList::Init( char* pszDirname, char* pszFileExt )
{
    HRESULT hr = S_OK;

    if( m_bInitialized )
        hr = m_XBEItems.EmptyList();    // Ensure our list is empty

    if( SUCCEEDED( hr ) )
    {
        m_bInitialized = FALSE;
        hr = LoadXBEs( pszDirname, pszFileExt );

        if( SUCCEEDED( hr ) )
            m_bInitialized = TRUE;
        else // Clean up our list if we are in a bad way
            m_XBEItems.EmptyList();
    }

    return hr;
}


// Load the XBEs in to our Linked List
HRESULT CXBEList::LoadXBEs( char* pszDirname, char* pszFileExt )
{
    // Ensure we were passed valid parameters
    if( ( NULL == pszDirname ) || ( NULL == pszFileExt ) )
    {
        DebugPrint( "CXBEList::LoadXBEs():Invalid arguments passed in!!\n" );

        return E_INVALIDARG;
    }

    char pszSearchPath[MAX_PATH];
    _snprintf( pszSearchPath, MAX_PATH, "%s\\*.%s", pszDirname, pszFileExt );

    WIN32_FIND_DATA wfd;
    HANDLE hFind;

    // Find the first file in the directory
    hFind = FindFirstFile( pszSearchPath, &wfd );

    // If FindFirstFile failed with an error, return E_FAIL
    if( INVALID_HANDLE_VALUE == hFind )
        return E_FAIL;

    // Parse the directories looking for the correct file types
    do
    {
        // Skip over files that we are not interested in
		if ( wfd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM )
			continue;

		if ( wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )
			continue;

		if ( wfd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY )
			continue;

        if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            continue;

		if ( '.' == wfd.cFileName[0] )
			continue;

        HRESULT hrFileStatus = ProcessFile( wfd.cFileName, "d:", pszDirname, pszFileExt );
        if( FAILED( hrFileStatus ) )
            DebugPrint( "CXBEList::LoadXBEs():Failed to process the file!! File - '%hs', Dir - '%hs', Error - '0x%.8X (%d)'\n", wfd.cFileName, pszDirname, hrFileStatus, hrFileStatus );

    } while( FindNextFile( hFind, &wfd ) );

// cleanup:
    if( !FindClose( hFind ) )
        DebugPrint( "CXBEList::LoadXBEs():FindClose failed!! - (%d)\n", GetLastError() );

    return S_OK;
}


// Process the file and add it to our XBE list if it's valid
HRESULT CXBEList::ProcessFile( char* pszFileName, char* pszDrive, char* pszDirectory, char* pszExtension )
{
    if( ( NULL == pszFileName ) || ( NULL == pszDirectory ) || ( NULL == pszExtension ) || ( NULL == pszDrive ) )
    {
        DebugPrint( "CXBEList::ProcessFile():Invalid parameter passed in!!\n" );

        return E_INVALIDARG;
    }

    // Process files that only END in .XBE
    char* pszLocExtention = pszFileName + ( strlen( pszFileName ) - 3 );

    // Match the extension using the upcased version of the file name
    if( 0 == _stricmp( pszLocExtention, pszExtension ) )
    {
        CXBEInfo* pXBEInfo = new CXBEInfo;
        if( NULL == pXBEInfo )
        {
            DebugPrint( "CXBEList::ProcessFile():Failed to allocate memory!!\n" );

            return E_OUTOFMEMORY;
        }

        pXBEInfo->SetDirectory( pszDirectory + 2 );

        unsigned int uiStrLen = strlen( pszFileName ) - strlen( pszExtension ) - 1;
        char* pszNewFileName = new char[uiStrLen + 1];
        if( NULL == pszNewFileName )
        {
            DebugPrint( "CXBEList::ProcessFile():Unable to allocate memory!!\n" );

            delete pXBEInfo;
            pXBEInfo = NULL;

            return E_OUTOFMEMORY;
        }
        pszNewFileName[uiStrLen] = '\0';
        strncpy( pszNewFileName, pszFileName, uiStrLen );
        pXBEInfo->SetFilename( pszNewFileName );

        delete[] pszNewFileName;
        pszNewFileName = NULL;

        pXBEInfo->SetFileExtension( pszLocExtention );

        // DebugPrint( "CXBEList::ProcessFile():Dir - '%hs', File - '%hs'\n", pXBEInfo->GetDirectory(), pXBEInfo->GetFilename() );

        // Determine if it's a valid XBE, get the title and publisher info
        BOOL bValidXBE;
        if( FAILED( GetInfoFromXBE( pXBEInfo, bValidXBE, pszDrive ) ) )
        {
            DebugPrint( "CXBEList::ProcessFile(): Get Info Failed!!\n" );

            delete pXBEInfo;
            pXBEInfo = NULL;
        }
        else if( !bValidXBE )
        {
            DebugPrint( "CXBEList::ProcessFile():Invalid XBE!!\n" );

            delete pXBEInfo;
            pXBEInfo = NULL;
        }
        else
		{
			// Process Configuration File
            if( FAILED( ProcessConfigFile( pXBEInfo ) ) )
            {
                DebugPrint( "CXBEList::ProcessFile():ProcessConfigFile failed!!\n" );

                delete pXBEInfo;
                pXBEInfo = NULL;
            }
            else
            {
                // Add a configuration based off of the default configuration
                CTestConfig* pTestConfig = new CTestConfig;
                if( NULL == pTestConfig )
                {
                    DebugPrint( "CXBEList::ProcessFile():Failed to allocate memory!!\n");

                    delete pXBEInfo;
                    pXBEInfo = NULL;
                    
                    return E_FAIL;
                }

                *pTestConfig = *(pXBEInfo->GetDefaultTestConfig());

                // Add the new test config to our XBE
                pXBEInfo->AddTestConfig( pTestConfig );

                // Add the XBE to our list
                m_XBEItems.AddNode( pXBEInfo );
            }
		}
    }

    return S_OK;
}


// Find the index of an XBE by filename
unsigned int CXBEList::GetIndexOfXBE( char* pszFilename )
{
    if( NULL == pszFilename )
    {
        DebugPrint( "CXBEList::GetIndexOfXBE():Invalid argument(s) passed in!!\n" );

        return 9999;
    }

    BOOL bFound = FALSE;
    unsigned int uiReturnIndex = 0;
    MoveFirst();
    do
    {
        if( 0 == _stricmp( pszFilename, GetCurrentXBEInfoPtr()->GetFilename() ) )
        {
            bFound = TRUE;

            break;
        }

        uiReturnIndex++;
    } while( MoveNext() );

    if( !bFound )
        uiReturnIndex = 9999;

    return uiReturnIndex;
}
