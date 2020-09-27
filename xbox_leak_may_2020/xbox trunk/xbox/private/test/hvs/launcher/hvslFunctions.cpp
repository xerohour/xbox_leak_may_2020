#include "stdafx.h"
#include "hvslFunctions.h"

static XFONT*               g_pHeaderFont = NULL;       // Header font used by all menus
static XFONT*               g_pBodyFont = NULL;         // Body font used by all menus
static XFONT*               g_pButtonFont = NULL;       // Button font used by all menus

static CHelpMenu            g_HelpMenu;                 // Help menu (will display help for each menu)
static CKeypadMenu          g_KeypadMenu;               // Virtual Keypad (Can be used by any Menu)                            
static CKeyboardMenu        g_KeyboardMenu;             // Virtual Keyboard (Can be used by any Menu)
static CChooseSaveMenu      g_ChooseSaveMenu;           // Allows the user to select a Saved Game
static CMemAreaSelectMenu   g_MemAreaSelectMenu;        // Allows the user to select a Saved Game
static CMessageMenu         g_MessageMenu;              // Present a message to the user
static CYesNoMenu           g_YesNoMenu;                // Ask the user a YES / NO question

static CXItem*              g_pCurrentScreen = NULL;    // Pointer to the current menu object
static CXItem*              g_pPrevScreen = NULL;	    // Pointer to the previous menu object
                            
static CMemoryUnit          g_MemoryUnits[8];           // Memory Units that can be inserted, removed, etc
static DWORD                g_dwInsertedMUs;            // Tracks which MUs are inserted or removed
static BOOL                 g_bMUsChanged = TRUE;       // Tracks whether MU's have been inserted or removed

static CXBEInfo             g_LauncherXBEInfo;          // XBE Information for our test launcher


// Initialize our Menus
void InitializeMenus( void )
{
    g_HelpMenu.Init( GetBodyFont(), GetHeaderFont() );
    g_KeypadMenu.Init( GetBodyFont(), GetHeaderFont() );
    g_KeyboardMenu.Init( GetBodyFont(), GetHeaderFont() );
    g_ChooseSaveMenu.Init( GetBodyFont(), GetHeaderFont() );
    g_MemAreaSelectMenu.Init( GetBodyFont(), GetHeaderFont() );
    g_MessageMenu.Init( GetBodyFont(), GetHeaderFont() );
    g_YesNoMenu.Init( GetBodyFont(), GetHeaderFont() );
}


// Return a pointer to the YES / NO Menu
CYesNoMenu* GetYesNoPtr( void )
{
    return &g_YesNoMenu;
}


// Return a pointer to the Help Menu
CHelpMenu* GetHelpMenuPtr( void )
{
    return &g_HelpMenu;
}


// Return a pointer to the Message Menu
CMessageMenu* GetMessagePtr( void )
{
    return &g_MessageMenu;
}


// Return a pointer to the Memory Area Select Menu
CMemAreaSelectMenu* GetMemAreaSelectPtr( void )
{
    return &g_MemAreaSelectMenu;
}

// Return a pointer to the Choose Saved Game Menu
CChooseSaveMenu* GetChooseSavePtr( void )
{
	return &g_ChooseSaveMenu;
}

// Return a pointer to the Virtual Keypad
CKeypadMenu* GetKeypadPtr( void )
{
	return &g_KeypadMenu;
}

// Return a pointer to the Virtual Keyboard
CKeyboardMenu* GetKeyboardPtr( void )
{
	return &g_KeyboardMenu;
}

// Initialize our fonts
void InitializeFonts( void )
{
    ////////////////////////////////////////
    // Setup and Initialize our Default FONT
    ////////////////////////////////////////
    if( FAILED( OpenTTFontFile( g_pHeaderFont, gc_pwszFONT_HEADER_FILENAME ) ) )
        DebugPrint( "Init():Failed to open the header font!! - '%ls'\n", gc_pwszFONT_HEADER_FILENAME );

    if( FAILED( OpenTTFontFile( g_pBodyFont, gc_pwszFONT_BODY_FILENAME ) ) )
        DebugPrint( "Init():Failed to open the body font!! - '%ls'\n", gc_pwszFONT_BODY_FILENAME );

    if( FAILED( OpenTTFontFile( g_pButtonFont, gc_pwszFONT_BUTTON_FILENAME ) ) )
        DebugPrint( "Init():Failed to open the button font!! - '%ls'\n", gc_pwszFONT_BODY_FILENAME );
}

// Close and clean up our fonts
void CleanupFonts( void )
{
    if( FAILED( CloseTTFont( g_pHeaderFont ) ) )
        DebugPrint( "CleanUp():Failed to close the Header Font!!\n" );

    if( FAILED( CloseTTFont( g_pBodyFont ) ) )
        DebugPrint( "CleanUp():Failed to close the Body Font!!\n" );

    if( FAILED( CloseTTFont( g_pButtonFont ) ) )
        DebugPrint( "CleanUp():Failed to close the Button Font!!\n" );
}

// Return a pointer to our Header Font
XFONT* GetHeaderFont( void )
{
    return g_pHeaderFont;
}

// Return a pointer to our Body Font
XFONT* GetBodyFont( void )
{
    return g_pBodyFont;
}

// Return a pointer to our Button Font
XFONT* GetButtonFont( void )
{
    return g_pButtonFont;
}

// Return a pointer to the current menu
CXItem* GetCurrentMenu( void )
{
    return g_pCurrentScreen;
}

