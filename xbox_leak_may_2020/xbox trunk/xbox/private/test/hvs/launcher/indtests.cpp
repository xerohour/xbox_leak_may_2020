/*****************************************************
*** indtests.cpp
***
*** CPP file for our Individual Tests Menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** November 26th, 2001
***
*****************************************************/

#include "stdafx.h"
#include "indtests.h"

// Constructors and Destructor
CIndTests::CIndTests( void ) :
m_eCurrentAction( ENUM_INDTESTSMENU_NOACTION ),
m_pwszConfigName( NULL ),
m_usILoopCount( 0 ),
m_usILoopMax( 0 ),
m_usCurrentDefConfig( 0 ),
m_usTotalDefConfigs( 0 )
{
}

CIndTests::~CIndTests( void )
{
    ClearConfigNameInfo();
}

// Renders to the texture and calls any per-frame processing
void CIndTests::Action( CUDTexture* pTexture )
{
    // Make sure we handle any user initiated actions
    ProcessMenuActions();

    if( m_eCurrentAction != ENUM_INDTESTSMENU_NOACTION )
        return;

    // Call the base class function
    CMenuScreen::Action( pTexture );
}

// Render the menu items properly on our menu
void CIndTests::RenderMenuItems( CUDTexture* pTexture )
{
    if( 0 == GetNumMenuItems() )
    {
        // Display Message Here
        float fXPos = gc_fMENU_AREA_HORIZ_CENTER - ( GetStringPixelWidth( GetBodyFont(), gc_pwszINDTESTS_NO_TEST_MESSAGE ) / 2 );
        float fYPos = gc_fINDTESTS_NO_TEST_TEXT_YPOS;

        pTexture->DrawText( GetBodyFont(), fXPos, fYPos, m_dwMenuItemEnabledFGColor, m_dwMenuItemEnabledBGColor, L"%ls", gc_pwszINDTESTS_NO_TEST_MESSAGE );
    }
    else
    {
        //
        // Render the total number of tests that will be run
        //
        WCHAR pwszNumTests[MAX_PATH+1];
        pwszNumTests[MAX_PATH] = L'\0';

        _snwprintf( pwszNumTests, MAX_PATH, L"[%hu test(s)]", GetTotalNumberTestsToRun() );

        float fXPos = ( m_fRightMenuAreaBoundary - m_fRightMenuAreaBorder ) - GetStringPixelWidth( GetBodyFont(), pwszNumTests );
        float fYPos = 30;

        pTexture->DrawText( GetBodyFont(), fXPos, fYPos, m_dwMenuItemHighlightedFGColor, m_dwMenuItemHighlightedBGColor, L"%ls", pwszNumTests );

        //
        // Render the name of the current configuration (if there is one)
        //
        if( NULL != GetConfigName() )
        {
            int iLen = GetStringPixelWidth( GetBodyFont(), L"[]" );
            fXPos = ( m_fRightMenuAreaBoundary - m_fRightMenuAreaBorder ) - GetStringPixelWidth( GetBodyFont(), GetConfigName() ) - iLen;
            fYPos = 60;

            pTexture->DrawText( GetBodyFont(), fXPos, fYPos, m_dwMenuItemHighlightedFGColor, m_dwMenuItemHighlightedBGColor, L"[%ls]", GetConfigName() );
        }

        CMenuScreen::RenderMenuItems( pTexture );
    }
}

// Initialize the Menu
HRESULT CIndTests::Init( XFONT* pMenuItemFont, XFONT* pMenuTitleFont )
{
    HRESULT hr = CMenuScreen::Init( pMenuItemFont, pMenuTitleFont );
    if( FAILED( hr ) )
    {
        DebugPrint( "CIndTests::Init():Failed to initialize base class!!\n" );

        return hr;
    }

	// Set the menu title
	SetMenuTitle( pMenuTitleFont, L"Test List" );

    /////////////////////////
    // Add screen items here
    /////////////////////////
    AddBitmapItem( "d:\\media\\images\\background.bmp", 0, 0 );

    DisplayLeftMenu();

    ///////////////////////
	// Add menu items here
    ///////////////////////

    // Find each test .XBE
    m_XBEList.Init( "d:\\tests", "XBE" );

    // Add each XBE to a structure, and create a menu item for that XBE
    for( unsigned int x = 0; x < m_XBEList.GetNumItems(); x++ )
        AddMenuItem( pMenuItemFont, m_XBEList.GetXBETitle( x ), FALSE, TRUE, x );

    // Initialize sub-menus
    m_TestConfigMenu.Init( pMenuItemFont, pMenuTitleFont );
    m_TestConfigMenu.SetBackPtr( this );

    m_ResultsMenu.Init( pMenuItemFont, pMenuTitleFont );
    m_ResultsMenu.SetBackPtr( this );

    m_ROMMenu.Init( pMenuItemFont, pMenuTitleFont );
    m_ROMMenu.SetBackPtr( this );

    return hr;
}


// Handles input (of the CONTROLS) for the current menu
void CIndTests::HandleInput( enum CONTROLS controlPressed, BOOL bFirstPress )
{
    if( CONTROL_START != controlPressed && 
		CONTROL_BACK != controlPressed &&
		CONTROL_DPAD_LEFT != controlPressed &&
		CONTROL_DPAD_RIGHT != controlPressed &&
        CONTROL_LEFT_THUMB != controlPressed &&
        CONTROL_RIGHT_THUMB != controlPressed )
    {
        CMenuScreen::HandleInput( controlPressed, bFirstPress );

        return;
    }

    // Process the default button delay
    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Controls being pressed
    switch( controlPressed )
    {
    case CONTROL_BACK:
        {
            GoToHelp();

            break;
        }
    case CONTROL_START:
        {
            RunTests();

            break;
        }
	case CONTROL_DPAD_LEFT:
		{
			UpdateTexture();

			DisplayLeftMenu();

			break;
		}
	case CONTROL_DPAD_RIGHT:
		{
			UpdateTexture();

			DisplayRightMenu();

			break;
		}
    case CONTROL_LEFT_THUMB:
        {
            SelectAllOrNone();

            break;
        }
    case CONTROL_RIGHT_THUMB:
        {
            // Don't do anything if there aren't any menu items
            if( 0 == GetNumMenuItems() )
                return;

            // Ask the user if they really want to clear all data
            m_eCurrentAction = ENUM_INDTESTSMENU_RESETCONFIG_DATA;

            GetYesNoPtr()->SetBackPtr( this );
            GetYesNoPtr()->SetMenuTitle( GetBodyFont(), L"Clear Test Configurations" );
            GetYesNoPtr()->SetText( L"Are you sure that you wish to clear\nALL configuration data for ALL tests?" );

            Leave( GetYesNoPtr() );

            break;
        }
	}
}


// Handles input (of the BUTTONS) for the current menu
void CIndTests::HandleInput( enum BUTTONS buttonPressed, BOOL bFirstPress )
{
    // Process the default button delay
    if( !HandleInputDelay( bFirstPress ) )
        return;

    // Handle Buttons being pressed
    switch( buttonPressed )
    {
    case BUTTON_A:
        {
            ToggleTestSelectON();
            EditConfig();

            break;
        }
    case BUTTON_B:
        {
            GoBack();

            break;
        }
    case BUTTON_X:
        {
            SaveSuite();

            break;
        }
    case BUTTON_Y:
        {
            LoadSuite();

            break;
        }
    case BUTTON_BLACK:
        {
            ToggleTestSelect();

            break;
        }
    case BUTTON_WHITE:
        {
            DisplayResults();            

            break;
        }
    case BUTTON_RIGHT_TRIGGER:
        {
			m_TestConfigMenu.SetMenuTitle( GetHeaderFont(), L"Test Run Globals" );
            m_TestConfigMenu.SetMultiConfigs( TRUE );
            m_TestConfigMenu.SetXBEInfoPtr( GetLauncherXBEInfo() );

            Leave( &m_TestConfigMenu );

            break;
        }
    case BUTTON_LEFT_TRIGGER:
        {
            Leave( &m_ROMMenu );

            break;
        }
    }
}


