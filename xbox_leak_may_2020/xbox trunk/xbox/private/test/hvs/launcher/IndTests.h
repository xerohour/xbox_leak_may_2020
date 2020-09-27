/*****************************************************
*** indtests.h
***
*** Header file for our Individual Tests Menu class.
*** This is a standard menu contains a list of menu
*** items to be selected.
***
*** by James N. Helm
*** November 26th, 2001
***
*****************************************************/

#ifndef _INDTESTS_H_
#define _INDTESTS_H_

#include "menuscreen.h"
#include "xbelist.h"
#include "testconfigmenu.h"
#include "resultsmenu.h"
#include "rommenu.h"

enum INDTESTS_ACTIONS
{
    ENUM_INDTESTSMENU_NOACTION,
    ENUM_INDTESTSMENU_SAVECONFIG_NAME,
    ENUM_INDTESTSMENU_SAVECONFIG_MA_SELECT,
    ENUM_INDTESTSMENU_LOADCONFIG_MA_SELECT,
    ENUM_INDTESTSMENU_LOADCONFIG2_SG_SELECT,
    ENUM_INDTESTSMENU_RESETCONFIG_DATA
};

const WCHAR  gc_pwszINDTESTS_NO_TEST_MESSAGE[] =    L"No tests were found.";
const float  gc_fINDTESTS_NO_TEST_TEXT_YPOS =       125.0f;

class CIndTests : public CMenuScreen
{
public:
    // Constructors and Destructor
    CIndTests( void );
    ~CIndTests( void );

    // Public Methods
    void Action( CUDTexture* pTexture );                        // Renders to the texture and calls any per-frame processing
    HRESULT Init( XFONT* pMenuItemFont,                         // Initialize the Menu
                  XFONT* pMenuTitleFont );                      
    void HandleInput( enum BUTTONS buttonPressed,               // Handles input (of the BUTTONS) for the current menu
                      BOOL bFirstPress );
    void HandleInput( enum CONTROLS controlPressed,             // Handles input from the controls
                      BOOL bFirstPress );
    void Enter( void );                                         // This should be called whenever the menu is entered or activated
    HRESULT LaunchSuiteFromSave( const char* pszPath,           // Launch a test suite from a path and a Saved Game Name
                                 const WCHAR* pwszSaveName );

    WCHAR* GetConfigName( void ) { return m_pwszConfigName; };  // Get the name of the current configuration
    void SetConfigNameInfo( WCHAR* pwszConfigName );            // Set the config name info

private:                                                        
    CXBEList              m_XBEList;                            // List of XBEs (tests) that this menu will deal with
    enum INDTESTS_ACTIONS m_eCurrentAction;                     // Used to track the current action of the user
    WCHAR*                m_pwszConfigName;                     // Used to hold the name of the current configuration loaded

    unsigned short        m_usILoopCount;                       // Used to track the inner loop test number
    unsigned short        m_usILoopMax;                         // Used to track how many total inner loop tests there are
    unsigned short        m_usCurrentDefConfig;                 // Used to track the currently running default config (outer loop number)
    unsigned short        m_usTotalDefConfigs;                  // Used to determine how many total global test configs there are (outer loop total)
                                                            
    // Sub-Menus                                            
    CTestConfigMenu m_TestConfigMenu;
    CResultsMenu m_ResultsMenu;
    CROMMenu m_ROMMenu;
                               
    // Methods
    void RenderMenuItems( CUDTexture* pTexture );               // Render the menu items properly on our menu
    HRESULT LoadSuiteConfiguration( const char* pszDirectory ); // Load a configuration suite specified by the user
    HRESULT SaveSuiteConfiguration( void );                     // Save a configuration suite specified by the user
    void ProcessMenuActions( void );                            // If the user is performing an action, this will handle it
    void ProcessRunningTests( void );                           // Handle it if the launcher is running a set of tests
    void SelectAllOrNone( void );                               // Will select all of the tests, or none of the tests, depending upon current select status

    HRESULT GenerateTestRunInfoFile( char* pszCurrentTest );    // Create the Test Run Info file for the current test run
    HRESULT ReadTestRunInfoFile( char* pszCurrentTest,          // Read the Test Run Info file for the current test run
                                 char* pszNextTest,
                                 WCHAR* pwszConfigName );
    HRESULT StoreRunningTestConfig( void );                     // Write the configurations of the tests the user is running to the T:\ drive
    HRESULT StoreLatestResults( char* pszTestName,              // Write the previously run test info to the results file
                                unsigned __int64 uiPassed,      
                                unsigned __int64 uiFailed,
                                char* pszTestNotes );    
    void LaunchTest( char* pszTestName,                         // Launch the correct test passing the write config information
                     unsigned int uiConfigNumber );
    void LaunchTestSuite( void );                               // Launch the test suite that is currently loaded
    BOOL ItemsHighlighted( void );                              // Will return TRUE if any item in our list is highlighted
    void GoToHelp( void );                                      // Sets up and goes to the help menu
    int ProcessConfigItemForLaunch( CConfigItem* pConfigItem,   // Process a config item to be passed to the test we are launching
                                    char* pszBuffer );
	void DisplayLeftMenu( void );                               // Display the menu items for the 'left' dpad button
	void DisplayRightMenu( void );								// Display the menu items for the 'right' dpad button
    void ClearAllConfigData( void );                            // Reset the program to it's initial state (nothing loaded, no configs)
    void ClearConfigNameInfo( void );                           // Clear the config name and path info
    unsigned short GetTotalNumberTestsToRun( void );            // Calculate the total number of tests to run
    unsigned short GetTotalNumberTestsToRunMinusGlobal( void ); // Calculate the total number of tests to run without factoring in the global config data


    // Menu commands
    void RunTests( void );
    void EditConfig( void );
    void ToggleTestSelect( void );
    void ToggleTestSelectON( void );
    void DisplayResults( void );
    void SaveSuite( void );
    void LoadSuite( void );
};

#endif // _INDTESTS_H_