// Set the current Menu
void SetCurrentMenu( CXItem* pMenuPtr )
{
    g_pCurrentScreen = pMenuPtr;

    // Make sure we call the enter function of our new menu
    GetCurrentMenu()->Enter();
}

// Return a pointer to the previous Menu
CXItem* GetPreviousMenu( void )
{
    return g_pPrevScreen;
}

// Set the previous Menu
void SetPreviousMenu( CXItem* pMenuPtr )
{
    g_pPrevScreen = pMenuPtr;
}

//////////////////////////////
// Memory Units / Areas
//////////////////////////////

// Can be used to determine if there have been any changes to our Memory Units
BOOL GetMUsChanged( void )
{
    return g_bMUsChanged;
}

// Initialize our MUs and mount any that need mounting
void InitializeMUs( void )
{
    // Initialize our Memory Units
    g_MemoryUnits[0].SetPortSlot( XDEVICE_PORT0, XDEVICE_TOP_SLOT );
    g_MemoryUnits[1].SetPortSlot( XDEVICE_PORT0, XDEVICE_BOTTOM_SLOT );
    g_MemoryUnits[2].SetPortSlot( XDEVICE_PORT1, XDEVICE_TOP_SLOT );
    g_MemoryUnits[3].SetPortSlot( XDEVICE_PORT1, XDEVICE_BOTTOM_SLOT );
    g_MemoryUnits[4].SetPortSlot( XDEVICE_PORT2, XDEVICE_TOP_SLOT );
    g_MemoryUnits[5].SetPortSlot( XDEVICE_PORT2, XDEVICE_BOTTOM_SLOT );
    g_MemoryUnits[6].SetPortSlot( XDEVICE_PORT3, XDEVICE_TOP_SLOT );
    g_MemoryUnits[7].SetPortSlot( XDEVICE_PORT3, XDEVICE_BOTTOM_SLOT );

    // Mount any MU that is inserted
    g_dwInsertedMUs = XGetDevices( XDEVICE_TYPE_MEMORY_UNIT );
    for( unsigned int x = 0; x < 8; ++x )
    {
        if( g_dwInsertedMUs & gc_dwMUBitMask[x] )
        {
            DebugPrint( "Mounting... %d\n", x );
            g_MemoryUnits[x].Mount();
        }
    }
}

// Check to see if an MU has been inserted or removed, and properly Mounts, or Dismounts the MUs
void ProcessMUs( void )
{
    DWORD dwMUInsertions;
    DWORD dwMURemovals;

    // If the state has changed, update our texture and our previous mask
    g_bMUsChanged = XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, &dwMUInsertions, &dwMURemovals );
    if( g_bMUsChanged )
    {
        // Mount or Unmount our MU's
        for( unsigned int x = 0; x < 8; ++x )
        {
            // Check to see if the MU was removed
            if( dwMURemovals & gc_dwMUBitMask[x] )
            {
                // Remove the MU from our Mask
                g_dwInsertedMUs = g_dwInsertedMUs & ( ~ gc_dwMUBitMask[x] );

                // Unmount the MU
                g_MemoryUnits[x].Unmount();

            }

            // Check to see if the MU is inserted
            if( dwMUInsertions & gc_dwMUBitMask[x] )
            {
                // Add the MU to our Mask
                g_dwInsertedMUs = g_dwInsertedMUs | gc_dwMUBitMask[x];

                // Mount the MU
                g_MemoryUnits[x].Mount();
            }
        }
    }
}

// Get a pointer to a memory unit based on index
CMemoryUnit* GetMUPtr( unsigned int uiIndex )
{
    if( uiIndex > 8 )
        return NULL;

    return &g_MemoryUnits[uiIndex];
}