// Will return TRUE if any item in our list is highlighted
BOOL CIndTests::ItemsHighlighted( void )
{
    // Check to see if there is anything highlighted
    BOOL bFoundItem = FALSE;

    unsigned int uiCurrentIndex = m_MenuItems.GetCurrentIndex();
    m_MenuItems.MoveFirst();
    do
    {
        if( m_MenuItems.GetCurrentNode()->m_bHighlighted )
            bFoundItem = TRUE;
    } while( m_MenuItems.MoveNext() );

    m_MenuItems.MoveTo( uiCurrentIndex );

    return bFoundItem;
}


// Save a configuration suite specified by the user
HRESULT CIndTests::SaveSuiteConfiguration( void )
{
    // Create the Saved Game used to store this configuration
    char pszGamePath[MAX_PATH + 1];
    pszGamePath[MAX_PATH] = '\0';

    char pszDrivePath[4];
    pszDrivePath[3] = '\0';

    if( 8 == GetMemAreaSelectPtr()->GetSelectedItemValue() ) // Xbox Hard Disk
        strcpy( pszDrivePath, "U:\\" );
    else
        strcpy( pszDrivePath, GetMUPtr( GetMemAreaSelectPtr()->GetSelectedItemValue() )->GetDrivePath() );
    WCHAR* pwszGameName = GetKeyboardPtr()->GetValue();

    DebugPrint( "pszDrivePath - '%hs', pwszGameName - '%ls'\n", pszDrivePath, pwszGameName );
    if( ERROR_SUCCESS != XCreateSaveGame( pszDrivePath,
                                          pwszGameName,
                                          CREATE_NEW, // OPEN_ALWAYS,
                                          0,
                                          pszGamePath,
                                          MAX_PATH ) )
    {
        // Delete the game (it might exist) and try again
        if( ERROR_SUCCESS != XDeleteSaveGame( pszDrivePath, pwszGameName ) )
        {
            DebugPrint( "CIndTests::SaveSuiteConfiguration():Failed to delete the existing Saved Game!! - '%hs%ls'\n", pszDrivePath, pwszGameName );

            return E_FAIL;
        }
        else if( ERROR_SUCCESS != XCreateSaveGame( pszDrivePath,
                                                   pwszGameName,
                                                   CREATE_NEW, // OPEN_ALWAYS,
                                                   0,
                                                   pszGamePath,
                                                   MAX_PATH ) )
        {
            DebugPrint( "CIndTests::SaveSuiteConfiguration():Failed to create the Saved Game!! - '%hs%ls'\n", pszDrivePath, pwszGameName );

            return E_FAIL;
        }
    }

    BOOL bConfigFound = FALSE;      // Used to determine if there are any configurations to save

    // For every Highlighted Menu Item, Save the configuration
    m_MenuItems.MoveFirst();
    m_XBEList.MoveFirst();
    do
    {
        if( m_MenuItems.GetCurrentNode()->m_bHighlighted )
        {
            bConfigFound = TRUE;

            // Save the configuration
            if( FAILED( SaveXBEConfigs( m_XBEList.GetCurrentXBEInfoPtr(), pszGamePath ) ) )
                DebugPrint( "CIndTests::SaveSuiteConfiguration():Failed to save the test configuration for - '%ls.xbe'", m_XBEList.GetCurrentXBEInfoPtr()->GetFilename() );
        }

        m_XBEList.MoveNext();
    } while( m_MenuItems.MoveNext() );

    if( 0 != GetLauncherXBEInfo()->GetNumTestConfigs() )
    {
        // Save the overall test run information (comments) entered by the user
        if( FAILED( SaveXBEConfigs( GetLauncherXBEInfo(), pszGamePath ) ) )
            DebugPrint( "CIndTests::SaveSuiteConfiguration():Failed to save the test configuration for - '%ls.xbe'", GetLauncherXBEInfo()->GetFilename() );
    }

    // Check if we found any configs to write
    // If not, delete the saved game we created
    if( !bConfigFound )
    {
        DebugPrint( "SaveXBEConfigs():Nothing to save -- Deleting Save\n" );
        if( ERROR_SUCCESS != XDeleteSaveGame( pszDrivePath, pwszGameName ) )
        {
            DebugPrint( "CIndTests::SaveSuiteConfiguration():Failed to delete the existing Saved Game!! - '%hs%ls'\n", pszDrivePath, pwszGameName );

            return E_FAIL;
        }
    }
    else
    {
        SetConfigNameInfo( pwszGameName );

        //DebugPrint( "Config Name: '%ls'\n", GetConfigName() );
        //DebugPrint( "Config Path: '%hs'\n", GetConfigPath() );
    }

    return S_OK;
}

// Load a configuration suite specified by the user
HRESULT CIndTests::LoadSuiteConfiguration( const char* pszDirectory )
{
    if( NULL == pszDirectory )
    {
        DebugPrint( "CIndTests::LoadSuiteConfiguration():Invalid argument(s) passed in!!\n" );

        return E_INVALIDARG;
    }

    // Clear any loaded configuration that currently exists
    ClearAllConfigData();

    // For every .DAT found in the Save Game, load the config for the .XBE (in order)
    char pszConfigPath[MAX_PATH+1];
    pszConfigPath[MAX_PATH] = '\0';

    int iCount = 0;
    m_XBEList.MoveFirst();
    m_MenuItems.MoveFirst();
    do
    {
        // Check to see if any configs exists for this XBE
        if( pszDirectory[strlen(pszDirectory) - 1] != '\\' )
            _snprintf( pszConfigPath, MAX_PATH, "%hs\\%hs_0.dat", pszDirectory, m_XBEList.GetCurrentXBEInfoPtr()->GetFilename() );
        else
            _snprintf( pszConfigPath, MAX_PATH, "%hs%hs_0.dat", pszDirectory, m_XBEList.GetCurrentXBEInfoPtr()->GetFilename() );

        // If the file exists, load it, otherwise move on
        if( FileExists( pszConfigPath ) )
        {
            // Load the XBE's configs
            if( FAILED( LoadXBEConfigs( m_XBEList.GetCurrentXBEInfoPtr(), pszDirectory ) ) )
            {
                DebugPrint( "CIndTests::LoadSuiteConfiguration():Failed to load the test configuration for - '%ls.xbe'\n", m_XBEList.GetCurrentXBEInfoPtr()->GetFilename() );
            }
            else    // Highlight the menu option
            {
                m_XBEList.GetCurrentXBEInfoPtr()->SetLoaded( TRUE );
                UpdateMenuItem( iCount, m_MenuItems.GetCurrentNode()->m_pwszItemName, TRUE, m_MenuItems.GetCurrentNode()->m_bEnabled );
            }
        }

        iCount++;
        m_MenuItems.MoveNext();
    } while( m_XBEList.MoveNext() );

    // Check to see if we have an overall Test Run Info file.  If so, load it
    if( pszDirectory[strlen(pszDirectory) - 1] != '\\' )
        _snprintf( pszConfigPath, MAX_PATH, "%hs\\%hs_0.dat", pszDirectory, GetLauncherXBEInfo()->GetFilename() );
    else
        _snprintf( pszConfigPath, MAX_PATH, "%hs%hs_0.dat", pszDirectory, GetLauncherXBEInfo()->GetFilename() );

    // If the file exists, load it, otherwise move on
    if( FileExists( pszConfigPath ) )
    {
        // Load the HVS Launcher XBE's configs
        if( FAILED( LoadXBEConfigs( GetLauncherXBEInfo(), pszDirectory ) ) )
            DebugPrint( "CIndTests::LoadSuiteConfiguration():Failed to load the test configuration for - '%ls.xbe'\n", GetLauncherXBEInfo()->GetFilename() );
        else    // Highlight the menu option
            GetLauncherXBEInfo()->SetLoaded( TRUE );
    }

    return S_OK;
}


