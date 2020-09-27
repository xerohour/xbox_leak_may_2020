/*****************************************************
*** resultsmenu.h
***
*** Header file for our Results Menu class.
*** This class will parse the results file and
*** display the data to the user
***
*** by James N. Helm
*** December 7th, 2001
***
*****************************************************/

#ifndef _RESULTSMENU_H_
#define _RESULTSMENU_H_

class CTestResult;

#include "menuscreen.h"

const WCHAR  gc_pwszRESULTS_NO_RESULTS_MESSAGE[] =      L"No test results were found.";
const float  gc_fRESULTS_NO_RESULTS_TEXT_YPOS =         175.0f;
const unsigned int g_cuiRESULTSFILE_LINE_SIZE =         4096;       // Size of the buffer to read info from our file
const unsigned int g_cuiRESULTS_TEST_NOTES_SIZE =       3072;       // Size of the buffer for our test notes
const unsigned int g_cuiRESULTS_TEST_NAME_SIZE =        MAX_PATH;   // Size of the buffer for our test name

class CResultsMenu : public CMenuScreen
{
public:
    // Constructors and Destructor
    CResultsMenu( void );
    ~CResultsMenu( void );

    // Public Methods
    HRESULT Init( XFONT* pMenuItemFont,             // Initialize the Menu
                  XFONT* pMenuTitleFont );

    void HandleInput( enum CONTROLS controlPressed, // Handles input from the controls
                      BOOL bFirstPress );

    void RenderMenuItems( CUDTexture* pTexture );   // Used to render the menu items on the screen

    void Enter( void );                             // This should be called whenever the menu is entered or activated

private:
    // Properties
    CLinkedList< CTestResult* > m_TestResults;      // List of test results
    BOOL m_bFileProcessed;                          // Used to determine if the results file was already processed

    // Methods
    HRESULT ProcessResultsFile( void );             // Parse the results file and create the list of Passed / Fail info
    void GoToHelp( void );                          // Sets up and goes to the help menu
    void UpdateScreenItems( void );                 // Updates the screen items depending upon screen dimensions
};


////////////////////////////////////////
////////////////////////////////////////
//// CTestResult Class
////////////////////////////////////////
////////////////////////////////////////
class CTestResult
{
public:
    CTestResult( void );
    ~CTestResult( void );

    WCHAR* GetTestName( void )                              // Get the name of the test
        { return m_pwszTestName; };
    unsigned __int64 GetPassed( void )                      // Get the number of passes for this test
        { return m_ui64Passed; };
    unsigned __int64 GetFailed( void )                      // Get the number of failures for this test
        { return m_ui64Failed; };
    char* GetTestNotes( void )                              // Get the notes for the test
        { return m_pszTestNotes; };

    void SetTestName( WCHAR* pszTestName );                 // Set the name of the test
    void SetPassed( unsigned __int64 ui64Passed )           // Set the number of passes for this test
        { m_ui64Passed = ui64Passed; };
    void SetFailed( unsigned __int64 ui64Failed )           // Set the number of fails for this test
        { m_ui64Failed = ui64Failed; };
    void SetTestNotes( char* pszTestNotes );                // Store the test notes
    void Update( WCHAR* pwszTestName,                       // Update all fields at once
                 unsigned __int64 ui64Passed,               
                 unsigned __int64 ui64Failed,
                 char* pszTestNotes );

    // Operators
    BOOL operator>( const CTestResult& item );
    BOOL operator<( const CTestResult& item );
    BOOL operator==( const CTestResult& item );

private:
    WCHAR* m_pwszTestName;
    unsigned __int64 m_ui64Passed;
    unsigned __int64 m_ui64Failed;
    char* m_pszTestNotes;
};

#endif // _RESULTSMENU_H_