// Load configuration from a file
HRESULT LoadXBEConfigs( CXBEInfo* pXBEInfo, const char* pszLoadPath )
{
    if( NULL == pszLoadPath || NULL == pXBEInfo )
    {
        DebugPrint( "LoadXBEConfigs():Invalid argument(s) passed in!!\n" );

        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;  // Return code

    // Clear out all configurations for this XBE
    pXBEInfo->ClearTestConfigs();

    char pszFilePath[MAX_PATH+1];
    WCHAR pwszFileLine[MAX_PATH+1];
    char pszValue[MAX_PATH+1];
    WCHAR pwszVariable[64];
    WCHAR* pwszEnd = NULL;
    

    // Load each .DAT file in order (very important)
    // Must start with _0 and count up.  If there are missing numbers, it will miss them
    for( unsigned int uiConfNum = 0; uiConfNum < 1000; uiConfNum++ )
    {
        pszFilePath[MAX_PATH] = '\0';
        pwszFileLine[MAX_PATH] = L'\0';
        pszValue[MAX_PATH] = '\0';
        pwszVariable[63] = L'\0';

        // Process each config file for the specified test
        if( pszLoadPath[strlen( pszLoadPath ) - 1] != '\\' )
            sprintf( pszFilePath, "%hs\\%hs_%u.dat", pszLoadPath, pXBEInfo->GetFilename(), uiConfNum );
        else
            sprintf( pszFilePath, "%hs%hs_%u.dat", pszLoadPath, pXBEInfo->GetFilename(), uiConfNum );

        if( !FileExists( pszFilePath ) )
            break;

        // Process the file

        // Create a new test config (based on defaults) to work with
        CTestConfig* pTestConfig = new CTestConfig( *(pXBEInfo->GetDefaultTestConfig()) );
        if( NULL == pTestConfig )
        {
            DebugPrint( "LoadXBEConfigs():Unable to allocate memory!!\n" );
            hr = E_OUTOFMEMORY;

            break;
        }

        FILE* pfConfigFile = fopen( pszFilePath, "rt" );
        if( NULL == pfConfigFile )
        {
            DebugPrint( "LoadXBEConfigs():Failed to open the config file!! - '%hs'\n", pszFilePath );
            hr = E_FAIL;

            delete pTestConfig;
            pTestConfig = NULL;

            break;
        }

        /*
        // Check the version in the file
        fgetws( pwszFileLine, MAX_PATH, pfConfigFile );

        // Remove the new-line
        pwszEnd = wcschr( pwszFileLine, '\n' );
        if( pwszEnd ) *pwszEnd = L'\0';

        // Read and process the version line
        if( swscanf( pwszFileLine, L"%l63[^=]= %h255[^\0]", pwszVariable, pszValue ) != 2 )
        {
            DebugPrint( "Version was not processed!! Should be first line in the file!!\n" );

            delete pTestConfig;
            pTestConfig = NULL;

            fclose( pfConfigFile );
            pfConfigFile = NULL;

            continue;
        }
        else
        {
            if( ( 0 != _wcsicmp( pwszVariable, L"version" ) ) ||
                ( pTestConfig->GetConfigVersion() != atol( pszValue ) ) )
            {
                DebugPrint( "Version was not processed!! Should be first line in the file!!\n" );

                delete pTestConfig;
                pTestConfig = NULL;

                fclose( pfConfigFile );
                pfConfigFile = NULL;

                continue;
            }
        }
        */

        // Process each line of the file, storing the new value
        while( NULL != fgetws( pwszFileLine, MAX_PATH, pfConfigFile ) )
        {
            // DebugPrint( "pwszFileLine - '%ls'\n", pwszFileLine );

            // Remove the new-line
            pwszEnd = wcschr( pwszFileLine, '\n' );
            if( pwszEnd ) *pwszEnd = L'\0';

            // Make sure we have a vaild data line
            if( !( ( pwszFileLine[0] > L'0' && pwszFileLine[0] < L'9' ) ||
                   ( pwszFileLine[0] > L'A' && pwszFileLine[0] < L'Z' ) ||
                   ( pwszFileLine[0] > L'a' && pwszFileLine[0] < L'z' ) ) )
                continue;

            // Remove any comments that might be in the line
            WCHAR* pwszC = wcschr( pwszFileLine, ';' );
            if( NULL != pwszC ) *pwszC = L'\0';
            pwszC = wcschr( pwszFileLine, L'[' );
            if( NULL != pwszC ) *pwszC = L'\0';

            RightTrim( pwszFileLine );

            // Process the file line
            pwszVariable[0] = L'\0';
            pszValue[0] = '\0';
            if( swscanf( pwszFileLine, L"%l63[^=]= %h255[^\0]", pwszVariable, pszValue ) == 0 )
            {
                DebugPrint( "Unable to process the current file line!! - '%ls'\n", pwszFileLine );

                continue;
            }

            RightTrim( pwszVariable );

            // DebugPrint( "pwszVariable - '%ls' = '%hs'\n", pwszVariable, pszValue );
            
            // Check our current values
            pTestConfig->MoveFirstConfigItem();
            do
            {
                // Check to see if we've already processed this item.  If so, skip it
                if( pTestConfig->GetCurrentConfigItem()->GetProcessed() )
                    continue;

                // If we find the item, process it
                if( 0 == _wcsicmp( pTestConfig->GetCurrentConfigItem()->GetFieldName(), pwszVariable ) )
                {
                    if( 0 == _wcsicmp( pTestConfig->GetCurrentConfigItem()->GetType(), L"number" ) )
                    {
                        CConfigNumber* pConfigNumber = (CConfigNumber*)pTestConfig->GetCurrentConfigItem();

                        pConfigNumber->ProcessValue( pszValue );
                    }
                    else if( 0 == _wcsicmp( pTestConfig->GetCurrentConfigItem()->GetType(), L"range" ) )
                    {
                        CConfigRange* pConfigRange = (CConfigRange*)pTestConfig->GetCurrentConfigItem();

                        pConfigRange->ProcessValue( pszValue );
                    }
                    else if( 0 == _wcsicmp( pTestConfig->GetCurrentConfigItem()->GetType(), L"string" ) )
                    {
                        CConfigString* pConfigString = (CConfigString*)pTestConfig->GetCurrentConfigItem();

                        pConfigString->ProcessValue( pszValue );
                    }
                    else if( 0 == _wcsicmp( pTestConfig->GetCurrentConfigItem()->GetType(), L"string2" ) )
                    {
                        CConfigString2* pConfigString2 = (CConfigString2*)pTestConfig->GetCurrentConfigItem();

                        pConfigString2->ProcessValue( pszValue );
                    }
                    else if( 0 == _wcsicmp( pTestConfig->GetCurrentConfigItem()->GetType(), L"numberstring" ) )
                    {
                        CConfigNumberString* pConfigNumberString = (CConfigNumberString*)pTestConfig->GetCurrentConfigItem();

                        pConfigNumberString->ProcessValue( pszValue );
                    }

                    break;
                }
            } while( pTestConfig->MoveNextConfigItem() );
        }

        // Close our file
        fclose( pfConfigFile );
        pfConfigFile = NULL;

        // Add our test config to our XBE
        pXBEInfo->AddTestConfig( pTestConfig );
    }

    // Check to see if we have added any configs.  If not, add a default
    if( uiConfNum == 0 )
    {
        // Create a New Test Config with the defaults to store in our XBE Object
        CTestConfig* pTestConfig = new CTestConfig( *(pXBEInfo->GetDefaultTestConfig()) );
        if( NULL == pTestConfig )
        {
            DebugPrint( "LoadXBEConfigs():Unable to allocate memory!!\n" );

            return E_OUTOFMEMORY;
        }
        
        pXBEInfo->AddTestConfig( pTestConfig );
    }

    return hr;
}


// Save the current XBE configuration to a .DAT file
HRESULT SaveXBEConfigs( CXBEInfo* pXBEInfo, const char* pszSavePath )
{
    if( NULL == pXBEInfo || NULL == pszSavePath )
    {
        DebugPrint( "SaveXBEConfigs():Invalid argument(s) passed in!!\n" );

        return E_INVALIDARG;
    }

    char pszFilePath[MAX_PATH+1];
    pszFilePath[MAX_PATH] = '\0';

    // DebugPrint( "SaveXBEConfigs():pXBEInfo->pszFilename - '%hs'\n", pXBEInfo->pszFilename );

    // For each configuration, create a separate file
    for( unsigned int x = 0; x < pXBEInfo->GetNumTestConfigs(); x++ )
    {
        // Dump Save Configuration
        if( pszSavePath[strlen(pszSavePath) - 1] != '\\' )
            sprintf( pszFilePath, "%s\\%s_%u.dat", pszSavePath, pXBEInfo->GetFilename(), x );
        else
            sprintf( pszFilePath, "%s%s_%u.dat", pszSavePath, pXBEInfo->GetFilename(), x );

        FILE* pfConfigFile = fopen( pszFilePath, "wt" );
        if( NULL == pfConfigFile )
        {
            DebugPrint( "SaveXBEConfigs():Failed to create the config file!! - '%hs'\n", pszFilePath );

            continue;
        }

        // Write the version
        if( fprintf( pfConfigFile, ";Version=%ld\n", pXBEInfo->GetDefaultTestConfig()->GetConfigVersion() ) < 0 )
            DebugPrint( "SaveXBEConfigs():Failed to write the version!! Version - '%ld'\n", pXBEInfo->GetDefaultTestConfig()->GetConfigVersion() );

        // For each Test Config, write it out to the file
        pXBEInfo->MoveToTestConfig( x );
        CTestConfig* pTestConfig = pXBEInfo->GetCurrentTestConfig();
        pTestConfig->MoveFirstConfigItem();
        do
        {
            if( 0 == _wcsicmp( pTestConfig->GetCurrentConfigItem()->GetType(), L"number" ) )
            {
                CConfigNumber* pConfigNumber = (CConfigNumber*)pTestConfig->GetCurrentConfigItem();

                if( fprintf( pfConfigFile, "%ls=%ld\t\t;%ls\n", pConfigNumber->GetFieldName(),
                                                                pConfigNumber->GetDefault(),
                                                                pConfigNumber->GetHelpText() ) < 0 )
                    DebugPrint( "SaveXBEConfigs():Failed to write the number!!\n" );
            }
            else if( 0 == _wcsicmp( pTestConfig->GetCurrentConfigItem()->GetType(), L"range" ) )
            {
                CConfigRange* pConfigRange = (CConfigRange*)pTestConfig->GetCurrentConfigItem();

                if( fprintf( pfConfigFile, "%ls=%ld-%ld\t\t;%ls\n", pConfigRange->GetFieldName(),
                                                                    pConfigRange->GetDefault1(),
                                                                    pConfigRange->GetDefault2(),
                                                                    pConfigRange->GetHelpText() ) < 0 )
                    DebugPrint( "SaveXBEConfigs():Failed to write the range!!\n" );
            }
            else if( 0 == _wcsicmp( pTestConfig->GetCurrentConfigItem()->GetType(), L"string" ) )
            {
                CConfigString* pConfigString = (CConfigString*)pTestConfig->GetCurrentConfigItem();

                if( fprintf( pfConfigFile, "%ls=%ls\t\t;%ls\n", pConfigString->GetFieldName(),
                                                                pConfigString->GetCurrentString(),
                                                                pConfigString->GetHelpText() ) < 0 )
                    DebugPrint( "SaveXBEConfigs():Failed to write the string!!\n" );
            }
            else if( 0 == _wcsicmp( pTestConfig->GetCurrentConfigItem()->GetType(), L"string2" ) )
            {
                CConfigString2* pConfigString2 = (CConfigString2*)pTestConfig->GetCurrentConfigItem();

                if( fprintf( pfConfigFile, "%ls=%ls\t\t;%ls\n", pConfigString2->GetFieldName(),
                                                                pConfigString2->GetString(),
                                                                pConfigString2->GetHelpText() ) < 0 )
                    DebugPrint( "SaveXBEConfigs():Failed to write the string2!!\n" );
            }
            else if( 0 == _wcsicmp( pTestConfig->GetCurrentConfigItem()->GetType(), L"numberstring" ) )
            {
                CConfigNumberString* pConfigNumberString = (CConfigNumberString*)pTestConfig->GetCurrentConfigItem();

                if( fprintf( pfConfigFile, "%ls=%ld %ls\t\t;%ls\n", pConfigNumberString->GetFieldName(),
                                                                    pConfigNumberString->GetDefault(),
                                                                    pConfigNumberString->GetCurrentString(),
                                                                    pConfigNumberString->GetHelpText() ) < 0 )
                    DebugPrint( "SaveXBEConfigs():Failed to write the numberstring!!\n" );
            }
        } while( pTestConfig->MoveNextConfigItem() );

        fclose( pfConfigFile );
        pfConfigFile = NULL;
    }

    return S_OK;
}


// Will return TRUE if a file exists, otherise FALSE
BOOL FileExists( const char* pszFileName )
{
    if( NULL == pszFileName )
    {
        DebugPrint( "CMemoryArea::FileExists():Invalid parameter passed in!!\n" );

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
        if( ( ERROR_FILE_NOT_FOUND != GetLastError() ) && ( ERROR_NOT_READY != GetLastError() ) && ( ERROR_PATH_NOT_FOUND != GetLastError() ) )
            DebugPrint( "CMemoryArea::FileExists():CreateFile failed!! - '%s', '0x%.8X (%d)'", pszFileName, GetLastError(), GetLastError() );
    }
    else
    {
        bReturn = TRUE;

        CloseHandle( hFile );
        hFile = NULL;
    }

    return bReturn;
}


// Delete a directory and all files within
BOOL DeleteDirectory( const char* pszPath )
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
                continue;

            _snprintf( pszBuf, MAX_PATH, "%s\\%s", pszPath, fd.cFileName );

            if ( ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
            {
                if ( !DeleteDirectory( pszBuf ) )
                    return FALSE;
            }
            else
            {
                if ( !DeleteFile( pszBuf ) )
                    return FALSE;
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
            DebugPrint( "DeleteDirectory():FindFirstFile Failed - '%s', Error - '0x%.8X (%d)'", pszBuf, GetLastError(), GetLastError() );
    }

    BOOL fReturn;

    // Check to see if we are dealing with a rool level directory
    if( ( strlen( pszPath ) == 2 ) && ( pszPath[1] == ':' ) )
        fReturn = TRUE; // Yes we are dealing with a root level directory, keep the directory
    else
        fReturn = RemoveDirectory( pszPath ) != FALSE;  // No we are not dealing with a root level directory, remove the directory

    return fReturn;
}


// Will return a scaled number
long ThumbStickScale( int iStickValue,
                      long lMinValue,
                      long lMaxValue )
{
    // Sub dead-zone
	if( iStickValue > 0 )
        iStickValue -= gc_nJOYSTICK_DEADZONE;
	else
        iStickValue += gc_nJOYSTICK_DEADZONE;

    if( abs( iStickValue ) <= lMinValue )
        return( 1 );

    if( iStickValue > 0 )
        iStickValue -= lMinValue;
    else
        iStickValue += lMinValue;

    // Divide by ( Max - Dead Zone )
    double fValue = (double)iStickValue / (double)( 32768 - gc_nJOYSTICK_DEADZONE - lMinValue );

    // Multiply by Itself
    fValue = fValue * fValue * fValue * fValue * fValue * fValue * fValue * fValue;

    // Multiply by Top end scale value
    long lReturn = (long)( fValue * (double)( lMaxValue - 1 ) ) + 1;

    return lReturn;
}


// Process a configuration file for a specific XBE
HRESULT ProcessConfigFile( CXBEInfo* pXBEInfo )
{
    if( NULL == pXBEInfo )
    {
        DebugPrint( "ProcessConfigFile():Invalid Argument passed in!!\n" );
        return E_INVALIDARG;
    }

    // Pointers to our buffers which will need clean up
    WCHAR* pwszFieldName = NULL;
    WCHAR* pwszHelpText = NULL;
    WCHAR* pwszFieldType = NULL;
    WCHAR* pwszNewValue = NULL;
    WCHAR* pwszFileLine = NULL;
    WCHAR* pwszNewString = NULL;
    FILE* pfConfigFile = NULL;

    // Open the configuration file
    
    char pszConfigFileName[MAX_PATH + 1];
    ZeroMemory( pszConfigFileName, MAX_PATH + 1 );

    _snprintf( pszConfigFileName, MAX_PATH, "d:%s\\%s.cfg", pXBEInfo->GetDirectory(), pXBEInfo->GetFilename() );
    
    pfConfigFile = fopen( pszConfigFileName, "rt" );
    if( NULL == pfConfigFile )
    {
        DebugPrint( "ProcessConfigFile():Failed to open the configuration file!! - '%s'\n", pszConfigFileName );
        return E_FAIL;
    }

    HRESULT hr = S_OK;

    // Allocate memory to hold information from our file
    pwszFileLine = new WCHAR[g_cuiFILE_LINE_SIZE];
    if( NULL == pwszFileLine )
    {
        DebugPrint( "ProcessConfigFile():Failed to allocate memory!!\n" );
        hr = E_OUTOFMEMORY;

        goto cleanup;
    }

    CTestConfig* pDefaultConfig = pXBEInfo->GetDefaultTestConfig(); // Store a handy pointer to our defalut test config

    // Get the config version
    if( NULL == fgetws( pwszFileLine, g_cuiFILE_LINE_SIZE, pfConfigFile ) )
    {
        DebugPrint( "ProcessConfigFile():Failed to read the version file line!!\n" );
        hr = E_FAIL;

        goto cleanup;
    }
    else    // Get the version number
    {
        long lConfigVersion = 0l;

        if( swscanf( pwszFileLine, L"%ld", &lConfigVersion ) != 1 )
        {
            DebugPrint( "ProcessConfigFile():Failed to get the version!! - '%s'\n", pwszFileLine );
            hr = E_FAIL;

            goto cleanup;
        }
        pDefaultConfig->SetConfigVersion( lConfigVersion );
    }

    pwszFieldName = new WCHAR[g_cuiCONFIG_ITEM_SIZE];
    pwszHelpText = new WCHAR[g_cuiCONFIG_ITEM_SIZE];
    pwszFieldType = new WCHAR[g_cuiCONFIG_ITEM_SIZE];
    pwszNewValue = new WCHAR[g_cuiCONFIG_ITEM_SIZE];

    if( ( NULL == pwszFieldName ) || ( NULL == pwszHelpText ) || ( NULL == pwszFieldType ) || ( NULL == pwszNewValue ) )
    {
        hr = E_OUTOFMEMORY;
        goto cleanup;
    }

    while( NULL != fgetws( pwszFileLine, g_cuiFILE_LINE_SIZE, pfConfigFile ) )
    {
        // Make sure we have a vaild data line
        if( !( ( pwszFileLine[0] > L'0' && pwszFileLine[0] < L'9' ) ||
               ( pwszFileLine[0] > L'A' && pwszFileLine[0] < L'Z' ) ||
               ( pwszFileLine[0] > L'a' && pwszFileLine[0] < L'z' ) ) )
            continue;

        // Remove any comments that might be in the line
        WCHAR* wc = wcschr( pwszFileLine, ';' );
        if( NULL != wc ) *wc = L'\0';
        wc = wcschr( pwszFileLine, L'[' );
        if( NULL != wc ) *wc = L'\0';

        RightTrim( pwszFileLine );

        // If we fail to parse the line, return an error
        if( swscanf( pwszFileLine, L"%[^,], \"%[^\"]\", %[^,]", pwszFieldName, pwszHelpText, pwszFieldType ) != 3 )
        {
            DebugPrint( "ProcessConfigFile():Failed to properly parse line!! - '%ls'\n", pwszFileLine );
            hr = E_FAIL;
            break;
        }

        // DebugPrint( "Name - '%ls', Help - '%ls', Type - '%ls'\n", pwszFieldName, pwszHelpText, pwszFieldType );

        // Determine what type of field we are dealing with
        if( 0 == _wcsicmp( pwszFieldType, L"number" ) )
        {
            long lDef1, lMin1, lMax1;

            if( swscanf( pwszFileLine, L"%[^,], \"%[^\"]\", %[^,], %ld, %ld, %ld", pwszFieldName, pwszHelpText, pwszFieldType, &lDef1, &lMin1, &lMax1 ) != 6 )
            {
                DebugPrint( "ProcessConfigFile():NUMBER: Failed to properly parse line!! - '%ls'\n", pwszFileLine );

                hr = E_FAIL;
                break;
            }

            CConfigNumber* pConfigNumber = new CConfigNumber;
            if( NULL == pConfigNumber )
            {
                DebugPrint( "ProcessConfigFile():NUMBER:Failed to allocate memory!!\n" );
                hr = E_OUTOFMEMORY;
                break;
            }

            pConfigNumber->UpdateItem( pwszFieldName, pwszHelpText, pwszFieldType, lDef1, lMin1, lMax1 );

            // Add this item to our list
            pDefaultConfig->AddConfigItem( (CConfigItem*)pConfigNumber );

            /*
            DebugPrint( "NUMBER: '%ls', '%ls', '%ls', DEF:'%ld', MIN:'%ld', MAX:'%ld'\n",
                pConfigNumber->GetFieldName(),
                pConfigNumber->GetHelpText(),
                pConfigNumber->GetType(),
                pConfigNumber->GetDefault(),
                pConfigNumber->GetMinimum(),
                pConfigNumber->GetMaximum() );
            */
        }
        else if( 0 == _wcsicmp( pwszFieldType, L"range" ) )
        {
            long lDef1, lDef2, lMin1, lMin2, lMax1, lMax2;

            if( swscanf( pwszFileLine, L"%[^,], \"%[^\"]\", %[^,], %ld, %ld, %ld, %ld, %ld, %ld", pwszFieldName, pwszHelpText, pwszFieldType, &lDef1, &lMin1, &lMax1, &lDef2, &lMin2, &lMax2 ) != 9 )
            {
                DebugPrint( "ProcessConfigFile():RANGE: Failed to properly parse line!! - '%ls'\n", pwszFileLine );

                hr = E_FAIL;
                break;
            }

            CConfigRange* pConfigRange = new CConfigRange;
            if( NULL == pConfigRange )
            {
                DebugPrint( "ProcessConfigFile():RANGE:Failed to allocate memory!!\n" );
                hr = E_OUTOFMEMORY;
                break;
            }

            pConfigRange->UpdateItem( pwszFieldName, pwszHelpText, pwszFieldType, lDef1, lMin1, lMax1, lDef2, lMin2, lMax2 );

            // Add this item to our list
            pDefaultConfig->AddConfigItem( (CConfigItem*)pConfigRange );

            /*
            DebugPrint( "RANGE: '%ls', '%ls', '%ls', DEF1:'%ld', MIN1:'%ld', MAX1:'%ld', DEF2:'%ld', MIN2:'%ld', MAX2:'%ld'\n",
                pConfigRange->GetFieldName(),
                pConfigRange->GetHelpText(),
                pConfigRange->GetType(),
                pConfigRange->GetDefault1(),
                pConfigRange->GetMinimum1(),
                pConfigRange->GetMaximum1(),
                pConfigRange->GetDefault2(),
                pConfigRange->GetMinimum2(),
                pConfigRange->GetMaximum2() );
            */
        }
        else if( 0 == _wcsicmp( pwszFieldType, L"string" ) )
        {
            CConfigString* pConfigString = new CConfigString;
            if( NULL == pConfigString )
            {
                DebugPrint( "ProcessConfigFile():STRING:Failed to allocate memory!!\n" );
                hr = E_OUTOFMEMORY;
                goto cleanup;
            }

            // Set our field name and help text
            pConfigString->UpdateItem( pwszFieldName, pwszHelpText, pwszFieldType );
            
            // Process each string
            WCHAR* pwszCurrString = pwszFileLine + wcslen( pwszFieldName ) + wcslen( pwszHelpText ) + wcslen( pwszFieldType ) + 8;
            WCHAR* pwszSubString = NULL;
            ZeroMemory( pwszNewValue, sizeof( WCHAR ) * g_cuiCONFIG_ITEM_SIZE );
            while( pwszSubString = wcsstr( pwszCurrString, L"," ) )
            {
                lstrcpyn( pwszNewValue, pwszCurrString, pwszSubString - pwszCurrString + 1 );
                pConfigString->AddString( pwszNewValue );

                // Make sure we don't crash on bad data by checking to see if we have 2 chars to advance accross
                if( wcslen( pwszSubString ) < 2 )
                {
                    DebugPrint( "ProcessConfigFile():Improperly configured string!! - '%ls'\n", pwszSubString );

                    hr = E_FAIL;
                    goto cleanup;
                }
                
                pwszCurrString = pwszSubString + 2;
            }

            // Add the last string
            // TODO: Should we add checking here, to make sure we have a valid string?
            pConfigString->AddString( pwszCurrString );

            // Add this item to our list
            pConfigString->MoveFirstString();

            // Add the config item to our default configuration
            pDefaultConfig->AddConfigItem( (CConfigItem*)pConfigString );

            /*
            DebugPrint( "STRING: '%ls', '%ls', '%ls'",
                pConfigString->GetFieldName(),
                pConfigString->GetHelpText(),
                pConfigString->GetType() );
            
            for( unsigned int x = 0; x < pConfigString->GetNumStrings(); x++ )
            {
                DebugPrint( ", '%ls'", pConfigString->GetString( x ) );
            }
            
            DebugPrint( "\n" );
            */
        }
        else if( 0 == _wcsicmp( pwszFieldType, L"string2" ) )
        {
            pwszNewString = new WCHAR[g_cuiCONFIG_ITEM_SIZE];
            if( NULL == pwszNewString )
            {
                DebugPrint( "ProcessConfigFile():STRING2:Failed to allocate memory!!\n" );
                hr = E_OUTOFMEMORY;
                goto cleanup;
            }

            int iScanFields = swscanf( pwszFileLine, L"%[^,], \"%[^\"]\", %[^,], \"%[^\"]\"", pwszFieldName, pwszHelpText, pwszFieldType, pwszNewString );

            // If it only scanned 3 fields, the last string must be empty, or not there (default to nothing)
            if( 3 == iScanFields )
            {
                pwszNewString[0] = L'\0';
            }
            else if( 4 != iScanFields ) // Make sure that if it didn't scan three fields, it scanned 4
            {
                DebugPrint( "ProcessConfigFile():STRING2: Failed to properly parse line!! - '%ls'\n", pwszFileLine );
                hr = E_FAIL;
                goto cleanup;
            }

            CConfigString2* pConfigString2 = new CConfigString2;
            if( NULL == pConfigString2 )
            {
                DebugPrint( "ProcessConfigFile():STRING2:Failed to allocate memory!!\n" );
                hr = E_OUTOFMEMORY;
                goto cleanup;
            }

            // Set our field name and help text
            // TODO: Should we add checking here, to make sure we have a valid string?
            pConfigString2->UpdateItem( pwszFieldName, pwszHelpText, pwszFieldType, pwszNewString );

            // Add this item to our list
            pDefaultConfig->AddConfigItem( (CConfigItem*)pConfigString2 );

            /*
            DebugPrint( "STRING2: '%ls', '%ls', '%ls', '%ls'\n",
                pConfigString2->GetFieldName(),
                pConfigString2->GetHelpText(),
                pConfigString2->GetType(),
                pConfigString2->GetString() );
            */
        }
        else if( 0 == _wcsicmp( pwszFieldType, L"numberstring" ) )
        {
            long lDef1, lMin1, lMax1;

            if( swscanf( pwszFileLine, L"%[^,], \"%[^\"]\", %[^,], %ld, %ld, %ld", pwszFieldName, pwszHelpText, pwszFieldType, &lDef1, &lMin1, &lMax1 ) != 6 )
            {
                DebugPrint( "ProcessConfigFile():NUMBERSTRING: Failed to properly parse line!! - '%ls'\n", pwszFileLine );
                hr = E_FAIL;
                break;
            }

            CConfigNumberString* pConfigNumberString = new CConfigNumberString;
            if( NULL == pConfigNumberString )
            {
                DebugPrint( "ProcessConfigFile():NUMBERSTRING:Failed to allocate memory!!\n" );
                hr = E_OUTOFMEMORY;
                break;
            }

            // Set our field name, help text, type, and number values
            pConfigNumberString->UpdateItem( pwszFieldName, pwszHelpText, pwszFieldType, lDef1, lMin1, lMax1  );
            
            // Process each string
            WCHAR* pwszSubString = NULL;
            WCHAR* pwszCurrString = pwszFileLine +              // This will point at the first number after our identifiers
                                    wcslen( pwszFieldName ) +
                                    wcslen( pwszHelpText ) + 
                                    wcslen( pwszFieldType ) +
                                    8;

            // Find the position in the string that is after all of the numbers
            for( unsigned int x = 0; x < 3; x++ )
                pwszCurrString = wcsstr( pwszCurrString, L"," ) + 1;

            // Move the pointer ahead to so we can skip the first space after the numbers
            pwszCurrString++;

            // Process each string that is left in the line
            ZeroMemory( pwszNewValue, sizeof( WCHAR ) * g_cuiCONFIG_ITEM_SIZE );
            while( pwszSubString = wcsstr( pwszCurrString, L"," ) )
            {
                lstrcpyn( pwszNewValue, pwszCurrString, pwszSubString - pwszCurrString + 1 );
                pConfigNumberString->AddString( pwszNewValue );

                // Make sure we don't crash on bad data by checking to see if we have 2 chars to advance accross
                if( wcslen( pwszSubString ) < 2 )
                {
                    DebugPrint( "ProcessConfigFile():Improperly configured string!! - '%ls'\n", pwszSubString );
                    hr = E_FAIL;
                    goto cleanup;
                }
                
                pwszCurrString = pwszSubString + 2;
            }

            // Add the last string
            // TODO: Should we add checking here, to make sure we have a valid string?
            pConfigNumberString->AddString( pwszCurrString );

             // Add this item to our list
            pConfigNumberString->MoveFirstString();

            // Add the configuration item to our default version
            pDefaultConfig->AddConfigItem( (CConfigItem*)pConfigNumberString );

            /*
            DebugPrint( "NUMBERSTRING: '%ls', '%ls', '%ls', '%ld', '%ld', '%ld'",
                pConfigNumberString->GetFieldName(),
                pConfigNumberString->GetHelpText(),
                pConfigNumberString->GetType(),
                pConfigNumberString->GetDefault(),
                pConfigNumberString->GetMinimum(), 
                pConfigNumberString->GetMaximum() );
            
            for( unsigned int x = 0; x < pConfigNumberString->GetNumStrings(); x++ )
            {
                DebugPrint( ", '%ls'", pConfigNumberString->GetString( x ) );
            }
            
            DebugPrint( "\n" );
            */
        }
    }

cleanup:
    // Clean up our allocated memory and close our file
    if( NULL != pwszFileLine ) delete[] pwszFileLine;
    if( NULL != pwszFieldName ) delete[] pwszFieldName;
    if( NULL != pwszHelpText ) delete[] pwszHelpText;
    if( NULL != pwszFieldType ) delete[] pwszFieldType;
    if( NULL != pwszNewValue ) delete[] pwszNewValue;
    if( NULL != pwszNewString ) delete[] pwszNewString;
    if( NULL != pfConfigFile ) fclose( pfConfigFile );

    // Set all our pointers to NULL
    pwszFileLine = NULL;
    pwszFieldName = NULL;
    pwszHelpText = NULL;
    pwszFieldType = NULL;
    pwszNewValue = NULL;
    pwszNewString = NULL;
    pfConfigFile = NULL;

    return hr;
}


// Return a pointer to our HVS Launcher XBE Info object
CXBEInfo* GetLauncherXBEInfo( void )
{
    return &g_LauncherXBEInfo;
}


// Initialize the HVS Launcher Config if one exists
void InitLauncherXBEInfo( void )
{
    g_LauncherXBEInfo.SetDirectory( "" );
    g_LauncherXBEInfo.SetFilename( "default" );
    g_LauncherXBEInfo.SetFileExtension( "xbe" );
    g_LauncherXBEInfo.SetLoaded( FALSE );
    g_LauncherXBEInfo.SetTitleName( L"HVS Launcher" );

    if( FAILED( ProcessConfigFile( &g_LauncherXBEInfo ) ) )
    {
        DebugPrint( "InitLauncherXBEInfo():Failed to ProcessConfigFile()!!\n");
        g_LauncherXBEInfo.ClearTestConfigs();

        // Create a bogus Test Config so we don't crash
        CTestConfig* pTestConfig = new CTestConfig;
        if( NULL == pTestConfig )
        {
            DebugPrint( "InitLauncherXBEInfo():Failed to allocate memory!!\n");                
            return;
        }

        CConfigString2* pConfigString2 = new CConfigString2;
        if( NULL == pConfigString2 )
        {
            DebugPrint( "InitLauncherXBEInfo():Failed to allocate memory!!\n");
            delete pTestConfig;
            pTestConfig = NULL;
            return;
        }

        // Update our string and add it to our test config
        pConfigString2->UpdateItem( L"DefaultString", L"Error or no Default Config File Found", L"string2", L"This config was auto-generated" );
        pTestConfig->AddConfigItem( (CConfigItem*)pConfigString2 );

        // Add the new test config to our XBE Info for the Launcher
        g_LauncherXBEInfo.AddTestConfig( pTestConfig );
    }
    else
    {
        // Add a new test configuration based off of the default configuration for the HVS Launcher
        CTestConfig* pTestConfig = new CTestConfig;
        if( NULL == pTestConfig )
        {
            DebugPrint( "InitLauncherXBEInfo():Failed to allocate memory!!\n");                
            return;
        }

        *pTestConfig = *(g_LauncherXBEInfo.GetDefaultTestConfig());

        // Add the new test config to our XBE Info for the Launcher
        g_LauncherXBEInfo.AddTestConfig( pTestConfig );
    }

    // DebugPrint( "Num Launcher Configs - '%u'\n", g_LauncherXBEInfo.GetNumTestConfigs() );
}