// If the user is performing an action, this will handle it
void CIndTests::ProcessMenuActions( void )
{
    switch( m_eCurrentAction )
    {
    case ENUM_INDTESTSMENU_SAVECONFIG_NAME:
        {
            if( !GetKeyboardPtr()->GetCancelled() )
            {
                // Get the Configuration Name
                m_eCurrentAction = ENUM_INDTESTSMENU_SAVECONFIG_MA_SELECT;

                GetMemAreaSelectPtr()->SetBackPtr( this );

                Leave( GetMemAreaSelectPtr() );
            }
            else
                m_eCurrentAction = ENUM_INDTESTSMENU_NOACTION;  // Make sure we set this to no action

            break;
        }
    case ENUM_INDTESTSMENU_SAVECONFIG_MA_SELECT:
        {
            // Make sure we set this to no action
            m_eCurrentAction = ENUM_INDTESTSMENU_NOACTION;
            
            if( !GetMemAreaSelectPtr()->GetCancelled() )
                if( FAILED( SaveSuiteConfiguration() ) )
                    DebugPrint( "CIndTests::ProcessMenuActions():SaveSuiteConfiguration() Failed!!\n" );

            break;
        }
    case ENUM_INDTESTSMENU_LOADCONFIG_MA_SELECT:
        {
            if( !GetMemAreaSelectPtr()->GetCancelled() )
            {
                m_eCurrentAction = ENUM_INDTESTSMENU_LOADCONFIG2_SG_SELECT;

                GetChooseSavePtr()->SetBackPtr( this );
                if( 8 == GetMemAreaSelectPtr()->GetSelectedItemValue() ) // Xbox Hard Disk
                    GetChooseSavePtr()->SetInfo( "U:\\" );
                else
                    GetChooseSavePtr()->SetInfo( GetMUPtr( GetMemAreaSelectPtr()->GetSelectedItemValue() )->GetDrivePath() );

                Leave( GetChooseSavePtr() );
            }
            else    // User cancelled
                m_eCurrentAction = ENUM_INDTESTSMENU_NOACTION;  // Make sure we set this to no action

            break;
        }
    case ENUM_INDTESTSMENU_LOADCONFIG2_SG_SELECT:
        {
            // Make sure we set this to no action
            m_eCurrentAction = ENUM_INDTESTSMENU_NOACTION;

            if( !GetChooseSavePtr()->GetCancelled() )
            {
                if( FAILED( LoadSuiteConfiguration( GetChooseSavePtr()->GetCurrentSaveDirectory() ) ) )
                    DebugPrint( "CIndTests::ProcessMenuActions():LoadSuiteConfiguration() Failed!!\n" );
                else // Store the info of the loaded config
                {
                    SetConfigNameInfo( GetChooseSavePtr()->GetCurrentSaveName() );

                    //DebugPrint( "Config Name: '%ls'\n", GetConfigName() );
                    //DebugPrint( "Config Path: '%hs'\n", GetConfigPath() );
                }
            }
            else // User selected back from the Choose Save Menu
            {
                // Get the Saved Game the user wishes to process
                m_eCurrentAction = ENUM_INDTESTSMENU_LOADCONFIG_MA_SELECT;
                GetMemAreaSelectPtr()->SetBackPtr( this );
                Leave( GetMemAreaSelectPtr() );
            }

            break;
        }
    case ENUM_INDTESTSMENU_RESETCONFIG_DATA:
        {
            // Make sure we set this to no action
            m_eCurrentAction = ENUM_INDTESTSMENU_NOACTION;

            // If they didn't cancel, delete all config data
            if( !GetYesNoPtr()->GetCancelled() )
            {
                ClearAllConfigData();
            }

            break;
        }
    }
}


// Write the configurations of the tests the user is running to the T:\ drive
HRESULT CIndTests::StoreRunningTestConfig( void )
{
    // Create the Directory used to store this configuration
    
    if( !CreateDirectory( gc_pszRunningTestsPath, NULL ) )
    {
        // Attempt to delete the directory
        if( !DeleteDirectory( gc_pszRunningTestsPath ) )
        {
            DebugPrint( "CIndTests::StoreRunningTestConfig():Failed to delete the directory!! - '%hs'\n", gc_pszRunningTestsPath );

            return E_FAIL;
        }
        else
        {
            // Create the directory again
            if( !CreateDirectory( gc_pszRunningTestsPath, NULL ) )
            {
                DebugPrint( "CIndTests::StoreRunningTestConfig():Failed to create the directory!! - '%hs'\n", gc_pszRunningTestsPath );

                return E_FAIL;
            }
        }
    }

    BOOL bConfigFound = FALSE;      // Used to determine if there are any configurations to save

    // For every Highlighted Menu Item, Save the configuration
    m_MenuItems.MoveFirst();
    m_XBEList.MoveFirst();
    do
    {
        if( m_MenuItems.GetCurrentNode()->m_bHighlighted )
        {
            bConfigFound = TRUE;

            // Save the configuration
            if( FAILED( SaveXBEConfigs( m_XBEList.GetCurrentXBEInfoPtr(), (char*)gc_pszRunningTestsPath ) ) )
                DebugPrint( "CIndTests::StoreRunningTestConfig():Failed to save the test configuration for - '%ls.xbe'", m_XBEList.GetCurrentXBEInfoPtr()->GetFilename() );
        }

        m_XBEList.MoveNext();
    } while( m_MenuItems.MoveNext() );

    if( 0 != GetLauncherXBEInfo()->GetNumTestConfigs() )
    {
        // Save the overall test run information (comments) entered by the user
        if( FAILED( SaveXBEConfigs( GetLauncherXBEInfo(), (char*)gc_pszRunningTestsPath ) ) )
            DebugPrint( "CIndTests::StoreRunningTestConfig():Failed to save the test configuration for - '%ls.xbe'", GetLauncherXBEInfo()->GetFilename() );
    }

    // Check if we found any configs to write
    // If not, delete the saved game we created
    if( !bConfigFound )
    {
        DebugPrint( "StoreRunningTestConfig():Nothing to run -- Deleting Running Tests Directory\n" );
        if( !DeleteDirectory( gc_pszRunningTestsPath ) )
        {
            DebugPrint( "CIndTests::StoreRunningTestConfig():Failed to delete the directory!! - '%hs'\n", gc_pszRunningTestsPath );

            return E_FAIL;
        }

        return E_ABORT;
    }

    return S_OK;
}


// Read the Test Run Info file for the current test run
HRESULT CIndTests::ReadTestRunInfoFile( char* pszCurrentTest, char* pszNextTest, WCHAR* pwszConfigName )
{
    if( NULL == pszCurrentTest || NULL == pszNextTest || NULL == pwszConfigName )
    {
        DebugPrint( "CIndTests::ReadTestRunInfoFile():Invalid argument(s) passed in!!\n" );
        return E_INVALIDARG;
    }

    FILE* pfCurrentTestInfo = fopen( gc_pszCurrentTestInfoFile, "rt" );
    if( NULL == pfCurrentTestInfo )
    {
        DebugPrint( "CIndTests::ReadTestRunInfoFile():Unable to open the file!! - '%hs'\n", gc_pszCurrentTestInfoFile );
        return E_FAIL;
    }

    // Suite Name
    if( 1 != fscanf( pfCurrentTestInfo, "\"%l255[^\"]\"\n", pwszConfigName ) ) DebugPrint( "CIndTests::ReadTestRunInfoFile():Failed to scan Config Name!!\n" );
    
    // Current inner loop test number
    if( 1 != fscanf( pfCurrentTestInfo, "%hu\n", &m_usILoopCount ) ) DebugPrint( "CIndTests::ReadTestRunInfoFile():Failed to scan Current Inner Loop Tests Info!!\n" );

    // Total inner loop test number
    if( 1 != fscanf( pfCurrentTestInfo, "%hu\n", &m_usILoopMax ) ) DebugPrint( "CIndTests::ReadTestRunInfoFile():Failed to scan Current Inner Loop Total Tests!!\n" );

    // Current Global Default Test Config Number
    if( 1 != fscanf( pfCurrentTestInfo, "%hu\n", &m_usCurrentDefConfig ) ) DebugPrint( "CIndTests::ReadTestRunInfoFile():Failed to scan Current Default Config Number!!\n" );

    // Total Global Default Test Configs
    if( 1 != fscanf( pfCurrentTestInfo, "%hu\n", &m_usTotalDefConfigs ) ) DebugPrint( "CIndTests::ReadTestRunInfoFile():Failed to scan Total Default Config Number!!\n" );
    
    // Current Test Name
    if( 1 != fscanf( pfCurrentTestInfo, "%hs\n", pszCurrentTest ) ) DebugPrint( "CIndTests::ReadTestRunInfoFile():Failed to read the current test file name!!\n" );

    // Test List
    if( ( ( m_usCurrentDefConfig * m_usILoopMax ) + m_usILoopCount + 1 ) == ( m_usTotalDefConfigs * m_usILoopMax ) )
    {
        // If there are no more tests to run, simply return an empty string
        pszNextTest[0] = '\0';
    }
    else // There is a test to run, get the file name
    {
        char pszFirstTestInList[MAX_PATH+1];
        pszFirstTestInList[MAX_PATH] = '\0';

        for( unsigned int x = 0; x < m_usILoopMax; x++ )
        {
            // Read the test filename
            if( 1 != fscanf( pfCurrentTestInfo, "%hs\n", pszNextTest ) ) DebugPrint( "CIndTests::ReadTestRunInfoFile():Failed to read the test file name!!\n" );

            if( x == ( m_usILoopCount + 1 ) )
                break;

            // Store the very first test in our list in case we need to use that test as our 'next' test
            if( 0 == x )
                strncpy( pszFirstTestInList, pszNextTest, MAX_PATH );
        }

        // Check to see if we need to use the first test in our list
        if( x == m_usILoopMax )
            strcpy( pszNextTest, pszFirstTestInList );
    }

    fclose( pfCurrentTestInfo );
    pfCurrentTestInfo = NULL;

    return S_OK;
}


// Create the Test Run Info file for the current test run
HRESULT CIndTests::GenerateTestRunInfoFile( char* pszCurrentTest )
{
    if( NULL == pszCurrentTest )
    {
        DebugPrint( "CIndTests::GenerateTestRunInfoFile():Invalid argument(s) passed in!!\n" );
        return E_INVALIDARG;
    }

    FILE* pfCurrentTestInfo = fopen( gc_pszCurrentTestInfoFile, "wt" );
    if( NULL == pfCurrentTestInfo )
    {
        DebugPrint( "CIndTests::GenerateTestRunInfoFile():Unable to create the file!! - '%hs'\n", gc_pszCurrentTestInfoFile );
        return E_FAIL;
    }

    // Suite Name
    fprintf( pfCurrentTestInfo, "\"%ls\"\n", GetConfigName() );

    // Current inner loop test number
    fprintf( pfCurrentTestInfo, "%hu\n", m_usILoopCount );

    // Total inner loop test number
    fprintf( pfCurrentTestInfo, "%hu\n", m_usILoopMax );

    // Current Global Default Test Config Number
    fprintf( pfCurrentTestInfo, "%hu\n", m_usCurrentDefConfig );

    // Total Number of Global Default Test Configs
    fprintf( pfCurrentTestInfo, "%hu\n", m_usTotalDefConfigs );

    // Current Test File Name
    fprintf( pfCurrentTestInfo, "%hs\n", pszCurrentTest );

    unsigned int uiMenuIndex = m_MenuItems.GetCurrentIndex();
    unsigned int uiXBEListIndex = m_XBEList.GetCurrentIndex();

    // Test List
    m_MenuItems.MoveFirst();
    m_XBEList.MoveFirst();
    do
    {
        if( m_MenuItems.GetCurrentNode()->m_bHighlighted )
        {
            for( unsigned int x = 0; x < m_XBEList.GetCurrentXBEInfoPtr()->GetNumTestConfigs(); x++ )
                fprintf( pfCurrentTestInfo, "%hs_%u.dat\n", m_XBEList.GetCurrentXBEInfoPtr()->GetFilename(), x );
        }

        m_XBEList.MoveNext();
    } while( m_MenuItems.MoveNext() );

    fclose( pfCurrentTestInfo );
    pfCurrentTestInfo = NULL;

    // Restore our indexes
    m_MenuItems.MoveTo( uiMenuIndex );
    m_XBEList.MoveTo( uiXBEListIndex );

    return S_OK;
}


// Launch the correct test passing the right config information
void CIndTests::LaunchTest( char* pszTestName, unsigned int uiConfigNumber )
{
    if( NULL == pszTestName )
    {
        DebugPrint( "CIndTests::LaunchTest():Invalid argument(s) passed in!!\n" );

        return;
    }

    // Find the test in our list
    BOOL bTestFound = FALSE;
    m_XBEList.MoveFirst();
    do
    {
        if( 0 == _stricmp( pszTestName, m_XBEList.GetCurrentXBEInfoPtr()->GetFilename() ) )
        {
            bTestFound = TRUE;
            break;
        }
    } while( m_XBEList.MoveNext() );

    if( !bTestFound )
    {
        DebugPrint( "CIndTests::LaunchTest():Could not find test - '%hs'\n", pszTestName );
        return;
    }

    // Check to make sure the configuration passed in exists
    if( uiConfigNumber >= m_XBEList.GetCurrentXBEInfoPtr()->GetNumTestConfigs() )
    {
        DebugPrint( "CIndTests::LaunchTest():Invalid configuration '%u' passed in for '%hs'!!\n", uiConfigNumber, pszTestName );
        return;
    }

    char pszConfigData[3072];
    ZeroMemory( pszConfigData, 3072 );

    char* pCurrentPos = pszConfigData;
    int iNumBytes = 0;

    // Check to make sure the global configuration passed in exists
    if( m_usCurrentDefConfig >= GetLauncherXBEInfo()->GetNumTestConfigs() )
    {
        DebugPrint( "CIndTests::LaunchTest():Invalid configuration '%hu' specified for the global configs!!\n", m_usCurrentDefConfig );
        return;
    }

    // Process the default config items for the HVS Launcher
    GetLauncherXBEInfo()->MoveToTestConfig( m_usCurrentDefConfig );
    GetLauncherXBEInfo()->GetCurrentTestConfig()->MoveFirstConfigItem();
    do
    {
        iNumBytes = ProcessConfigItemForLaunch( GetLauncherXBEInfo()->GetCurrentTestConfig()->GetCurrentConfigItem(), pCurrentPos );

        pCurrentPos += iNumBytes;
    } while( GetLauncherXBEInfo()->GetCurrentTestConfig()->MoveNextConfigItem() );

    // Process the config items for the current test being launched
    CTestConfig* pTestConfig = m_XBEList.GetCurrentXBEInfoPtr()->GetTestConfig( uiConfigNumber );
    pTestConfig->MoveFirstConfigItem();
    do
    {
        iNumBytes = ProcessConfigItemForLaunch( pTestConfig->GetCurrentConfigItem(), pCurrentPos );

        pCurrentPos += iNumBytes;
    } while( pTestConfig->MoveNextConfigItem() );

    // Construct the path of the XBE to launch
    char pszXBEPath[MAX_PATH + 1];
    ZeroMemory( pszXBEPath, MAX_PATH + 1 );

    _snprintf( pszXBEPath, MAX_PATH, "d:%hs\\%hs.%hs", m_XBEList.GetCurrentXBEInfoPtr()->GetDirectory(),
                                                       m_XBEList.GetCurrentXBEInfoPtr()->GetFilename(),
                                                       m_XBEList.GetCurrentXBEInfoPtr()->GetFileExtension() );

    // Construct the structure to pass to the test
    int nSize = sizeof( ToTestData ) + strlen( pszConfigData ) + 1;
    struct ToTestData* pTestData = (ToTestData*)new char[sizeof(LAUNCH_DATA)];
    ZeroMemory( pTestData, nSize );

    pTestData->titleID = HVSLAUNCHERID;
    pTestData->testNumber = ( m_usCurrentDefConfig * m_usILoopMax ) + m_usILoopCount + 1;
    pTestData->totalTests = m_usTotalDefConfigs * m_usILoopMax;
    strcpy( pTestData->configSettings, pszConfigData );

    // DebugPrint( "CIndTests::LaunchTest():pszTestName - '%hs'\n", pszTestName );
    // DebugPrint( "CIndTests::LaunchTest():pszXBEPath  - '%hs'\n", pszXBEPath );

    LaunchImage( pszXBEPath, (LAUNCH_DATA*)pTestData );
}


// Process a config item to be passed to the test we are launching
int CIndTests::ProcessConfigItemForLaunch( CConfigItem* pConfigItem, char* pszBuffer )
{
    int iReturnVal = 0;

    if( 0 == _wcsicmp( pConfigItem->GetType(), L"number" ) )
    {
        CConfigNumber* pConfigNumber = (CConfigNumber*)pConfigItem;

        // Append each config item to the buffer
        iReturnVal = sprintf( pszBuffer, "%ls=%ld\n", 
            pConfigNumber->GetFieldName(),
            pConfigNumber->GetDefault() );
    }
    else if( 0 == _wcsicmp( pConfigItem->GetType(), L"range" ) )
    {
        CConfigRange* pConfigRange = (CConfigRange*)pConfigItem;

        // Append each config item to the buffer
        iReturnVal = sprintf( pszBuffer, "%ls=%ld-%ld\n", 
            pConfigRange->GetFieldName(),
            pConfigRange->GetDefault1(),
            pConfigRange->GetDefault2() );
    }
    else if( 0 == _wcsicmp( pConfigItem->GetType(), L"string" ) )
    {
        CConfigString* pConfigString = (CConfigString*)pConfigItem;

        // Append each config item to the buffer
        iReturnVal = sprintf( pszBuffer, "%ls=%ls\n", 
            pConfigString->GetFieldName(),
            pConfigString->GetCurrentString() );
    }
    else if( 0 == _wcsicmp( pConfigItem->GetType(), L"string2" ) )
    {
        CConfigString2* pConfigString2 = (CConfigString2*)pConfigItem;

        // Append each config item to the buffer
        iReturnVal = sprintf( pszBuffer, "%ls=%ls\n", 
            pConfigString2->GetFieldName(),
            pConfigString2->GetString() );
    }
    else if( 0 == _wcsicmp( pConfigItem->GetType(), L"numberstring" ) )
    {
        CConfigNumberString* pConfigNumberString = (CConfigNumberString*)pConfigItem;

        // Append each config item to the buffer
        iReturnVal = sprintf( pszBuffer, "%ls=%ld %ls\n", 
            pConfigNumberString->GetFieldName(),
            pConfigNumberString->GetDefault(),
            pConfigNumberString->GetCurrentString() );
    }

    return iReturnVal;
}


// Write the previously run test info to the results file
HRESULT CIndTests::StoreLatestResults( char* pszTestName, unsigned __int64 uiPassed, unsigned __int64 uiFailed, char* pszTestNotes )
{
    if( NULL == pszTestName )
    {
        DebugPrint( "CIndTests::StoreLatestResults():Invalid argument(s) passed in!!\n" );
        return E_FAIL;
    }

    FILE* pfResults = fopen( gc_pszResultsFile, "at" );
    if( NULL == pfResults )
    {
        DebugPrint( "CIndTests::StoreLatestResults():Unable to create the file!! - '%hs'\n", gc_pszResultsFile );
        return E_FAIL;
    }

    // Remove any trailing "\n" from the notes
    if( '\n' == pszTestNotes[strlen(pszTestNotes) - 1] )
        pszTestNotes[strlen(pszTestNotes) - 1] = '\0';

    // Get the test .XBE file name, and get the current config index
    char pszFileName[MAX_PATH+1];
    pszFileName[MAX_PATH] = '\0';
    unsigned int uiConfigNumber = 0;

    char* pszDOTString = strstr( pszTestName, "_" );
    strncpy( pszFileName, pszTestName, pszDOTString - pszTestName );
    pszFileName[pszDOTString - pszTestName] = '\0';

    pszDOTString += 1;
    if( 1 != sscanf( pszDOTString, "%u", &uiConfigNumber ) )
        DebugPrint( "CIndTests::StoreLatestResults():Unabled to determine config number!!\n" );

    CXBEInfo* pXBEInfo = m_XBEList.GetXBEInfoPtr( m_XBEList.GetIndexOfXBE( pszFileName ) );

    // DebugPrint( "%ls (Config %u), %I64u, %I64u\n", pXBEInfo->GetTitleName(), uiConfigNumber, uiPassed, uiFailed );

    fprintf( pfResults, "%ls (Config %u), %I64u, %I64u, \"%hs\"\n", pXBEInfo->GetTitleName(), uiConfigNumber + 1, uiPassed, uiFailed, pszTestNotes );

    fclose( pfResults );
    pfResults = NULL;

    return S_OK;
}


// Handle it if the launcher is running a set of tests
void CIndTests::ProcessRunningTests( void )
{
    // Check to see if we have any data and get the launch data if found
    DWORD dwLaunchDataType = 0l;
    LAUNCH_DATA oLaunchData;
    ZeroMemory( &oLaunchData, sizeof( oLaunchData ) );

    // Check to see if there is any data from a previous test
    if( ERROR_SUCCESS != XGetLaunchInfo( &dwLaunchDataType, &oLaunchData ) )
        return;
    else
    {
        // Check to make sure it's from one of our tests
        if( LDT_TITLE != dwLaunchDataType )
        {
            DebugPrint( "CIndTests::ProcessRunningTests():Data not from one of our tests!!\n" );
            return;
        }
    }

    // We got data, let's process it
    struct ToLauncherData* pLauncherData = (struct ToLauncherData*)&oLaunchData;

    /*
    DebugPrint( "Title ID = 0x%0.8X\n", pLauncherData->titleID );
    DebugPrint( "Passed   = %u\n", pLauncherData->numPass );
    DebugPrint( "Failed   = %u\n", pLauncherData->numFail );
    DebugPrint( "Notes    = '%hs'\n", pLauncherData->notes );
    */

    // Get the information from the previous test

    // If this was our last config for a test, let's delete it's data information
    WCHAR pwszConfigName[MAX_PATH+1];
    char pszCurrentTest[MAX_PATH+1];
    char pszNextTest[MAX_PATH+1];
    ZeroMemory( pwszConfigName, sizeof( WCHAR ) * ( MAX_PATH + 1 ) );
    pszCurrentTest[MAX_PATH] = '\0';
    pszNextTest[MAX_PATH] = '\0';

    if( FAILED( ReadTestRunInfoFile( pszCurrentTest, pszNextTest, pwszConfigName ) ) )
    {
        DebugPrint( "CIndTests::ProcessRunningTests():Failed to read the test run info file!!\n" );
        return;
    }

    /*
    DebugPrint( "Current I    - '%hu'\n", m_usILoopCount );
    DebugPrint( "Total I      - '%hu'\n", m_usILoopMax );
    DebugPrint( "Current O    - '%hu'\n", m_usCurrentDefConfig );
    DebugPrint( "Total O      - '%hu'\n", m_usTotalDefConfigs );
    DebugPrint( "Current Test - '%hs'\n", pszCurrentTest );
    DebugPrint( "Next Test    - '%hs'\n", pszNextTest );
    */

    // Update the Results file with the latest data
    StoreLatestResults( pszCurrentTest, (unsigned __int64)pLauncherData->numPass, (unsigned __int64)pLauncherData->numFail, pLauncherData->notes );

    // Load Configs from Running Test Directory
    LoadSuiteConfiguration( (char*)gc_pszRunningTestsPath );

    // Set our configuration name if necessary
    if( 0 != wcscmp( L"(null)", pwszConfigName ) )
        SetConfigNameInfo( pwszConfigName );

    // Increment our current test to run number and inner loop counter
    m_usILoopCount++;

    if( m_usILoopCount == m_usILoopMax )
    {
        m_usILoopCount = 0;
        m_usCurrentDefConfig++;
    }


    // Check to see if we are done running tests
    if( m_usCurrentDefConfig == m_usTotalDefConfigs )
    {
        // We're done, let's clean up
        DebugPrint( "CIndTests::ProcessRunningTests():No more tests, cleaning up...\n" );

        if( !DeleteDirectory( gc_pszRunningTestsPath ) )
            DebugPrint( "Unable to delete directory - '%hs'\n", gc_pszRunningTestsPath );

        if( !DeleteFile( gc_pszCurrentTestInfoFile ) )
            DebugPrint( "Unable to delete the running test file - '%hs'\n", gc_pszCurrentTestInfoFile );

        // Clear out test number counters
        m_usILoopCount = 0;
        m_usILoopMax = 0;
        m_usCurrentDefConfig = 0;
        m_usTotalDefConfigs = 0;

        // Display the results
        Leave( &m_ResultsMenu );
    }
    else
    {
        // Generate an updated Test Info File
        GenerateTestRunInfoFile( pszNextTest );

        //
        // Launch the next test
        //

        // Get the test name and config number
        char pszFileName[MAX_PATH+1];
        pszFileName[MAX_PATH] = '\0';
        unsigned int uiConfigNumber = 0;

        char* pszDOTString = strstr( pszNextTest, "_" );
        strncpy( pszFileName, pszNextTest, pszDOTString - pszNextTest );
        pszFileName[pszDOTString - pszNextTest] = '\0';

        pszDOTString += 1;
        if( 1 != sscanf( pszDOTString, "%u", &uiConfigNumber ) )
            DebugPrint( "CIndTests::StoreLatestResults():Unabled to determine config number!!\n" );

        LaunchTest( pszFileName, uiConfigNumber );
    }
}


// This should be called whenever the menu is entered or activated
void CIndTests::Enter( void )
{
    // Call the base class enter
    CMenuScreen::Enter();

    // Check to see if we are running tests
    static BOOL bProcessed = FALSE;
    if( !bProcessed )
    {
        bProcessed = TRUE;
        ProcessRunningTests();
    }
}


// Launch a test suite from a path and a Saved Game Name
HRESULT CIndTests::LaunchSuiteFromSave( const char* pszPath, const WCHAR* pwszSaveName )
{
    // Open the Saved Game and verify it exists
    char pszGamePath[MAX_PATH+1];
    pszGamePath[MAX_PATH] = '\0';
    DWORD dwResult = XCreateSaveGame( pszPath,
                                      pwszSaveName,
                                      OPEN_EXISTING,
                                      0,
                                      pszGamePath,
                                      MAX_PATH );

    if( ERROR_SUCCESS != dwResult )
    {
        DebugPrint( "CIndTests::LaunchSuiteFromSave():Failed to open the Saved Game!! Path - '%hs', Name - '%ls'\n", pszPath, pwszSaveName );

        return E_FAIL;
    }

    // Load Configs from Test Directory
    LoadSuiteConfiguration( pszGamePath );

    // Set the name of our Saved Config
    SetConfigNameInfo( (WCHAR*)pwszSaveName );

    // Launch the tests
    RunTests();

    return S_OK;
}


// Launch the test suite that is currently loaded
void CIndTests::LaunchTestSuite( void )
{
    if( E_ABORT == StoreRunningTestConfig() )
    {
        GetMessagePtr()->SetText( L"At least one test must be selected\nin order to launch the test suite." );
        GetMessagePtr()->SetBackPtr( this );
        Leave( GetMessagePtr() );

        return;
    }

    // Find out how many total tests there are to run
    m_usILoopCount = 0;
    m_usILoopMax = GetTotalNumberTestsToRunMinusGlobal();

    m_usCurrentDefConfig = 0;
    m_usTotalDefConfigs = (unsigned short)GetLauncherXBEInfo()->GetNumTestConfigs();

    // Delete the Results File from last run
    DeleteFile( gc_pszResultsFile );

    // Find the first highlighted test and launch it
    m_MenuItems.MoveFirst();
    m_XBEList.MoveFirst();
    do
    {
        if( m_MenuItems.GetCurrentNode()->m_bHighlighted )
        {
            // Create our Test Run Information File
            char pszFileName[MAX_PATH+1];
            pszFileName[MAX_PATH] = '\0';

            _snprintf( pszFileName, MAX_PATH, "%hs_0.dat", m_XBEList.GetCurrentXBEInfoPtr()->GetFilename() );

            GenerateTestRunInfoFile( pszFileName );

            LaunchTest( m_XBEList.GetCurrentXBEInfoPtr()->GetFilename(), 0 );
        }

        m_XBEList.MoveNext();
    } while( m_MenuItems.MoveNext() );
}

// Calculate the total number of tests to run without factoring in the global config data
unsigned short CIndTests::GetTotalNumberTestsToRunMinusGlobal( void )
{
    unsigned short usReturnVal = 0;

    // Find out how many total tests there are to run
    m_MenuItems.MoveFirst();
    m_XBEList.MoveFirst();
    do
    {
        if( m_MenuItems.GetCurrentNode()->m_bHighlighted )
        {
            usReturnVal += (unsigned short) m_XBEList.GetCurrentXBEInfoPtr()->GetNumTestConfigs();
        }
        m_XBEList.MoveNext();
    } while( m_MenuItems.MoveNext() );

    return usReturnVal;
}

// Calculate the total number of tests to run
unsigned short CIndTests::GetTotalNumberTestsToRun( void )
{
    return GetTotalNumberTestsToRunMinusGlobal() * (unsigned short)GetLauncherXBEInfo()->GetNumTestConfigs();
}


// Set the config name and path info
void CIndTests::SetConfigNameInfo( WCHAR* pwszConfigName )
{
    if( NULL == pwszConfigName )
    {
        DebugPrint( "CIndTests::SetConfigNameInfo():Invalid argument(s) passed in!!\n" );
        return;
    }

    // Clean up any memory that may already be in use
    ClearConfigNameInfo();

    m_pwszConfigName = new WCHAR[wcslen( pwszConfigName ) + 1];
    if( NULL == m_pwszConfigName )
    {
        DebugPrint( "CIndTests::SetConfigNameInfo():Unable to allocate memory for Config Name!!\n" );
        return;
    }

    // Copy the name to our member variable
    wcscpy( m_pwszConfigName, pwszConfigName );
}

void CIndTests::RunTests( void )
{
    // Don't do anything if there aren't any menu items
    if( 0 == GetNumMenuItems() )
        return;

    LaunchTestSuite();  // Launch the currently loaded Suite Configuration
}

void CIndTests::EditConfig( void )
{
    ////////////////////////////////////
    // Edit the currently selected test
    ////////////////////////////////////

    // Don't do anything if there aren't any menu items
    if( 0 == GetNumMenuItems() )
        return;

	m_TestConfigMenu.SetMenuTitle( GetHeaderFont(), m_XBEList.GetXBEInfoPtr( GetSelectedItem() )->GetTitleName() );
    m_TestConfigMenu.SetMultiConfigs( TRUE );
    m_TestConfigMenu.SetXBEInfoPtr( m_XBEList.GetXBEInfoPtr( GetSelectedItem() ) );
    Leave( &m_TestConfigMenu );
}

void CIndTests::ToggleTestSelect( void )
{
    //////////////////////////////////////
    // Toggle the currently selected item
    //////////////////////////////////////

    // Don't do anything if there aren't any menu items
    if( 0 == GetNumMenuItems() )
        return;

    UpdateMenuItem( GetSelectedItem(), m_MenuItems.GetNode( GetSelectedItem() )->m_pwszItemName, !m_MenuItems.GetNode( GetSelectedItem() )->m_bHighlighted, m_MenuItems.GetNode( GetSelectedItem() )->m_bEnabled );
}


void CIndTests::ToggleTestSelectON( void )
{
    ////////////////////////////////////////
    // Select the currently highlighted item
    ////////////////////////////////////////

    // Don't do anything if there aren't any menu items
    if( 0 == GetNumMenuItems() )
        return;

    UpdateMenuItem( GetSelectedItem(), m_MenuItems.GetNode( GetSelectedItem() )->m_pwszItemName, TRUE, m_MenuItems.GetNode( GetSelectedItem() )->m_bEnabled );
}


void CIndTests::DisplayResults( void )
{
    // Display the results
    Leave( &m_ResultsMenu );
}

void CIndTests::SaveSuite( void )
{
    //////////////////////////////////////////////////
    // Save Current Suite Configuration to Saved Game
    //////////////////////////////////////////////////

    // Don't do anything if there aren't any menu items
    if( 0 == GetNumMenuItems() )
        return;

    if( !ItemsHighlighted() )
    {
        GetMessagePtr()->SetText( L"At least one test must be selected\nin order to save the test suite." );
        GetMessagePtr()->SetBackPtr( this );
        Leave( GetMessagePtr() );
    }
    else
    {
        // Get the Configuration Name
        m_eCurrentAction = ENUM_INDTESTSMENU_SAVECONFIG_NAME;

        GetKeyboardPtr()->SetBackPtr( this );
        GetKeyboardPtr()->SetInfo( L"Enter the name of the Suite Config File", 40, GetConfigName() );

        Leave( GetKeyboardPtr() );
    }
}

void CIndTests::LoadSuite( void )
{
    //////////////////////////////////////
    // Load Configuration from Saved Game
    //////////////////////////////////////

    // Don't do anything if there aren't any menu items
    if( 0 == GetNumMenuItems() )
        return;

    // Get the Saved Game the user wishes to process
    m_eCurrentAction = ENUM_INDTESTSMENU_LOADCONFIG_MA_SELECT;
    GetMemAreaSelectPtr()->SetBackPtr( this );
    Leave( GetMemAreaSelectPtr() );
}


// Sets up and goes to the help menu
void CIndTests::GoToHelp( void )
{
    // Set the menu title
    WCHAR pwszTitle[50];
    pwszTitle[49] = L'\0';

    _snwprintf( pwszTitle, 49, L"%ls Info", m_MenuItems.GetNode( GetSelectedItem() )->m_pwszItemName );
    GetHelpMenuPtr()->SetMenuTitle( GetHeaderFont(), pwszTitle );

    // Set the back pointer for the help menu
    GetHelpMenuPtr()->SetBackPtr( this );

    // Clear the current help
    GetHelpMenuPtr()->ClearHelpItems();

    // Populate the help menu with the text file info (if it exists)
    char pszFilePath[MAX_PATH+1];
    pszFilePath[MAX_PATH] = '\0';

    _snprintf( pszFilePath, MAX_PATH, "d:%hs\\%hs.txt", m_XBEList.GetXBEInfoPtr( GetSelectedItem() )->GetDirectory(), m_XBEList.GetXBEInfoPtr( GetSelectedItem() )->GetFilename() );

    if( FileExists( pszFilePath ) )
    {
        // Open the file
        FILE* pfInputFile = fopen( pszFilePath, "rt" );
        if( NULL == pfInputFile )
        {
            DebugPrint( "CIndTests::GoToHelp():Failed to open the input file!! - %hs\n", pszFilePath );

            return;
        }

        WCHAR pwszFileLine[MAX_PATH+1];
        pwszFileLine[MAX_PATH] = L'\0';

        while( NULL != fgetws( pwszFileLine, MAX_PATH, pfInputFile ) )
        {
            if( L'\n' == pwszFileLine[wcslen(pwszFileLine) - 1] )
                pwszFileLine[wcslen(pwszFileLine) - 1] = L'\0';

            GetHelpMenuPtr()->AddHelpItem( GetBodyFont(),
                                           pwszFileLine,
                                           GetBodyFont(),
                                           L"" );
        }

        fclose( pfInputFile );
        pfInputFile = NULL;

        // navigate to the menu
        Leave( GetHelpMenuPtr() );
    }

    /*
    m_pXBEInfo->GetCurrentTestConfig()->MoveFirstConfigItem();
    do
    {
        GetHelpMenuPtr()->AddHelpItem( GetBodyFont(),
                                       m_pXBEInfo->GetCurrentTestConfig()->GetCurrentConfigItem()->GetFieldName(),
                                       GetBodyFont(),
                                       m_pXBEInfo->GetCurrentTestConfig()->GetCurrentConfigItem()->GetHelpText() );
    } while( m_pXBEInfo->GetCurrentTestConfig()->MoveNextConfigItem() );
    */
}

// Display the menu items for the 'left' dpad button
void CIndTests::DisplayLeftMenu( void )
{
    // Clear out all screen items except the background bitmap
    unsigned int uiNumItems = m_ScreenItems.GetNumItems();
    for( unsigned int x = 1; x < uiNumItems; x++ )
    {
        m_ScreenItems.DelNode( 1 );
    }

    // Add the Right Arrow to the menu
    AddBitmapAlphaItem( "d:\\media\\images\\rightarrowbig.bmp",
                        "d:\\media\\images\\rightarrowbig_alpha.bmp",
                        (int)( m_fRightMenuAreaBoundary - m_fRightMenuAreaBorder - 19 ),
                        (int)gc_fSTATUS_TEXT_ROW1_YPOS - 7 );


	// Status Bar Column 1
	AddTextItem( GetButtonFont(), L"C",             gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_X_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetBodyFont(),   L" Save Suite",   gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetButtonFont(), L"D",             gc_fSTATUS_TEXT_COL1_XPOS,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_Y_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetBodyFont(),   L" Load Suite",   gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

	// Status Bar Column 2
	AddTextItem( GetButtonFont(), L"A",             gc_fSTATUS_TEXT_COL2_XPOS,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_A_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetBodyFont(),   L" Edit Test",    gc_fSTATUS_TEXT_COL2_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
    AddTextItem( GetButtonFont(), L"I",             gc_fSTATUS_TEXT_COL2_XPOS,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_BLACK_BUTTON,   gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetBodyFont(),   L" Select Test",  gc_fSTATUS_TEXT_COL2_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

	// Status Bar Column 3
	AddTextItem( GetButtonFont(), L"H",             gc_fSTATUS_TEXT_COL3_XPOS - 5,                       gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_BACK_BUTTON,    gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetBodyFont(),   L" Test Info",    gc_fSTATUS_TEXT_COL3_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetButtonFont(), L"G",             gc_fSTATUS_TEXT_COL3_XPOS - 5,                       gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_START_BUTTON,   gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetBodyFont(),   L" Run Test(s)",  gc_fSTATUS_TEXT_COL3_XPOS + gc_fBUTTON_WIDTH_ADJUST, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
}


// Display the menu items for the 'right' dpad button
void CIndTests::DisplayRightMenu( void )
{
    // Clear out all screen items except the background bitmap
    unsigned int uiNumItems = m_ScreenItems.GetNumItems();
    for( unsigned int x = 1; x < uiNumItems; x++ )
    {
        m_ScreenItems.DelNode( 1 );
    }
	
    // Add the Left Arrow to the menu
    AddBitmapAlphaItem( "d:\\media\\images\\leftarrowbig.bmp",
                        "d:\\media\\images\\leftarrowbig_alpha.bmp",
                        (int)gc_fSTATUS_TEXT_COL1_XPOS,
                        (int)gc_fSTATUS_TEXT_ROW1_YPOS - 7 );

    // Status Bar Column 1
	AddTextItem( GetButtonFont(), L"E",                     gc_fSTATUS_TEXT_COL1_XPOS + 27,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_L_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetBodyFont(),   L" Flash ROM",            gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST + 27, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetButtonFont(), L"F",                     gc_fSTATUS_TEXT_COL1_XPOS + 27,                           gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwCOLOR_R_BUTTON,       gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetBodyFont(),   L" Test Run Globals",     gc_fSTATUS_TEXT_COL1_XPOS + gc_fBUTTON_WIDTH_ADJUST + 27, gc_fSTATUS_TEXT_ROW2_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );

	// Status Bar Column 2
	AddTextItem( GetButtonFont(), L"J",                     gc_fSTATUS_TEXT_COL2_XPOS + 27,                           gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwCOLOR_WHITE_BUTTON,   gc_dwSTATUS_TEXT_BG_COLOR );
	AddTextItem( GetBodyFont(),   L" Last Results",         gc_fSTATUS_TEXT_COL2_XPOS + gc_fBUTTON_WIDTH_ADJUST + 27, gc_fSTATUS_TEXT_ROW1_YPOS, gc_dwSTATUS_TEXT_FG_COLOR, gc_dwSTATUS_TEXT_BG_COLOR );
}


// Will select all of the tests, or none of the tests, depending upon current select status
void CIndTests::SelectAllOrNone( void )
{
    BOOL bAllSelected = TRUE;

    m_MenuItems.MoveFirst();
    do
    {
        if( !m_MenuItems.GetCurrentNode()->m_bHighlighted )
        {
            bAllSelected = FALSE;
            break;
        }
    } while( m_MenuItems.MoveNext() );

    // Toggle the select status
    m_MenuItems.MoveFirst();
    do
    {
        UpdateMenuItem( m_MenuItems.GetCurrentIndex(), m_MenuItems.GetCurrentNode()->m_pwszItemName, !bAllSelected, m_MenuItems.GetCurrentNode()->m_bEnabled );
    } while( m_MenuItems.MoveNext() );
}


// Reset the program to it's initial state (nothing loaded, no configs)
void CIndTests::ClearAllConfigData( void )
{
    //
    // Clear global test data (default parameters, etc)
    //

    // Clear out any existing test configurations
    GetLauncherXBEInfo()->ClearTestConfigs();

    // Add 1 global configuration based off of the default config
    CTestConfig* pTestConfig = new CTestConfig;
    if( NULL == pTestConfig )
    {
        DebugPrint( "CIndTests::ClearAllConfigData()):Failed to allocate memory!!\n");
    }
    else
    {
        *pTestConfig = *( GetLauncherXBEInfo()->GetDefaultTestConfig() );

        // Add the new test config to our current XBE
        GetLauncherXBEInfo()->AddTestConfig( pTestConfig );
    }

    // Don't do anything if there aren't any menu items
    if( 0 == GetNumMenuItems() )
        return;

    // De-Select all tests
    m_MenuItems.MoveFirst();
    do
    {
        UpdateMenuItem( m_MenuItems.GetCurrentIndex(), m_MenuItems.GetCurrentNode()->m_pwszItemName, FALSE, m_MenuItems.GetCurrentNode()->m_bEnabled );
    } while( m_MenuItems.MoveNext() );

    // Delete all configs for each test and set config 1 to the default configuration
    m_XBEList.MoveFirst();
    do
    {
        // Clear out any existing test configurations
        m_XBEList.GetCurrentXBEInfoPtr()->ClearTestConfigs();

        // Add 1 configuration based off of the default config for the test
        pTestConfig = new CTestConfig;
        if( NULL == pTestConfig )
        {
            DebugPrint( "CIndTests::ClearAllConfigData()):Failed to allocate memory!!\n");
            continue;
        }

        *pTestConfig = *( m_XBEList.GetCurrentXBEInfoPtr()->GetDefaultTestConfig() );

        // Add the new test config to our current XBE
        m_XBEList.GetCurrentXBEInfoPtr()->AddTestConfig( pTestConfig );
    } while( m_XBEList.MoveNext() );

    // Remove any loaded save name info
    if( NULL != m_pwszConfigName )
    {
        delete[] m_pwszConfigName;
        m_pwszConfigName = NULL;
    }
}

// Clear the config name and path info
void CIndTests::ClearConfigNameInfo( void )
{
    if( NULL != m_pwszConfigName )
    {
        delete[] m_pwszConfigName;
        m_pwszConfigName = NULL;
    }
}